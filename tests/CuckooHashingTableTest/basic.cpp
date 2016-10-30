//
// Created by rui_wang on 9/8/16.
//

#include <iostream>

#include "gtest/gtest.h"
#include "CuckoohashingTable.h"

class CuckooHasingTableBasicTest : public ::testing::Test { };

TEST_F(CuckooHasingTableBasicTest, BasicTest) {
  concurrent_lib::CuckoohashingTable<int, int, std::hash<int>, std::equal_to<int>> table;
  //table.Insert(std::move(3), std::move(3));
  // table.Insert(3, 3);
  //assert(table.Contains(3));
  std::cout << table.Size() << std::endl;
}