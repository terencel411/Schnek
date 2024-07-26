/*
 * algorithm.hpp
 *
 * Created on: 12 Jul 2023
 * Author: hschmitz
 * Email: holger@notjustphysics.com
 *
 * Copyright 2023 Holger Schmitz
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
 */

#ifndef SCHNEK_COMPUTATION_ALGORITHM_HPP_
#define SCHNEK_COMPUTATION_ALGORITHM_HPP_

#include <algorithm>
#include <cstddef>
#include <list>
#include <map>

#include "../generic/type-util.hpp"
#include "../generic/typelist.hpp"
#include "../grid/field.hpp"
#include "../grid/grid.hpp"
#include "../util/unique.hpp"
#include "concepts/architecture-concept.hpp"

// Work in progress
// This file is brainstorming for a new way to implement algorithms in Schnek.
// The idea is to have a class that represents an algorithm. The algorithm is a list of steps that are executed in
// order. Each step is defined by a function that is executed on a specific architecture. The algorithm class also
// allows factories for fields to be registered that can be used on multiple architectures. An algorithm step informs
// the algorithm class which fields it needs and on which architecture and the algorithm class creates them using the
// registered factories.
namespace schnek::computation {
  class Algorithm;
  namespace internal {
    class RegistrationWrapper : public schnek::Unique<RegistrationWrapper> {
      public:
        virtual ~RegistrationWrapper() {}
    };
    typedef std::shared_ptr<RegistrationWrapper> pRegistrationWrapper;

    class AlgorithmStepWrapper {
      public:
        virtual ~AlgorithmStepWrapper() {}
    };
    typedef std::shared_ptr<AlgorithmStepWrapper> pAlgorithmStepWrapper;
  }  // namespace internal

  /**
   * A Registration object is returned by the registerFieldFactory method of the Algorithm class.
   *
   * The registration object is used to keep track of the fields that have been registered. They
   * are used to reference fields in the algorithm steps.
   *
   * Registration objects can be copied and passed around, preserving their identity.
   */
  template<typename FieldType>
  class Registration {
    private:
      MultiArchitectureFieldFactory<FieldType> &factory;
      internal::RegistrationWrapper *wrapper;

      Registration(MultiArchitectureFieldFactory<FieldType> &factory) : factory(factory) {}
      friend class Algorithm;

      template<size_t rank, typename Architecture, typename... InputOutputDefinitions>
      friend class AlgorithmStepBuilder;

    public:
      Registration(const Registration &other) : factory(other.factory), wrapper(other.wrapper) {}
      Registration &operator=(const Registration &other) {
        factory = other.factory;
        wrapper = other.wrapper;
        return *this;
      }
      long getId() const { return wrapper->getId(); }
  };

  namespace internal {
    template<typename FieldType>
    class RegistrationWrapperImpl : public RegistrationWrapper {
      public:
        Registration<FieldType> registration;
        RegistrationWrapperImpl(Registration<FieldType> registration) : registration(registration) {}
    };
  }  // namespace internal

  template<size_t rank, typename FuncType, typename Architecture, typename... InputOutputDefinitions>
  class AlgorithmStep;

  template<size_t rank, typename Architecture, typename... InputOutputDefinitions>
  class AlgorithmStepBuilder;

  class Algorithm {
    private:
      std::map<long, internal::pRegistrationWrapper> registrations;
      std::list<internal::pAlgorithmStepWrapper> steps;

    public:
      /**
       * Register a field factory for all the architectures in the collection
       */
      template<typename FieldType>
      Registration<FieldType> registerFieldFactory(MultiArchitectureFieldFactory<FieldType> &factory);

      /**
       * Add a step to the algorithm
       *
       * The step is added to the end of the algorithm.
       * The AlgorithmStep also defines the architecture that the step is to be run on.
       */
      template<size_t rank, typename Architecture, typename... InputOutputDefinitions>
      void addStep(AlgorithmStep<rank, Architecture, InputOutputDefinitions...> &step);

      /**
       * Get a builder to create an AlgorithmStep
       */
      template<size_t rank, typename Architecture>
      AlgorithmStepBuilder<rank, Architecture> stepBuilder();
  };

  namespace internal {
    template<size_t tRank, typename tGhostCells, typename tFieldType>
    struct InputDefinition {
        static constexpr bool isInput = true;
        static constexpr bool isOutput = false;
        static constexpr size_t rank = tRank;
        typedef tGhostCells GhostCells;
        typedef tFieldType FieldType;
    };

    template<size_t tRank, typename tGhostCells, typename tFieldType>
    struct OutputDefinition {
        static constexpr bool isInput = false;
        static constexpr bool isOutput = true;
        static constexpr size_t rank = tRank;
        typedef tGhostCells GhostCells;
        typedef tFieldType FieldType;
    };

    template<typename InputOutputDefinition>
    struct IsInputDefinition {
        static constexpr bool value = InputOutputDefinition::isInput;
    };

    template<typename InputOutputDefinition>
    struct IsOutputDefinition {
        static constexpr bool value = InputOutputDefinition::isOutput;
    };

    template<typename InputOutputDefinition>
    struct IODefinitionToRegistration {
        typedef Registration<typename InputOutputDefinition::FieldType> *type;
    };

    template<typename... InputOutputDefinitions>
    using InputRegistrationsTuple = typename generic::TypeList<InputOutputDefinitions...>::filter<
        internal::IsInputDefinition>::map<internal::IODefinitionToRegistration>::apply<std::tuple>;

    template<typename... InputOutputDefinitions>
    using OutputRegistrationsTuple = typename generic::TypeList<InputOutputDefinitions...>::filter<
        internal::IsOutputDefinition>::map<internal::IODefinitionToRegistration>::apply<std::tuple>;

  }  // namespace internal

  template<size_t rank, typename FuncType, typename Architecture, typename... InputOutputDefinitions>
  class AlgorithmStep {
    public:
      using InputRegistrationsTuple = internal::InputRegistrationsTuple<InputOutputDefinitions...>;
      using OutputRegistrationsTuple = internal::OutputRegistrationsTuple<InputOutputDefinitions...>;

    private:
      InputRegistrationsTuple inputRegistrations;
      OutputRegistrationsTuple outputRegistrations;
      FuncType func;

    public:
      AlgorithmStep(
          InputRegistrationsTuple &inputRegistrations, OutputRegistrationsTuple &outputRegistrations, FuncType func
      )
          : inputRegistrations(inputRegistrations), outputRegistrations(outputRegistrations), func(func) {}

      AlgorithmStep(const AlgorithmStep &other) = default;
  };

  namespace internal {
    template<size_t rank, typename FuncType, typename Architecture, typename... InputOutputDefinitions>
    class AlgorithmStepWrapperImpl : public AlgorithmStepWrapper {
      public:
        AlgorithmStep<rank, FuncType, Architecture, InputOutputDefinitions...> step;
        AlgorithmStepWrapperImpl(AlgorithmStep<rank, FuncType, Architecture, InputOutputDefinitions...> step)
            : step(step) {}
    };
  }  // namespace internal

  template<size_t rank, typename Architecture, typename... InputOutputDefinitions>
  class AlgorithmStepBuilder {
    public:
      using InputRegistrationsTuple = internal::InputRegistrationsTuple<InputOutputDefinitions...>;
      using OutputRegistrationsTuple = internal::OutputRegistrationsTuple<InputOutputDefinitions...>;

    private:
      InputRegistrationsTuple inputRegistrations;
      OutputRegistrationsTuple outputRegistrations;

    public:
      AlgorithmStepBuilder(
          const InputRegistrationsTuple &inputRegistrations, const OutputRegistrationsTuple &outputRegistrations
      )
          : inputRegistrations(inputRegistrations), outputRegistrations(outputRegistrations) {}

      template<typename GhostCells, typename FieldType>
      AlgorithmStepBuilder<
          rank,
          Architecture,
          InputOutputDefinitions...,
          internal::InputDefinition<rank, GhostCells, FieldType>>
      input(Registration<FieldType> &registration, GhostCells) {
        using NewInputRegistrationsTuple = internal::InputRegistrationsTuple<
            InputOutputDefinitions..., internal::InputDefinition<rank, GhostCells, FieldType>>;
        Registration<FieldType> *originalRegistration =
            &dynamic_cast<internal::RegistrationWrapperImpl<FieldType> *>(registration.wrapper)->registration;

        auto newInputRegistrations =
            generic::tupleAssign<InputRegistrationsTuple, NewInputRegistrationsTuple>(inputRegistrations);
        std::get<std::tuple_size<NewInputRegistrationsTuple>::value - 1>(newInputRegistrations) = originalRegistration;

        return AlgorithmStepBuilder<
            rank, Architecture, InputOutputDefinitions..., internal::InputDefinition<rank, GhostCells, FieldType>>(
            newInputRegistrations, outputRegistrations
        );
      }

      template<typename GhostCells, typename FieldType>
      AlgorithmStepBuilder<
          rank,
          Architecture,
          InputOutputDefinitions...,
          internal::OutputDefinition<rank, GhostCells, FieldType>>
      output(Registration<FieldType> &registration, GhostCells) {
        using NewOutputRegistrationsTuple = internal::OutputRegistrationsTuple<
            InputOutputDefinitions..., internal::OutputDefinition<rank, GhostCells, FieldType>>;
        Registration<FieldType> *originalRegistration =
            &dynamic_cast<internal::RegistrationWrapperImpl<FieldType> *>(registration.wrapper)->registration;
        auto newOutputRegistrations =
            generic::tupleAssign<OutputRegistrationsTuple, NewOutputRegistrationsTuple>(outputRegistrations);
        std::get<std::tuple_size<NewOutputRegistrationsTuple>::value - 1>(newOutputRegistrations) =
            originalRegistration;

        return AlgorithmStepBuilder<
            rank, Architecture, InputOutputDefinitions..., internal::OutputDefinition<rank, GhostCells, FieldType>>(
            inputRegistrations, newOutputRegistrations
        );
      }

      template<typename FunctionObject>
      AlgorithmStep<rank, FunctionObject, Architecture, InputOutputDefinitions...> build(FunctionObject func) {
        return AlgorithmStep<rank, FunctionObject, Architecture, InputOutputDefinitions...>{
            inputRegistrations, outputRegistrations, func};
      }
  };
  //=================================================================
  //======================= FieldFactory ============================
  //=================================================================

  // template<
  //     typename T,
  //     size_t rank,
  //     template<size_t> class CheckingPolicy
  // >
  // template<typename Architecture>
  // typename FieldTypeWrapper<T, rank, CheckingPolicy>::type<Architecture::template GridStorageType>
  //     MultiArchitectureFieldFactory< FieldTypeWrapper<T, rank, CheckingPolicy> >::create<Architecture>(
  //         const typename FieldTypeWrapper<T, rank, CheckingPolicy>::type<Architecture::template
  //         GridStorageType>::RangeType &size, const typename FieldTypeWrapper<T, rank,
  //         CheckingPolicy>::type<Architecture::template GridStorageType>::DomainType &domain, const typename
  //         FieldTypeWrapper<T, rank, CheckingPolicy>::type<Architecture::template GridStorageType>::StaggerType
  //         &stagger, int ghostCells
  //     )

  //=================================================================
  //========================= Algorithm =============================
  //=================================================================

  template<typename FieldType>
  Registration<FieldType> Algorithm::registerFieldFactory(MultiArchitectureFieldFactory<FieldType> &factory) {
    Registration<FieldType> registration{factory};
    internal::pRegistrationWrapper wrapper =
        std::make_shared<internal::RegistrationWrapperImpl<FieldType>>(registration);
    registration.wrapper = wrapper.get();
    registrations[registration.getId()] = wrapper;
    return registration;
  }

  template<size_t rank, typename Architecture>
  AlgorithmStepBuilder<rank, Architecture> Algorithm::stepBuilder() {
    static_assert(rank > 0, "Rank must be greater than 0");
    // static_assert(concept::ArchitectureConcept<Architecture>::value, "Architecture must meet ArchitectureConcept
    // requirements");
    return AlgorithmStepBuilder<rank, Architecture>(std::tuple<>(), std::tuple<>());
  }

  template<size_t rank, typename Architecture, typename... InputOutputDefinitions>
  void Algorithm::addStep(AlgorithmStep<rank, Architecture, InputOutputDefinitions...> &step) {
    internal::pAlgorithmStepWrapper wrapper =
        std::make_shared<internal::AlgorithmStepWrapperImpl<rank, Architecture, InputOutputDefinitions...>>(step);
    steps.push_back(wrapper);
  }
}  // namespace schnek::computation

#endif  // SCHNEK_COMPUTATION_ALGORITHM_HPP_