/*
 * example_grids_basics.cpp
 *
 *  Created on: 2 Jul 2014
 *      Author: Holger Schmitz
 *       Email: holger@notjustphysics.com
 */

 #include "../src/grid.hpp"

 int main()
 {
   // Example 1
   std::cout <<"Example 1"<<std::endl;
   {
     schnek::Grid<double, 2> grid(schnek::Grid<double, 2>::IndexType(5, 5));
 
     grid = 1;
 
     for (int j = 0; j < 5; ++j)
       grid(j, j) = sqrt(2 * j);
 
     for (int i = 0; i < 5; ++i)
     {
       for (int j = 0; j < 5; ++j)
         std::cout << grid(i, j) << " ";
       std::cout << std::endl;
     }
   }

//     //  Example 1a
//     std::cout <<std::endl<<"Example 1a"<<std::endl;
//    {
//     schnek::Grid<double, 2> grid(schnek::Grid<double, 2>::IndexType(5, 5));

//     grid = 1;

//     for (int j = 0; j < 5; ++j)
//       grid(j, j) = sqrt(2 * j);

//     for (auto& row : grid)
//     {
//       for (auto& elem : row)
//         std::cout << elem << " ";
//       std::cout << std::endl;
//     }

    //  Example 1b
    std::cout <<std::endl<<"Example 1b"<<std::endl;
   {
    schnek::Grid<double, 2> grid(schnek::Grid<double, 2>::IndexType(5, 5));

    grid = 1;

    for (int j = 0; j < 5; ++j)
      grid(j, j) = sqrt(2 * j);

    for (auto& elem : grid.){
        std::cout << elem << " ";
        // std::cout << std::endl;
    }
   }

//     //  Example 1c
//     std::cout <<std::endl<<"Example 1c"<<std::endl;
//    {
//     schnek::Grid<double, 2> grid(schnek::Grid<double, 2>::IndexType(5, 5));

//     grid = 1;

//     for (int j = 0; j < 5; ++j)
//       grid(j, j) = sqrt(2 * j);

//     for (int j = 0; j < 5; ++j){
//         for (auto& elem : grid[j]){
//             std::cout << elem << " ";
//             // std::cout << std::endl;
//         }
//     }
//    }
 
   // Example 2
   std::cout <<std::endl<<"Example 2"<<std::endl;
   {
     typedef schnek::Grid<double, 2> MyGrid;
     typedef MyGrid::IndexType MyIndex;
 
     MyGrid grid(MyIndex(5, 5));
 
     grid = 1;
 
     for (int j = 0; j < 5; ++j)
       grid(j, j) = sqrt(2 * j);
 
     for (int i = 0; i < 5; ++i)
     {
       for (int j = 0; j < 5; ++j)
         std::cout << grid(i, j) << " ";
       std::cout << std::endl;
     }
   }
 }
 