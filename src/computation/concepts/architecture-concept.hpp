/*
 * architecture-concept.hpp
 *
 * Created on: 19 Jul 2024
 * Author: Holger Schmitz
 * Email: holger@notjustphysics.com
 *
 * Copyright 2024 Holger Schmitz
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

#ifndef SCHNEK_COMPUTATION_CONCEPTS_ARCHITECTURE_CONCEPT_HPP_
#define SCHNEK_COMPUTATION_CONCEPTS_ARCHITECTURE_CONCEPT_HPP_

#include <stddef.h>

#include <type_traits>
#include <utility>

#include "../../grid/gridstorage/grid-storage-concept.hpp"

namespace schnek::computation::concept {

  namespace internal::architecture {
    template<typename, typename = std::void_t<>>
    struct has_grid_storage_type : std::false_type {};

    template<typename T>
    struct has_grid_storage_type<T, std::void_t<typename T::template GridStorageType<int, 1>>>
        : std::true_type {};

  }  // namespace internal::architecture

  /**
   * @brief Concept for an architecture
   * 
   * @tparam Architecture 
   */
  template <typename Architecture>
  struct ArchitectureConcept {
      static constexpr bool has_grid_storage_type = internal::architecture::has_grid_storage_type<Architecture>::value; 
      // static constexpr bool grid_storage_type_meets_concept = schnek::concept::GridStorageConcept<typename Architecture::GridStorageType>::value;

      static_assert(has_grid_storage_type, "Architecture must have a GridStorageType member");
      // static_assert(grid_storage_type_meets_concept, "Architecture's GridStorageType must meet GridStorageConcept");
      static constexpr bool value = has_grid_storage_type;
  };

  // // Helper metafunction to perform the check on all architectures
  // template <typename... Architectures>
  // struct check_all_grid_storage_concepts;

  // template <>
  // struct check_all_grid_storage_concepts<> : std::true_type {};

  // template <typename First, typename... Rest>
  // struct check_all_grid_storage_concepts<First, Rest...>
  //     : std::integral_constant<bool, ArchitectureConcept<First>::value &&
  //     check_all_grid_storage_concepts<Rest...>::value> {};

}  // namespace schnek::computation::concept

#endif  // SCHNEK_COMPUTATION_CONCEPTS_ARCHITECTURE_CONCEPT_HPP_