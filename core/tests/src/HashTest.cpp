#include "core/hash.hpp"
#include "gtest/gtest.h"
#include <exception>
using namespace core::hash;

TEST(HashTest, noEmbeddedStuff)
{
    ASSERT_EQ(computeFirmwareHash(), FirmwareHashTestingValue);
}

TEST(HashTest, fnvWithSeed)
{
    static constexpr uint64_t TestingHashSeed{0xcbf29ce484222325};

    // this test relies on a specific prime due to hardcoded hashing results
    static_assert(MagicPrime == 0x00000100000001b3);

    // basic hashing
    static constexpr std::string_view testData = "heyWhatsUpDude?";
    const auto hashedTestData1 =
        fnvWithSeed(TestingHashSeed, reinterpret_cast<const uint8_t *>(&(*testData.begin())),
                    reinterpret_cast<const uint8_t *>(&(*testData.end())));

    static constexpr std::string_view testData2 = "heyWhatsUpDÜde?";
    const auto hashedTestData2 =
        fnvWithSeed(TestingHashSeed, reinterpret_cast<const uint8_t *>(&(*testData2.begin())),
                    reinterpret_cast<const uint8_t *>(&(*testData2.end())));

    static_assert(testData != testData2);
    EXPECT_NE(hashedTestData1, hashedTestData2);
    EXPECT_EQ(hashedTestData1, 16517619232421245306U);
    EXPECT_EQ(hashedTestData2, 1952215504172837380U);


    // different hashes with different start values
    const auto hashedTestData3 =
        fnvWithSeed(TestingHashSeed + 1, reinterpret_cast<const uint8_t *>(&(*testData2.begin())),
                    reinterpret_cast<const uint8_t *>(&(*testData2.end())));
    EXPECT_NE(hashedTestData2, hashedTestData3);


    // illegal pointers
    EXPECT_THROW(fnvWithSeed(TestingHashSeed,
                             reinterpret_cast<const uint8_t *>(&(*testData2.end())),
                             reinterpret_cast<const uint8_t *>(&(*testData2.begin()))),
                 std::runtime_error);

    EXPECT_THROW(fnvWithSeed(TestingHashSeed, nullptr,
                             reinterpret_cast<const uint8_t *>(&(*testData2.begin()))),
                 std::runtime_error);
    EXPECT_THROW(fnvWithSeed(TestingHashSeed,
                             reinterpret_cast<const uint8_t *>(&(*testData2.end())), nullptr),
                 std::runtime_error);
    EXPECT_THROW(fnvWithSeed(TestingHashSeed, nullptr, nullptr), std::runtime_error);
}