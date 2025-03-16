/*
 * example_grids_basics.cpp
 *
 *  Created on: 2 Jul 2014
 *      Author: Holger Schmitz
 *       Email: holger@notjustphysics.com
 */

 #include "../src/grid/range.hpp"
 #include "../src/grid.hpp"
 #include "../src/grid/subgrid.hpp"

 #include <iostream>
 
 using namespace schnek;
 
 int main()
 { typedef schnek::Grid<int, 2> MyGrid;
   typedef MyGrid::IndexType MyIndex;
 
   MyGrid grid(MyIndex(3, 3));
     
   grid = 10;

   Range<int, 2>::LimitType low(1, 1);
   Range<int, 2>::LimitType high(3, 3);
 
   // create range using limits
   Range<int, 2> range(low, high);
 
   // read lower and upper bounds
   low  = range.getLo();
   high = range.getHi();
 
//    modify bounds
   range.getLo() = Range<int, 2>::LimitType(0,0);
   range.getHi() = Range<int, 2>::LimitType(2,2);
 
//    range.grow(-1);
   // iterate over range
   for (Range<int, 2>::iterator it=range.begin(); it!=range.end(); ++it)
   {
     const Range<int, 2>::LimitType &pos = *it;
     std::cout << grid(pos[0], pos[1]) << std::endl;
   }
   
   Range<int, 2> range1(grid.getLo(), grid.getHi());
   range1.grow(-1);
 
   SubGrid<Grid<int, 2> > subGrid(range1,grid);
   
//    subGrid = 20;
//    subGrid.operator+=10;

   for (int i=subGrid.getLo(0); i<=subGrid.getHi(0); ++i)
     for (int j=subGrid.getLo(1); j<=subGrid.getHi(1); ++j)
        std::cout << subGrid(i, j) << std::endl;
 }
 