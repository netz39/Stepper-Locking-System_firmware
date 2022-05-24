#include "gtest/gtest.h"
#include "core/SafeAssert.h"
#include <exception>

TEST(SafeAssert, throwing) {
    ASSERT_THROW(SafeAssert(false), std::runtime_error);
    ASSERT_NO_THROW(SafeAssert(true));

    ASSERT_THROW(SafeAssert_Aux(false, 1, "hehe"), std::runtime_error);
    ASSERT_NO_THROW(SafeAssert_Aux(true, 1, "hehe"));
}