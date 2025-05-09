/*
 * single-array-storage-base.hpp
 *
 * Created on: 07 Nov 2022
 * Author: Holger Schmitz
 * Email: holger@notjustphysics.com
 *
 * Copyright 2012-2022 Holger Schmitz
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

#ifndef SCHNEK_GRID_GRIDSTORAGE_SINGLESTORAGEBASE_HPP_
#define SCHNEK_GRID_GRIDSTORAGE_SINGLESTORAGEBASE_HPP_

#include "../array.hpp"

namespace schnek {
  /**
   * @brief The storage base extends from an allocation policy and adds some accessor methods
   *
   * @tparam T The type of data stored in the grid
   * @tparam rank The rank of the grid
   * @tparam AllocationPolicy The allocation policy
   */
  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  class SingleArrayGridStorageBase : public AllocationPolicy<T, rank> {
    public:
      /// The grid index type
      typedef Array<int, rank> IndexType;

      /// The grid range type
      typedef Range<int, rank> RangeType;

    public:
      /// Default constructor
      SingleArrayGridStorageBase();

      /// Copy constructor
      SingleArrayGridStorageBase(const SingleArrayGridStorageBase &) = default;

      /**
       * @brief Assignment operator
       */
      SingleArrayGridStorageBase<T, rank, AllocationPolicy>
          &operator=(const SingleArrayGridStorageBase<T, rank, AllocationPolicy> &) = default;

      /// Access to the underlying raw data
      T *getRawData() const { return this->data->ptr; }

      /// Get the lowest coordinate in the grid (inclusive)
      SCHNEK_INLINE const IndexType &getLo() const { return this->range.getLo(); }

      /// Get the highest coordinate in the grid (inclusive)
      SCHNEK_INLINE const IndexType &getHi() const { return this->range.getHi(); }

      /// Get the lowest coordinate in the grid (inclusive)
      SCHNEK_INLINE const RangeType &getRange() const { return this->range; }

      /// Get the dimensions of the grid `dims = high - low + 1`
      SCHNEK_INLINE const IndexType &getDims() const { return this->dims; }

      /// Get k-th component of the lowest coordinate in the grid (inclusive)
      SCHNEK_INLINE int getLo(int k) const { return this->range.getLo(k); }

      /// Get k-th component of the highest coordinate in the grid (inclusive)
      SCHNEK_INLINE int getHi(int k) const { return this->range.getHi(k); }

      /// Get k-th component of the dimensions of the grid `dims = high - low + 1`
      SCHNEK_INLINE int getDims(int k) const { return this->dims[k]; }

      /// Get the length of the allocated array
      SCHNEK_INLINE int getSize() const { return this->size; }

      typedef T *storage_iterator;
      typedef const T *const_storage_iterator;

      SCHNEK_INLINE storage_iterator begin() { return this->data->ptr; }
      SCHNEK_INLINE storage_iterator end() { return this->data->ptr + this->size; }

      SCHNEK_INLINE const_storage_iterator cbegin() const { return this->data->ptr; }
      SCHNEK_INLINE const_storage_iterator cend() const { return this->data->ptr + this->size; }
  };

  /**
   * @brief Extends from SingleArrayGridStorageBase to provide C-order indexing over the
   * 1-dimensional data array for a multidimensional grid.
   *
   * @tparam T The type of data stored in the grid
   * @tparam rank The rank of the grid
   * @tparam AllocationPolicy The allocation policy
   */
  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  class SingleArrayGridCOrderStorageBase : public SingleArrayGridStorageBase<T, rank, AllocationPolicy> {
    private:
      /// A pointer offset to the origin for faster access
      T *data_fast;

    public:
      /// Base class type
      typedef SingleArrayGridStorageBase<T, rank, AllocationPolicy> BaseType;

      /// The grid index type
      typedef typename BaseType::IndexType IndexType;

      /// The grid index type
      typedef typename BaseType::RangeType RangeType;

      /// Default constructor
      SingleArrayGridCOrderStorageBase();

      /// Copy constructor
      SingleArrayGridCOrderStorageBase(const SingleArrayGridCOrderStorageBase &);

      /**
       * @brief Construct with a given size
       *
       * @param lo the lowest coordinate in the grid (inclusive)
       * @param hi the highest coordinate in the grid (inclusive)
       */
      SingleArrayGridCOrderStorageBase(const IndexType &lo, const IndexType &hi);

      /**
       * @brief Construct with a given size
       *
       * @param range the lowest and highest coordinates in the grid (inclusive)
       */
      SingleArrayGridCOrderStorageBase(const RangeType &range);

      /**
       * @brief Assignment operator
       */
      SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>
          &operator=(const SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy> &) = default;

      /**
       * @brief Get the lvalue at a given grid index
       *
       * @param index The grid index
       * @return the lvalue at the grid index
       */
      SCHNEK_INLINE T &get(const IndexType &index);

      /**
       * @brief Get the rvalue at a given grid index
       *
       * @param index The grid index
       * @return the rvalue at the grid index
       */
      SCHNEK_INLINE const T &get(const IndexType &index) const;

      /**
       * @brief resizes to grid with lower indices lo[0],...,lo[rank-1]
       * and upper indices hi[0],...,hi[rank-1]
       */
      void resize(const IndexType &low, const IndexType &high);

      /**
       * @brief resizes to grid with the range.
       * The endponts of the range are inclusive
       */
      void resize(const RangeType range);

      /**
       * @brief returns the stride of the specified dimension
       */
      ptrdiff_t stride(size_t dim) const;

    private:
      /**
       * @brief Update the data_fast pointer offset to the origin for faster access
       *
       * This method is called indirectly when a resize is performed on any of the copies of the
       * grid, including the original grid and any copies made by the copy constructor or the
       * assignment operator.
       *
       * This ensures that the data_fast pointer is always up to date between all copies of the
       * grid.
       */
      void updateDataFast();
  };

  /**
   * @brief Extends from SingleArrayGridStorageBase to provide Fortran-order indexing over the
   * 1-dimensional data array for a multidimensional grid.
   *
   * @tparam T The type of data stored in the grid
   * @tparam rank The rank of the grid
   * @tparam AllocationPolicy The allocation policy
   */
  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  class SingleArrayGridFortranOrderStorageBase : public SingleArrayGridStorageBase<T, rank, AllocationPolicy> {
    private:
      /// A pointer offset to the origin for faster access
      T *data_fast;

    public:
      /// Base class type
      typedef SingleArrayGridStorageBase<T, rank, AllocationPolicy> BaseType;

      /// The grid index type
      typedef typename BaseType::IndexType IndexType;

      /// The grid index type
      typedef typename BaseType::RangeType RangeType;

      /// Default constructor
      SingleArrayGridFortranOrderStorageBase();

      /// Copy constructor
      SingleArrayGridFortranOrderStorageBase(const SingleArrayGridFortranOrderStorageBase &);

      /**
       * @brief Construct with a given size
       *
       * @param lo the lowest coordinate in the grid (inclusive)
       * @param lo the highest coordinate in the grid (inclusive)
       */
      SingleArrayGridFortranOrderStorageBase(const IndexType &lo, const IndexType &hi);

      /**
       * @brief Construct with a given size
       *
       * @param range the lowest and highest coordinates in the grid (inclusive)
       */
      SingleArrayGridFortranOrderStorageBase(const RangeType &range);

      /**
       * @brief Assignment operator
       */
      SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>
          &operator=(const SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy> &) = default;

      /**
       * @brief Get the lvalue at a given grid index
       *
       * @param index The grid index
       * @return the lvalue at the grid index
       */
      SCHNEK_INLINE T &get(const IndexType &index);

      /**
       * @brief Get the rvalue at a given grid index
       *
       * @param index The grid index
       * @return the rvalue at the grid index
       */
      SCHNEK_INLINE const T &get(const IndexType &index) const;

      /**
       * @brief resizes to grid with lower indices low[0],...,low[rank-1]
       * and upper indices high[0],...,high[rank-1]
       */
      void resize(const IndexType &low, const IndexType &high);

      /**
       * @brief resizes to grid with the range.
       * The endponts of the range are inclusive
       */
      void resize(const RangeType range);

      /**
       * @brief returns the stride of the specified dimension
       */
      ptrdiff_t stride(size_t dim) const;

    private:
      void updateDataFast();
  };

  //=================================================================
  //================== SingleArrayGridStorageBase ===================
  //=================================================================

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SingleArrayGridStorageBase<T, rank, AllocationPolicy>::SingleArrayGridStorageBase() : AllocationPolicy<T, rank>() {}

  //=================================================================
  //=============== SingleArrayGridCOrderStorageBase ================
  //=================================================================

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>::SingleArrayGridCOrderStorageBase()
      : BaseType(), data_fast(NULL) {
    this->onUpdate([this]() { updateDataFast(); });
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>::SingleArrayGridCOrderStorageBase(
      const SingleArrayGridCOrderStorageBase &other
  )
      : BaseType(other), data_fast(other.data_fast) {
    this->onUpdate([this]() { updateDataFast(); });
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>::SingleArrayGridCOrderStorageBase(
      const IndexType &lo, const IndexType &hi
  )
      : BaseType(), data_fast(NULL) {
    this->onUpdate([this]() { updateDataFast(); });
    resize(lo, hi);
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>::SingleArrayGridCOrderStorageBase(const RangeType &range)
      : BaseType(), data_fast(NULL) {
    this->onUpdate([this]() { updateDataFast(); });
    resize(range.getLo(), range.getHi());
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SCHNEK_INLINE T &SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>::get(const IndexType &index) {
    size_t pos = index[0];
    for (size_t i = 1; i < rank; ++i) {
      pos = index[i] + this->dims[i] * pos;
    }
    return this->data_fast[pos];
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SCHNEK_INLINE const T &SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>::get(const IndexType &index
  ) const {
    size_t pos = index[0];
    for (size_t i = 1; i < rank; ++i) {
      pos = index[i] + this->dims[i] * pos;
    }
    return this->data_fast[pos];
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  inline void SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>::resize(
      const IndexType &lo, const IndexType &hi
  ) {
    this->resizeImpl(lo, hi);
    // size_t p = -this->range.getLo(0);

    // for (size_t d = 1; d < rank; ++d)
    // {
    //     p = p * this->dims[d] - this->getLo(d);
    // }
    // data_fast = this->data->ptr + p;
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  inline void SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>::resize(const RangeType range) {
    this->resize(range.getLo(), range.getHi());
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  inline ptrdiff_t SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>::stride(size_t dim) const {
    size_t stride = 1;
    for (size_t i = rank - 1; i > dim; --i) {
      stride *= this->dims[i];
    }
    return stride;
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  void SingleArrayGridCOrderStorageBase<T, rank, AllocationPolicy>::updateDataFast() {
    ptrdiff_t p = -this->range.getLo(0);

    for (size_t d = 1; d < rank; ++d) {
      p = p * this->dims[d] - this->getLo(d);
    }
    data_fast = this->data->ptr + p;
  }

  //=================================================================
  //============ SingleArrayGridFortranOrderStorageBase =============
  //=================================================================

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>::SingleArrayGridFortranOrderStorageBase()
      : BaseType(), data_fast(NULL) {
    this->onUpdate([this]() { updateDataFast(); });
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>::SingleArrayGridFortranOrderStorageBase(
      const SingleArrayGridFortranOrderStorageBase &other
  )
      : BaseType(other), data_fast(other.data_fast) {
    this->onUpdate([this]() { updateDataFast(); });
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>::SingleArrayGridFortranOrderStorageBase(
      const IndexType &lo, const IndexType &hi
  )
      : BaseType(), data_fast(NULL) {
    this->onUpdate([this]() { updateDataFast(); });
    resize(lo, hi);
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>::SingleArrayGridFortranOrderStorageBase(
      const RangeType &range
  )
      : BaseType(), data_fast(NULL) {
    this->onUpdate([this]() { updateDataFast(); });
    resize(range.getLo(), range.getHi());
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SCHNEK_INLINE T &SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>::get(const IndexType &index) {
    size_t pos = index[rank - 1];
    for (ptrdiff_t i = ptrdiff_t(rank) - 2; i >= 0; --i) {
      pos = index[i] + this->dims[i] * pos;
    }
    return this->data_fast[pos];
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  SCHNEK_INLINE const T &SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>::get(const IndexType &index
  ) const {
    size_t pos = index[rank - 1];
    for (ptrdiff_t i = ptrdiff_t(rank) - 2; i >= 0; --i) {
      pos = index[i] + this->dims[i] * pos;
    }
    return this->data_fast[pos];
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  inline void SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>::resize(
      const IndexType &lo, const IndexType &hi
  ) {
    this->resizeImpl(lo, hi);
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  inline void SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>::resize(const RangeType range) {
    this->resize(range.getLo(), range.getHi());
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  inline ptrdiff_t SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>::stride(size_t dim) const {
    ptrdiff_t stride = 1;
    for (size_t i = 0; i < dim; ++i) {
      stride *= this->dims[i];
    }
    return stride;
  }

  template<typename T, size_t rank, template<typename, size_t> class AllocationPolicy>
  void SingleArrayGridFortranOrderStorageBase<T, rank, AllocationPolicy>::updateDataFast() {
    size_t p = -this->getLo(rank - 1);

    for (ptrdiff_t d = ptrdiff_t(rank) - 2; d >= 0; --d) {
      p = p * this->dims[d] - this->getLo(d);
    }
    data_fast = this->data->ptr + p;
  }

}  // namespace schnek

#endif  // SCHNEK_GRID_GRIDSTORAGE_SINGLESTORAGEBASE_HPP_
