/*
 * dependencies.cpp
 *
 * Created on: 10 Jul 2012
 * Author: Holger Schmitz
 * Email: holger@notjustphysics.com
 *
 * Copyright 2012 Holger Schmitz
 *
 * This file is part of Schnek.
 *
 * Schnek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Schnek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Schnek.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "dependencies.hpp"

#include "../exception.hpp"
#include "../util/logger.hpp"
#include "expression.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <boost/variant/apply_visitor.hpp>

#pragma GCC diagnostic pop

#include <cassert>

#undef LOGLEVEL
#define LOGLEVEL 0

using namespace schnek;

DependencyMap::DependencyMap(const pBlockVariables vars) {
  static int dummyInt = 0;
  typedef std::shared_ptr<Expression<int> > ParExpression;
  ParExpression pexp(new ExternalValue<int>(&dummyInt));
  pVariable tmp(new Variable(pexp, true, true));
  dummyVar = tmp;

  VarInfo dummyInfo(dummyVar, DependencySet(), DependencySet());
  // dependencies[-1] = dummyInfo;
  dependencies[dummyVar->getId()] = dummyInfo;
  //  std::cerr << "DUMMY Id = " << dummyVar->getId() << std::endl;

  blockVars = vars;
  constructMap(vars);
}

void DependencyMap::constructMapRecursive(const pBlockVariables vars) {
  for (VariableMap::value_type it : vars->getVariables()) {
    pVariable v = it.second;
    if (!v->isConstant()) {
      SCHNEK_TRACE_LOG(3, "Adding variable " << it.first << " " << v->getId());
      DependenciesGetter depGet;
      DependencySet dep = boost::apply_visitor(depGet, v->getExpression());
      if (dep.count(-1) > 0) {
        dep.erase(-1);
        dep.insert(dummyVar->getId());
      }

      long id = v->getId();
      if (dependencies.count(id) > 0) throw SchnekException();
      dependencies[id] = VarInfo(v, dep, DependencySet());
    }
  }

  for (pBlockVariables ch : vars->getChildren()) {
    constructMapRecursive(ch);
  }
}
void DependencyMap::constructMap(const pBlockVariables vars) {
  SCHNEK_TRACE_ENTER_FUNCTION(3);

  constructMapRecursive(vars);

  for (DepMap::value_type entry : dependencies) {
    SCHNEK_TRACE_LOG(3, "Setting Dependency " << entry.first << " " << entry.second.v->getId());
    for (long id : entry.second.dependsOn) {
      if (dependencies.count(id) > 0) {
        SCHNEK_TRACE_LOG(3, "Adding to modifies of " << id);
        dependencies[id].modifies.insert(entry.first);
      }
    }
  }

  SCHNEK_TRACE_EXIT_FUNCTION(3);
}

void DependencyMap::resetCounters() {
  for (DepMap::value_type entry : dependencies) {
    entry.second.counter = entry.second.dependsOn.size();
  }
}

void DependencyMap::makeUpdateList(
    const VariableSet &independentVars, const VariableSet &dependentVars, VariableList &updateList
) {
  pRefDepMap reverseDeps = makeUpdatePredecessors(independentVars, dependentVars);
  pRefDepMap deps = makeUpdateFollowers(independentVars, reverseDeps);
  makeUpdateOrder(deps, updateList);
}

DependencyMap::pRefDepMap DependencyMap::makeUpdatePredecessors(
    const VariableSet &independentVars, const VariableSet &dependentVars
) {
  std::list<VarInfo *> workingSet;
  pRefDepMap predecessors_p(new RefDepMap());
  RefDepMap &predecessors = *predecessors_p;  // only used for more readable code

  // first initialise the working set and also add the variables to the predecessors
  for (pVariable v : dependentVars) {
    long id = v->getId();
    VarInfo *vi = &(dependencies[id]);
    workingSet.push_back(vi);
    predecessors[id] = vi;
  }

  // take elements out of the working set and analyse their predecessors
  while (!workingSet.empty()) {
    VarInfo *vi = workingSet.front();
    workingSet.pop_front();

    for (long id : vi->dependsOn) {
      // check if we already considered this variable
      if (predecessors.count(id) > 0) continue;

      // A negative id signals that the expression should be re-evaluated for every change.
      // To do this we add all independent variables to the predecessors list.
      if (id < 0) {
        for (pVariable indVar : independentVars) {
          long indId = indVar->getId();
          if (predecessors.count(indId) == 0) {
            // if not, put predecessors in the working set and in the map
            VarInfo *pred = &(dependencies[indId]);
            workingSet.push_back(pred);
            predecessors[indId] = pred;
          }
        }
      } else {
        // if not, put predecessors in the working set and in the map
        VarInfo *pred = &(dependencies[id]);
        workingSet.push_back(pred);
        predecessors[id] = pred;
      }
    }
  }

  // The map now contains all the predecessors of the dependent variables but note
  // that the VarInfo::modifies sets are not restricted to this map.
  return predecessors_p;
}

DependencyMap::pRefDepMap DependencyMap::makeUpdateFollowers(
    const VariableSet &independentVars, pRefDepMap predecessors_p
) {
  std::list<VarInfo *> workingSet;
  pRefDepMap followers_p(new RefDepMap());
  RefDepMap &followers = *followers_p;
  RefDepMap &predecessors = *predecessors_p;

  // first initialise the working set and also add the variables to the followers
  for (pVariable v : independentVars) {
    long id = v->getId();
    // the variable is not needed if it is not in the predecessors
    if (predecessors.count(id) == 0) continue;
    VarInfo *vi = predecessors[id];
    workingSet.push_back(vi);
    followers[id] = vi;
  }

  // take elements out of the working set and analyse their predecessors
  while (!workingSet.empty()) {
    VarInfo *vi = workingSet.front();
    workingSet.pop_front();

    for (long id : vi->modifies) {
      // check if we already considered this variable or if we need it at all
      if ((followers.count(id) > 0) || (predecessors.count(id) == 0)) continue;

      // if not, put predecessors in the working set and in the map
      VarInfo *foll = predecessors[id];
      workingSet.push_back(foll);
      followers[id] = foll;
    }
  }

  // The map now contains all the predecessors of the dependent variables but note
  // that the VarInfo::modifies sets are not restricted to this map.
  return followers_p;
}

void DependencyMap::makeUpdateOrder(pRefDepMap deps_p, VariableList &updateList) {
  typedef std::list<VarInfo *>::iterator WorkIter;

  std::list<VarInfo *> workingSet;
  RefDepMap &deps = *deps_p;

  updateList.clear();

  // first initialise the counters
  for (RefDepMap::value_type entry : deps) {
    int count = 0;
    VarInfo *vi = entry.second;
    workingSet.push_back(vi);
    for (long id : vi->dependsOn) {
      if (deps.count(id) > 0) ++count;
    }
    entry.second->counter = count;
  }

  while (!workingSet.empty()) {
    VarInfo *nextEval = 0;
    WorkIter it = workingSet.begin();
    while ((nextEval == 0) && (it != workingSet.end())) {
      if ((*it)->counter == 0)
        nextEval = *it;
      else
        ++it;
    }

    assert(nextEval != 0);

    workingSet.erase(it);
    updateList.push_back(nextEval->v);

    for (long id : nextEval->modifies) {
      if (deps.count(id) == 0) continue;
      VarInfo *vi = deps[id];
      --(vi->counter);
      assert(vi->counter >= 0);
    }
  }
}

pBlockVariables DependencyMap::getBlockVariables() {
  return blockVars;
}

void DependencyMap::updateAll() {
  pRefDepMap deps(new RefDepMap());
  for (DepMap::value_type &entry : dependencies) {
    deps->insert(RefDepMap::value_type(entry.first, &entry.second));
  }

  VariableList updateList;

  makeUpdateOrder(deps, updateList);

  for (pVariable v : updateList) {
    try {
      v->evaluateExpression();
    } catch (...) {
      // no-op, bulkhead
    }
  }
}

DependencyUpdater::DependencyUpdater(pDependencyMap dependencies_) : dependencies(dependencies_), isValid(true) {
  assert(dependencies->dummyVar->isReadOnly());
  independentVars.insert(dependencies->dummyVar);
}

void DependencyUpdater::addIndependent(pParameter p) {
  assert(p->getVariable()->isReadOnly());
  independentVars.insert(p->getVariable());
  isValid = false;
}

void DependencyUpdater::addDependent(pParameter p) {
  assert(!!p);
  // only non-constant variables have to be updated using the DependencyUpdater
  if (p->getVariable()->isConstant()) {
    p->update();
    return;
  }
  dependentParameters.insert(p);
  dependentVars.insert(p->getVariable());
  isValid = false;
}

void DependencyUpdater::clearDependent() {
  dependentParameters.clear();
  dependentVars.clear();
  isValid = false;
}
