//
// Created by 王 瑞 on 2016/10/29.
//

#include <iostream>
#include <assert.h>
#include <string.h>

#include "gtest/gtest.h"
#include "db/Status.h"

class StatusTests : public ::testing::Test { };

TEST_F(StatusTests, BasicTests) {
  releveldb::Status status = releveldb::Status::OK();
  EXPECT_EQ("OK", status.ToString());

  releveldb::Slice slice("key");
  status = releveldb::Status::NotFound(slice);
  EXPECT_EQ("Not Found: key", status.ToString());
}