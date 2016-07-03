#include <gtest/gtest.h>

#include "lib/addroffset.hh"
#include "processors/cpu_traits.hh"

class TestCPU;

template <>
struct cpu_traits<TestCPU> {
    typedef uint8_t AddrType;
};

TEST(AddrOffset, TestSimplePositiveOffset) {
    AddrOffset<TestCPU> off(0, 20);

    ASSERT_TRUE(off.offset() == 20);
    ASSERT_FALSE(off.negative());
}

TEST(AddrOffset, TestSimpleNegativeOffset) {
    AddrOffset<TestCPU> off(12, 3);

    ASSERT_TRUE(off.offset() == 9);
    ASSERT_TRUE(off.negative());
}
