/*
 * utility.hpp
 *
 *  Created on: 19 Jul 2017
 *      Author: Holger Schmitz
 */

#ifndef TESTSUITE_UTILITY_HPP_
#define TESTSUITE_UTILITY_HPP_

#include <grid/array.hpp>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <boost/test/unit_test.hpp>

#pragma GCC diagnostic pop


// safe comparison of two floating point numbers
bool is_equal(double a, double b);

/** Class to plug into the Array as CheckingPolicy.
 *  Performs Boost Test argument checking for unit tests.
 */
template<size_t limit>
class ArrayBoostTestArgCheck
{
  public:
    /** The check method does not do anything */
    void check(size_t i) const
    {
      BOOST_CHECK_GE(i,size_t(0));
      BOOST_CHECK_LT(i,limit);
    }
};

template<size_t rank>
class GridBoostTestCheck {
  public:
    typedef schnek::Array<int,rank,ArrayBoostTestArgCheck> IndexType;
    static const  IndexType &check(
        const IndexType &pos,
        const IndexType &low,
        const IndexType &high
    )
    {
      for (size_t i=0; i<rank; ++i)
      {
        BOOST_CHECK_GE(pos[i],low[i]);
        BOOST_CHECK_LE(pos[i],high[i]);
      }
      return pos;
    }
};



#endif /* TESTSUITE_UTILITY_HPP_ */
