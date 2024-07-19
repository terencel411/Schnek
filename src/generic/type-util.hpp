/*
 * type-util.hpp
 *
 * Created on: 19 Jul 2024
 * Author: hschmitz
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
 */

#ifndef SCHNEK_GENERIC_TYPE_UTIL_HPP_
#define SCHNEK_GENERIC_TYPE_UTIL_HPP_

#include <stddef.h>

namespace schnek::generic {
  template<size_t val>
  struct size_to_type {
    static constexpr size_t value = val;
  };

  template<int val>
  struct int_to_type {
    static constexpr int value = val;
  };

} // namespace schnek::generic

#endif // SCHNEK_GENERIC_TYPE_UTIL_HPP_