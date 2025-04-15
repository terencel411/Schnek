#ifndef GRID_UTILS_HPP
#define GRID_UTILS_HPP

#include <schnek/grid/grid.hpp>
#include <schnek/grid/gridstorage/kokkos-storage.hpp>

namespace grid_utils {

template<typename T, size_t rank>
void fill_grid(schnek::Grid<T, rank, schnek::GridAssertCheck, schnek::KokkosDefaultGridStorage>& grid, const T& value) {
    // Access the storage directly
    auto& storage = static_cast<schnek::KokkosDefaultGridStorage<T, rank>&>(grid);
    storage.fill(value);
}

} // namespace grid_utils

#endif // GRID_UTILS_HPP