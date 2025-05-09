/*
 * grid_test_fixture.hpp
 *
 *  Created on: 1 Dec 2022
 *      Author: Holger Schmitz
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

#ifndef SCHNEK_TEST_GRID_GRIDTESTFIXTURE
#define SCHNEK_TEST_GRID_GRIDTESTFIXTURE

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif

#include <grid/gridstorage/kokkos-storage.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include <boost/timer/progress_display.hpp>
#include <boost/test/unit_test.hpp>

#include <map>

struct GridTest
{
    boost::random::mt19937 rGen;
    boost::random::uniform_real_distribution<> dist;

    GridTest() : dist(-1.0,1.0) {}

    template<class GridType>
    void test_reduce_1d(GridType &grid)
    {

      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
      {
        double val = dist(rGen);
        grid(i) = val;
        sumDirect += val;
      }

      double sum_grid = grid.reduce(std::plus<double>(), 0.0);

      std::cout<<"Case (reduce_1d) : "<<"sumDirect = " <<sumDirect<<std::endl;
      std::cout<<"Case (reduce_1d) : "<<"sumgrid   = " <<sum_grid<<std::endl;

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_reduce_2d(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
        {
          double val = dist(rGen);
          grid(i,j) = val;
          sumDirect += val;
        }

      double sum_grid = grid.reduce(std::plus<double>(), 0.0);

      std::cout<<"Case (reduce_2d) : "<<"sumDirect = " <<sumDirect<<std::endl;
      std::cout<<"Case (reduce_2d) : "<<"sumgrid   = " <<sum_grid<<std::endl;

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_reduce_gridparam_1d(GridType &grid1, GridType &grid2)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid1.getLo();
      typename GridType::IndexType hi = grid1.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
      {
        double val = dist(rGen);
        grid1(i) = val;
        grid2(i) = val;
        sumDirect += (grid1(i) + grid2(i));
      }

      double sum_grid = grid1.reduceGridParams(std::plus<double>(), grid2, 0.0);

      std::cout<<"Case (reduce_gridparam_1d) : "<<"sumDirect = " <<sumDirect<<std::endl;
      std::cout<<"Case (reduce_gridparam_1d) : "<<"sumgrid   = " <<sum_grid<<std::endl;

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_mergegrids_1d(GridType &grid1, GridType &grid2)
    {

      typename GridType::IndexType lo = grid1.getLo();
      typename GridType::IndexType hi = grid1.getHi();

      GridType grid3(lo, hi);
      GridType grid4(lo, hi);

      bool eqCheck = true;

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
      {
        double val = dist(rGen);
        grid1(i) = val;
        grid2(i) = val;
        grid3(i) = grid1(i) * grid2(i);
      }

      grid1.mergeGrids(std::multiplies<double>(), grid4, grid2);

      for (int i=lo[0]; i<=hi[0]; ++i){
        if (grid3(i) != grid4(i)){
          eqCheck = false;
          break;
        }
      }

      std::cout<<"Case (mergegrids_1d) : "<<"grid3 = " <<grid3(lo[0])<<std::endl;
      std::cout<<"Case (mergegrids_1d) : "<<"grid4 = " <<grid4(lo[0])<<std::endl;

      BOOST_CHECK(eqCheck);
    }

    template<class GridType>
    void test_access_1d(GridType &grid)
    { 
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
      {
        double val = dist(rGen);
        grid(i) = val;
        sumDirect += val;
      }

      double sum_grid = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
      {
        sum_grid += grid(i);
      }

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_access_2d(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
        {
          double val = dist(rGen);
          grid(i,j) = val;
          sumDirect += val;
        }

      double sum_grid = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
        {
          sum_grid += grid(i,j);
        }

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_access_3d(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
          {
            double val = dist(rGen);
            grid(i,j,k) = val;
            sumDirect += val;
          }

      double sum_grid = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
          {
            sum_grid += grid(i,j,k);
          }

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_access_4d(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
            {
              double val = dist(rGen);
              grid(i,j,k,l) = val;
              sumDirect += val;
            }

      double sum_grid = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
          for (int l=lo[3]; l<=hi[3]; ++l)
            {
              sum_grid += grid(i,j,k,l);
            }

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_access_5d(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
              {
                double val = dist(rGen);
                grid(i,j,k,l,m) = val;
                sumDirect += val;
              }

      double sum_grid = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
              {
                sum_grid += grid(i,j,k,l,m);
              }

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_access_6d(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
                for (int n=lo[5]; n<=hi[5]; ++n)
                {
                  double val = dist(rGen);
                  grid(i,j,k,l,m,n) = val;
                  sumDirect += val;
                }

      double sum_grid = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
                for (int n=lo[5]; n<=hi[5]; ++n)
                {
                  sum_grid += grid(i,j,k,l,m,n);
                }

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_access_7d(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
                for (int n=lo[5]; n<=hi[5]; ++n)
                  for (int o=lo[6]; o<=hi[6]; ++o)
                  {
                    double val = dist(rGen);
                    grid(i,j,k,l,m,n,o) = val;
                    sumDirect += val;
                  }

      double sum_grid = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
                for (int n=lo[5]; n<=hi[5]; ++n)
                  for (int o=lo[6]; o<=hi[6]; ++o)
                  {
                    sum_grid += grid(i,j,k,l,m,n,o);
                  }

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_access_8d(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
                for (int n=lo[5]; n<=hi[5]; ++n)
                  for (int o=lo[6]; o<=hi[6]; ++o)
                    for (int p=lo[7]; p<=hi[7]; ++p)
                    {
                      double val = dist(rGen);
                      grid(i,j,k,l,m,n,o,p) = val;
                      sumDirect += val;
                    }

      double sum_grid = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
                for (int n=lo[5]; n<=hi[5]; ++n)
                  for (int o=lo[6]; o<=hi[6]; ++o)
                    for (int p=lo[7]; p<=hi[7]; ++p)
                    {
                      sum_grid += grid(i,j,k,l,m,n,o,p);
                    }

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_access_9d(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
                for (int n=lo[5]; n<=hi[5]; ++n)
                  for (int o=lo[6]; o<=hi[6]; ++o)
                    for (int p=lo[7]; p<=hi[7]; ++p)
                      for (int q=lo[8]; q<=hi[8]; ++q)
                      {
                        double val = dist(rGen);
                        grid(i,j,k,l,m,n,o,p,q) = val;
                        sumDirect += val;
                      }

      double sum_grid = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
                for (int n=lo[5]; n<=hi[5]; ++n)
                  for (int o=lo[6]; o<=hi[6]; ++o)
                    for (int p=lo[7]; p<=hi[7]; ++p)
                      for (int q=lo[8]; q<=hi[8]; ++q)
                      {
                        sum_grid += grid(i,j,k,l,m,n,o,p,q);
                      }

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_access_10d(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
                for (int n=lo[5]; n<=hi[5]; ++n)
                  for (int o=lo[6]; o<=hi[6]; ++o)
                    for (int p=lo[7]; p<=hi[7]; ++p)
                      for (int q=lo[8]; q<=hi[8]; ++q)
                        for (int r=lo[9]; r<=hi[9]; ++r)
                        {
                          double val = dist(rGen);
                          grid(i,j,k,l,m,n,o,p,q,r) = val;
                          sumDirect += val;
                        }

      double sum_grid = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
            for (int l=lo[3]; l<=hi[3]; ++l)
              for (int m=lo[4]; m<=hi[4]; ++m)
                for (int n=lo[5]; n<=hi[5]; ++n)
                  for (int o=lo[6]; o<=hi[6]; ++o)
                    for (int p=lo[7]; p<=hi[7]; ++p)
                      for (int q=lo[8]; q<=hi[8]; ++q)
                        for (int r=lo[9]; r<=hi[9]; ++r)
                        {
                          sum_grid += grid(i,j,k,l,m,n,o,p,q,r);
                        }

      BOOST_CHECK(is_equal(sumDirect, sum_grid));
    }

    template<class GridType>
    void test_stride(GridType &grid, const typename GridType::IndexType &index)
    {
      for (size_t i=0; i<GridType::Rank; ++i)
      {
        typename GridType::IndexType indexP = index;
        indexP[i] = index[i] + 1;
        ptrdiff_t diff = &(grid.get(indexP)) - &(grid.get(index));
        ptrdiff_t stride = grid.stride(i);
        BOOST_CHECK(is_equal(diff, stride));
      }
    }

    template<class GridType>
    void test_range_access(GridType &grid)
    {
      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();
      typename GridType::RangeType range = grid.getRange();

      for (size_t i=0; i<GridType::Rank; ++i)
      {
        BOOST_CHECK(is_equal(lo[i], range.getLo(i)));
        BOOST_CHECK(is_equal(hi[i], range.getHi(i)));
      }
    }

    template<size_t Rank, typename GridType>
    void generic_range_access_Nd()
    {
      typename GridType::IndexType lo, hi;
      boost::timer::progress_display show_progress(30);
      for (int n=0; n<5; ++n)
      {
        random_extent<Rank>(lo, hi);
        typename GridType::RangeType range{lo, hi};
        GridType g1(lo, hi);
        GridType g2(range);
        test_range_access(g1);
        test_range_access(g2);
        ++show_progress;
        for (int m=0; m<5; ++m)
        {
          random_extent<Rank>(lo, hi);
          typename GridType::RangeType range{lo, hi};
          g1.resize(lo,hi);
          g2.resize(range);
          test_range_access(g1);
          test_range_access(g2);
          ++show_progress;
        }
      }
    }

    template<class GridType>
    void test_copy_constructor(GridType &grid)
    {
      GridType copiedBefore{grid};

      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
          {
            double val = dist(rGen);
            grid(i,j,k) = val;
            sumDirect += val;
          }

      GridType copiedAfter{grid};
      double sumGridBefore = 0.0;
      double sumGridAfter = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
          {
            sumGridBefore += copiedBefore(i,j,k);
            sumGridAfter += copiedAfter(i,j,k);
          }

      BOOST_CHECK(is_equal(sumDirect, sumGridBefore));
      BOOST_CHECK(is_equal(sumDirect, sumGridAfter));
    }

    template<class GridType>
    void test_assignment_operator(GridType &grid)
    {
      double sumDirect = 0.0;

      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      GridType copiedBefore{lo, hi};
      GridType copiedAfter{lo, hi};

      copiedBefore = grid;
      // write random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
          {
            double val = dist(rGen);
            grid(i,j,k) = val;
            sumDirect += val;
          }

      copiedAfter = grid;
      double sumGridBefore = 0.0;
      double sumGridAfter = 0.0;

      // read back random numbers
      for (int i=lo[0]; i<=hi[0]; ++i)
        for (int j=lo[1]; j<=hi[1]; ++j)
          for (int k=lo[2]; k<=hi[2]; ++k)
          {
            sumGridBefore += copiedBefore(i,j,k);
            sumGridAfter += copiedAfter(i,j,k);
          }

      BOOST_CHECK(is_equal(sumDirect, sumGridBefore));
      BOOST_CHECK(is_equal(sumDirect, sumGridAfter));
    }

    template<class GridType>
    void test_copy_resize(GridType &grid)
    {
      GridType copied{grid};
      typename GridType::IndexType lo = grid.getLo();
      typename GridType::IndexType hi = grid.getHi();

      random_extent(lo, hi);
      grid.resize(lo, hi);

      typename GridType::IndexType loCopied = copied.getLo();
      typename GridType::IndexType hiCopied = copied.getHi();

      for (size_t i=0; i<GridType::Rank; ++i)
      {
        BOOST_CHECK_EQUAL(loCopied[i], lo[i]);
        BOOST_CHECK_EQUAL(hiCopied[i], hi[i]);
      }

      random_extent(loCopied, hiCopied);
      copied.resize(loCopied, hiCopied);

      lo = grid.getLo();
      hi = grid.getHi();

      for (size_t i=0; i<GridType::Rank; ++i)
      {
        BOOST_CHECK_EQUAL(loCopied[i], lo[i]);
        BOOST_CHECK_EQUAL(hiCopied[i], hi[i]);
      }
    }

    struct DeleteCounter
    {
      int value;
      std::map<int, int> *counters;

      DeleteCounter(): value(0), counters(NULL) {}

      DeleteCounter(int value, std::map<int, int> &counters): value(value), counters(&counters)
      {}

      DeleteCounter(const DeleteCounter&) = default;

      ~DeleteCounter() 
      {
        if (counters == NULL)
        {
          return;
        }

        if (counters->count(value) > 0)
        {
          ++(*counters)[value];
        } else {
          (*counters)[value] = 1;
        }
      }

      DeleteCounter& operator=(const DeleteCounter& other) = default;
    };

    template<size_t rank>
    void random_extent(schnek::Array<int,rank> &lo, schnek::Array<int,rank> &hi)
    {
      const int maxExtent = (int)pow(20000,(1.0/(double)rank));
      boost::random::uniform_int_distribution<> orig(-maxExtent/2, maxExtent/2);
      boost::random::uniform_int_distribution<> extent(1, maxExtent);
      for (size_t i=0; i<rank; ++i)
      {
        int o = orig(rGen);
        int l = extent(rGen);
        lo[i] = o;
        hi[i] = o+l;
      }
    }

    template<size_t rank>
    schnek::Array<int,rank> random_index(const schnek::Array<int,rank> &lo, const schnek::Array<int,rank> &hi)
    {
      schnek::Array<int,rank> index;
      for (size_t i=0; i<rank; ++i)
      {
        boost::random::uniform_int_distribution<> indexDist(lo[i], hi[i]);
        index[i] = indexDist(rGen);
      }
      return index;
    }
};

#endif