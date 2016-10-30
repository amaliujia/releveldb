//
// Created by 王 瑞 on 2016/10/29.
//

#include <iostream>
#include <assert.h>
#include <string.h>

#include "gtest/gtest.h"
#include "db/Slice.h"

class SliceTests : public ::testing::Test { };

TEST_F(SliceTests, ConstructorTest) {
  const char* data = "test_data";
  std::string dataStr("test_data");
  size_t size = strlen(data);

  releveldb::Slice slice(data, strlen(data));
  EXPECT_EQ(slice.Size(), size);
  EXPECT_EQ(slice.Data(), data);

  releveldb::Slice sliceFromStr(dataStr);
  EXPECT_EQ(slice.Size(), size);
  EXPECT_EQ(slice.String(), dataStr);
}

TEST_F(SliceTests, IthTest) {
  const char* data = "test_data";

  releveldb::Slice slice(data);
  char fourth = slice[4];
  EXPECT_EQ(fourth, data[4]);
}


TEST_F(SliceTests, CompareTest) {
  const char* data = "test_data";
  const char* data2 = "test_data";
  const char* data3 = "test_data3";
  const char* data4 = "abc_data4";

  releveldb::Slice slice(data);
  releveldb::Slice slice2(data2);
  releveldb::Slice slice3(data3);
  releveldb::Slice slice4(data4);

  EXPECT_EQ(0, slice.Compare(slice2));
  EXPECT_TRUE(slice.Compare(slice3) < 0);
  EXPECT_TRUE(slice.Compare(slice4) > 0);

  EXPECT_TRUE(slice == slice2);
  EXPECT_TRUE(slice3 != slice4);
}