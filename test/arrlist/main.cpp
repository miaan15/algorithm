#include <gtest/gtest.h>

extern "C" {
#include <arrlist.h>
}

// Define test types
DEFINE_ARRLIST(int)
DEFINE_ARRLIST(float)
DEFINE_ARRLIST(char)

class ArrListTest : public ::testing::Test {
  protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ============================================================================
// ARRLIST_APPEND Tests
// ============================================================================

TEST_F(ArrListTest, AppendSingleElement) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 42);

    EXPECT_EQ(list.count, 1);
    EXPECT_GE(list.capacity, 1);
    EXPECT_NE(list.buffer, nullptr);
    EXPECT_EQ(list.buffer[0], 42);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, AppendMultipleElements) {
    ArrList_int list = {};

    for (int i = 0; i < 100; i++) {
        ARRLIST_APPEND(&list, i);
    }

    EXPECT_EQ(list.count, 100);
    for (int i = 0; i < 100; i++) {
        EXPECT_EQ(list.buffer[i], i);
    }

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, AppendFloat) {
    ArrList_float list = {};

    ARRLIST_APPEND(&list, 1.5f);
    ARRLIST_APPEND(&list, 2.5f);
    ARRLIST_APPEND(&list, 3.5f);

    EXPECT_EQ(list.count, 3);
    EXPECT_FLOAT_EQ(list.buffer[0], 1.5f);
    EXPECT_FLOAT_EQ(list.buffer[1], 2.5f);
    EXPECT_FLOAT_EQ(list.buffer[2], 3.5f);

    ARRLIST_FREE(&list);
}

// ============================================================================
// ARRLIST_INSERT Tests
// ============================================================================

TEST_F(ArrListTest, InsertAtBeginning) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 2);
    ARRLIST_APPEND(&list, 3);
    ARRLIST_INSERT(&list, 0, 1);

    EXPECT_EQ(list.count, 3);
    EXPECT_EQ(list.buffer[0], 1);
    EXPECT_EQ(list.buffer[1], 2);
    EXPECT_EQ(list.buffer[2], 3);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, InsertInMiddle) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 1);
    ARRLIST_APPEND(&list, 3);
    ARRLIST_INSERT(&list, 1, 2);

    EXPECT_EQ(list.count, 3);
    EXPECT_EQ(list.buffer[0], 1);
    EXPECT_EQ(list.buffer[1], 2);
    EXPECT_EQ(list.buffer[2], 3);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, InsertAtEnd) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 1);
    ARRLIST_APPEND(&list, 2);
    ARRLIST_INSERT(&list, 2, 3);

    EXPECT_EQ(list.count, 3);
    EXPECT_EQ(list.buffer[0], 1);
    EXPECT_EQ(list.buffer[1], 2);
    EXPECT_EQ(list.buffer[2], 3);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, InsertIntoEmptyList) {
    ArrList_int list = {};

    ARRLIST_INSERT(&list, 0, 42);

    EXPECT_EQ(list.count, 1);
    EXPECT_EQ(list.buffer[0], 42);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, InsertBeyondCountIsNoOp) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 1);
    ARRLIST_APPEND(&list, 2);
    size_t original_count = list.count;

    ARRLIST_INSERT(&list, 10, 999);

    EXPECT_EQ(list.count, original_count);

    ARRLIST_FREE(&list);
}

// ============================================================================
// ARRLIST_POP Tests
// ============================================================================

TEST_F(ArrListTest, PopRemovesLastElement) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 1);
    ARRLIST_APPEND(&list, 2);
    ARRLIST_APPEND(&list, 3);

    ARRLIST_POP(&list);

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.buffer[0], 1);
    EXPECT_EQ(list.buffer[1], 2);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, PopMultipleTimes) {
    ArrList_int list = {};

    for (int i = 0; i < 10; i++) {
        ARRLIST_APPEND(&list, i);
    }

    for (int i = 0; i < 5; i++) {
        ARRLIST_POP(&list);
    }

    EXPECT_EQ(list.count, 5);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, PopEmptyListIsNoOp) {
    ArrList_int list = {};

    ARRLIST_POP(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.buffer, nullptr);
}

// ============================================================================
// ARRLIST_CLEAR Tests
// ============================================================================

TEST_F(ArrListTest, ClearRemovesAllElements) {
    ArrList_int list = {};

    for (int i = 0; i < 100; i++) {
        ARRLIST_APPEND(&list, i);
    }

    ARRLIST_CLEAR(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_NE(list.buffer, nullptr);
    EXPECT_GT(list.capacity, 0);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, AppendAfterClear) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 1);
    ARRLIST_APPEND(&list, 2);
    ARRLIST_CLEAR(&list);

    ARRLIST_APPEND(&list, 42);

    EXPECT_EQ(list.count, 1);
    EXPECT_EQ(list.buffer[0], 42);

    ARRLIST_FREE(&list);
}

// ============================================================================
// ARRLIST_TRIM Tests
// ============================================================================

TEST_F(ArrListTest, TrimReducesCapacityToCount) {
    ArrList_int list = {};

    for (int i = 0; i < 10; i++) {
        ARRLIST_APPEND(&list, i);
    }
    size_t original_capacity = list.capacity;

    ARRLIST_POP(&list);
    ARRLIST_POP(&list);
    ARRLIST_TRIM(&list);

    EXPECT_EQ(list.count, 8);
    EXPECT_EQ(list.capacity, 8);
    EXPECT_LT(list.capacity, original_capacity);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, TrimEmptyListFreesBuffer) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 1);
    ARRLIST_APPEND(&list, 2);
    ARRLIST_CLEAR(&list);
    ARRLIST_TRIM(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.capacity, 0);
    EXPECT_EQ(list.buffer, nullptr);
}

TEST_F(ArrListTest, TrimPreservesData) {
    ArrList_int list = {};

    for (int i = 0; i < 10; i++) {
        ARRLIST_APPEND(&list, i);
    }

    ARRLIST_TRIM(&list);

    EXPECT_EQ(list.count, 10);
    EXPECT_EQ(list.capacity, 10);
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(list.buffer[i], i);
    }

    ARRLIST_FREE(&list);
}

// ============================================================================
// ARRLIST_FREE Tests
// ============================================================================

TEST_F(ArrListTest, FreeReleasesMemory) {
    ArrList_int list = {};

    for (int i = 0; i < 10; i++) {
        ARRLIST_APPEND(&list, i);
    }

    ARRLIST_FREE(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.capacity, 0);
    EXPECT_EQ(list.buffer, nullptr);
}

TEST_F(ArrListTest, FreeEmptyListIsSafe) {
    ArrList_int list = {};

    ARRLIST_FREE(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.capacity, 0);
    EXPECT_EQ(list.buffer, nullptr);
}

TEST_F(ArrListTest, DoubleFreeIsSafe) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 1);
    ARRLIST_FREE(&list);
    ARRLIST_FREE(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.capacity, 0);
    EXPECT_EQ(list.buffer, nullptr);
}

// ============================================================================
// ARRLIST_RESERVE Tests
// ============================================================================

TEST_F(ArrListTest, ReserveIncreasesCapacity) {
    ArrList_int list = {};

    ARRLIST_RESERVE(&list, 100);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.capacity, 100);
    EXPECT_NE(list.buffer, nullptr);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, ReservePreservesData) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 1);
    ARRLIST_APPEND(&list, 2);
    ARRLIST_APPEND(&list, 3);

    ARRLIST_RESERVE(&list, 100);

    EXPECT_EQ(list.count, 3);
    EXPECT_EQ(list.buffer[0], 1);
    EXPECT_EQ(list.buffer[1], 2);
    EXPECT_EQ(list.buffer[2], 3);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, ReserveWithReallocFailureDoesNotCorrupt) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 1);
    ARRLIST_APPEND(&list, 2);

    size_t original_capacity = list.capacity;
    void *original_buffer = list.buffer;

    // Try to reserve an extremely large size (will likely fail)
    ARRLIST_RESERVE(&list, SIZE_MAX);

    // Should keep original state on failure
    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.buffer[0], 1);
    EXPECT_EQ(list.buffer[1], 2);

    ARRLIST_FREE(&list);
}

// ============================================================================
// Capacity Growth Tests
// ============================================================================

TEST_F(ArrListTest, AutoGrowthWorks) {
    ArrList_int list = {};

    size_t last_capacity = 0;
    for (int i = 0; i < 1000; i++) {
        ARRLIST_APPEND(&list, i);
        if (list.capacity != last_capacity) {
            // Capacity should only increase
            EXPECT_GT(list.capacity, last_capacity);
            last_capacity = list.capacity;
        }
    }

    EXPECT_EQ(list.count, 1000);

    ARRLIST_FREE(&list);
}

// ============================================================================
// Type Tests
// ============================================================================

TEST_F(ArrListTest, CharTypeWorks) {
    ArrList_char list = {};

    ARRLIST_APPEND(&list, 'a');
    ARRLIST_APPEND(&list, 'b');
    ARRLIST_APPEND(&list, 'c');

    EXPECT_EQ(list.count, 3);
    EXPECT_EQ(list.buffer[0], 'a');
    EXPECT_EQ(list.buffer[1], 'b');
    EXPECT_EQ(list.buffer[2], 'c');

    ARRLIST_FREE(&list);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(ArrListTest, LargeNumberOfElements) {
    ArrList_int list = {};

    for (int i = 0; i < 10000; i++) {
        ARRLIST_APPEND(&list, i);
    }

    EXPECT_EQ(list.count, 10000);

    // Spot check some elements
    EXPECT_EQ(list.buffer[0], 0);
    EXPECT_EQ(list.buffer[9999], 9999);

    ARRLIST_FREE(&list);
}

TEST_F(ArrListTest, InsertThenPopThenAppend) {
    ArrList_int list = {};

    ARRLIST_APPEND(&list, 1);
    ARRLIST_APPEND(&list, 3);
    ARRLIST_INSERT(&list, 1, 2);
    ARRLIST_POP(&list);
    ARRLIST_APPEND(&list, 10);

    EXPECT_EQ(list.count, 3);
    EXPECT_EQ(list.buffer[0], 1);
    EXPECT_EQ(list.buffer[1], 2);
    EXPECT_EQ(list.buffer[2], 10);

    ARRLIST_FREE(&list);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
