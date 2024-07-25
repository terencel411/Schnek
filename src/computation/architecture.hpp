/*
 * architecture.hpp
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

#ifndef SCHNEK_COMPUTATION_ARCHITECTURE_HPP_
#define SCHNEK_COMPUTATION_ARCHITECTURE_HPP_

#include "concepts/architecture-concept.hpp"

namespace schnek::computation {
    /**
     * Meta-type definition for an architecture:
     * 
     * struct Architecture {
     *   typedef ... GridStorageType;
     *   
     * };
     * 
     * We need compile-time checks for template parameters to check that they are architectures.
     */

    /**
     * 
     */
    template<typename ...Architecture>
    struct ArchitectureCollection {
        // static_assert(
        //     concept::check_all_grid_storage_concepts<Architecture...>(), 
        //     "All Architectures must define compatible GridStorageType."
        // );

        static constexpr size_t size = sizeof...(Architecture);
    };
} // namespace schnek::computation

#endif // SCHNEK_COMPUTATION_ARCHITECTURE_HPP_