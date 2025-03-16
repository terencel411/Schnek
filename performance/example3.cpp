/*
 * example_grids_basics.cpp
 *
 *  Created on: 2 Jul 2014
 *      Author: Holger Schmitz
 *       Email: holger@notjustphysics.com
 */

 #include "../src/grid.hpp"
 #include <cmath>
 
 using namespace schnek;
 
 int main()
 {
   Array<int, 2> low(0,0);
   Array<int, 2> high(4,4);
   Range<int, 2> range(low, high);

   typedef schnek::Grid<int, 2> MyGrid;
   typedef MyGrid::IndexType MyIndex;

   MyGrid subGrid(MyIndex(5, 5));
    
   subGrid = 10;
//    range.grow(-2);
 
//    for (Range<int, 3>::iterator iter = range.begin();
//         iter != range.end();
//         ++iter)
//    {
//      Array<int, 3> p = *iter;
//     //  std::cout << p[0] << " " << p[1] << " " << p[2] << std::endl;
//      std::cout << grid(grid.get) << std::endl;
//    }
 
//    bool isInRange = range.inside(low);

    // for (int i=subGrid.getLo(0); i<=subGrid.getHi(0); ++i)
    //   for (int j=subGrid.getLo(1); j<=subGrid.getHi(1); ++j)
    //     std::cout << subGrid(i,j) << std::endl;

    // for (const auto& i : subGrid.range(0))
    //     for (const auto& j : subGrid.range(1))
    //         std::cout << subGrid(i,j) << std::endl;
    
    std::cout << subGrid.begin() << std::endl;
    // std::cout << subGrid.cbegin() << std::endl;
    // std::cout << subGrid.cend() << std::endl;
    // std::cout << subGrid.check() << std::endl;
    // std::cout << subGrid.end() << std::endl;
    // std::cout << subGrid.get() << std::endl;
    // std::cout << subGrid.getDims() << std::endl;
    // std::cout << subGrid.getHi() << std::endl;
    // std::cout << subGrid.getLo() << std::endl;
    // std::cout << subGrid.getRange() << std::endl;
    // std::cout << subGrid.getRawData() << std::endl;
    // std::cout << subGrid.getSize() << std::endl;
 }
 