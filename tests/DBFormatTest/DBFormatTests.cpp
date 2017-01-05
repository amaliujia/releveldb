//
// Created by amaliujia on 2016/11/24.
//

#include "db/random.h"
#include "db/DBFormat.h"
#include "gtest/gtest.h"

namespace releveldb {

class DBFormatTests : public ::testing::Test { };

static std::string IKey(const std::string& user_key,
                        uint64_t seq,
                        ValueType vt) {
  std::string encoded;
  AppendInternalKey(&encoded, ParsedInternalKey(user_key, seq, vt));
  return encoded;
}

static std::string Shorten(const std::string& s, const std::string& l) {
  std::string result = s;
  InternalKeyComparator(BytewiseComparator()).FindShortestSeparator(&result, l);
  return result;
}

static std::string ShortSuccessor(const std::string& s) {
  std::string result = s;
  InternalKeyComparator(BytewiseComparator()).FindShortSuccessor(&result);
  return result;
}

static void TestKey(const std::string& key,
                    uint64_t seq,
                    ValueType vt) {
  std::string encoded = IKey(key, seq, vt);

  Slice in(encoded);
  ParsedInternalKey decoded("", 0, kTypeValue);

  ASSERT_TRUE(ParseInternalKey(in, &decoded));
  ASSERT_EQ(key, decoded.user_key.String());
  ASSERT_EQ(seq, decoded.sequence);
  ASSERT_EQ(vt, decoded.type);

  ASSERT_TRUE(!ParseInternalKey(Slice("bar"), &decoded));
}

TEST(DBFormatTests, InternalKey_EncodeDecode) {
  const char* keys[] = { "", "k", "hello", "longggggggggggggggggggggg" };
  const uint64_t seq[] = {
  1, 2, 3,
  (1ull << 8) - 1, 1ull << 8, (1ull << 8) + 1,
  (1ull << 16) - 1, 1ull << 16, (1ull << 16) + 1,
  (1ull << 32) - 1, 1ull << 32, (1ull << 32) + 1
  };
  for (int k = 0; k < sizeof(keys) / sizeof(keys[0]); k++) {
    for (int s = 0; s < sizeof(seq) / sizeof(seq[0]); s++) {
      TestKey(keys[k], seq[s], kTypeValue);
      TestKey("hello", 1, kTypeDeletion);
    }
  }
}

TEST(DBFormatTests, InternalKeyShortSeparator) {
  // When user keys are same
  ASSERT_EQ(IKey("foo", 100, kTypeValue),
            Shorten(IKey("foo", 100, kTypeValue),
                    IKey("foo", 99, kTypeValue)));
  ASSERT_EQ(IKey("foo", 100, kTypeValue),
            Shorten(IKey("foo", 100, kTypeValue),
                    IKey("foo", 101, kTypeValue)));
  ASSERT_EQ(IKey("foo", 100, kTypeValue),
            Shorten(IKey("foo", 100, kTypeValue),
                    IKey("foo", 100, kTypeValue)));
  ASSERT_EQ(IKey("foo", 100, kTypeValue),
            Shorten(IKey("foo", 100, kTypeValue),
                    IKey("foo", 100, kTypeDeletion)));

  // When user keys are misordered
  ASSERT_EQ(IKey("foo", 100, kTypeValue),
            Shorten(IKey("foo", 100, kTypeValue),
                    IKey("bar", 99, kTypeValue)));

  // When user keys are different, but correctly ordered
  ASSERT_EQ(IKey("g", kMaxSequenceNumber, kValueTypeForSeek),
            Shorten(IKey("foo", 100, kTypeValue),
                    IKey("hello", 200, kTypeValue)));

  // When start user key is prefix of limit user key
  ASSERT_EQ(IKey("foo", 100, kTypeValue),
            Shorten(IKey("foo", 100, kTypeValue),
                    IKey("foobar", 200, kTypeValue)));

  // When limit user key is prefix of start user key
  ASSERT_EQ(IKey("foobar", 100, kTypeValue),
            Shorten(IKey("foobar", 100, kTypeValue),
                    IKey("foo", 200, kTypeValue)));
}

TEST(DBFormatTests, InternalKeyShortestSuccessor) {
  ASSERT_EQ(IKey("g", kMaxSequenceNumber, kValueTypeForSeek),
            ShortSuccessor(IKey("foo", 100, kTypeValue)));
  ASSERT_EQ(IKey("\xff\xff", 100, kTypeValue),
            ShortSuccessor(IKey("\xff\xff", 100, kTypeValue)));
}

}  // namespace releveldb