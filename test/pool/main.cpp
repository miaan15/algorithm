#include <gtest/gtest.h>

extern "C" {
#include <pool/pool.h>
}

DEFINE_POOL(int)
DEFINE_POOL(float)
DEFINE_POOL(char)

class PoolTest : public ::testing::Test {
  protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ============================================================================
// POOL_INSERT Tests
// ============================================================================

TEST_F(PoolTest, InsertSingleElement) {
    Pool_int pool = {};

    size_t index = POOL_INSERT(&pool, 42);

    EXPECT_EQ(pool.count, 1);
    EXPECT_EQ(pool.max_count, 1);
    EXPECT_EQ(index, 0);
    EXPECT_TRUE(POOL_IS_VALID(&pool, index));
    EXPECT_EQ(pool.buffer[index].data, 42);

    POOL_FREE(&pool);
}

TEST_F(PoolTest, InsertMultipleElements) {
    Pool_int pool = {};

    size_t i0 = POOL_INSERT(&pool, 10);
    size_t i1 = POOL_INSERT(&pool, 20);
    size_t i2 = POOL_INSERT(&pool, 30);

    EXPECT_EQ(pool.count, 3);
    EXPECT_EQ(pool.max_count, 3);
    EXPECT_EQ(i0, 0);
    EXPECT_EQ(i1, 1);
    EXPECT_EQ(i2, 2);

    EXPECT_TRUE(POOL_IS_VALID(&pool, i0));
    EXPECT_TRUE(POOL_IS_VALID(&pool, i1));
    EXPECT_TRUE(POOL_IS_VALID(&pool, i2));

    EXPECT_EQ(pool.buffer[i0].data, 10);
    EXPECT_EQ(pool.buffer[i1].data, 20);
    EXPECT_EQ(pool.buffer[i2].data, 30);

    POOL_FREE(&pool);
}

TEST_F(PoolTest, InsertFloat) {
    Pool_float pool = {};

    size_t i0 = POOL_INSERT(&pool, 1.5f);
    size_t i1 = POOL_INSERT(&pool, 2.5f);
    size_t i2 = POOL_INSERT(&pool, 3.5f);

    EXPECT_EQ(pool.count, 3);
    EXPECT_FLOAT_EQ(pool.buffer[i0].data, 1.5f);
    EXPECT_FLOAT_EQ(pool.buffer[i1].data, 2.5f);
    EXPECT_FLOAT_EQ(pool.buffer[i2].data, 3.5f);

    POOL_FREE(&pool);
}

TEST_F(PoolTest, InsertReturnsStableIndices) {
    Pool_int pool = {};

    size_t i0 = POOL_INSERT(&pool, 1);
    size_t i1 = POOL_INSERT(&pool, 2);
    size_t i2 = POOL_INSERT(&pool, 3);

    EXPECT_EQ(i0, 0);
    EXPECT_EQ(i1, 1);
    EXPECT_EQ(i2, 2);

    POOL_FREE(&pool);
}

// ============================================================================
// POOL_REMOVE Tests
// ============================================================================

TEST_F(PoolTest, RemoveSingleElement) {
    Pool_int pool = {};

    size_t index = POOL_INSERT(&pool, 42);
    POOL_REMOVE(&pool, index);

    EXPECT_EQ(pool.count, 0);
    EXPECT_EQ(pool.max_count, 0);
    EXPECT_FALSE(POOL_IS_VALID(&pool, index));

    POOL_FREE(&pool);
}

TEST_F(PoolTest, RemoveMiddleElement) {
    Pool_int pool = {};

    size_t i0 = POOL_INSERT(&pool, 10);
    size_t i1 = POOL_INSERT(&pool, 20);
    size_t i2 = POOL_INSERT(&pool, 30);

    POOL_REMOVE(&pool, i1);

    EXPECT_EQ(pool.count, 2);
    EXPECT_EQ(pool.max_count, 3);
    EXPECT_TRUE(POOL_IS_VALID(&pool, i0));
    EXPECT_FALSE(POOL_IS_VALID(&pool, i1));
    EXPECT_TRUE(POOL_IS_VALID(&pool, i2));
    EXPECT_EQ(pool.buffer[i0].data, 10);
    EXPECT_EQ(pool.buffer[i2].data, 30);

    POOL_FREE(&pool);
}

TEST_F(PoolTest, RemoveHeadElement) {
    Pool_int pool = {};

    size_t i0 = POOL_INSERT(&pool, 10);
    size_t i1 = POOL_INSERT(&pool, 20);
    size_t i2 = POOL_INSERT(&pool, 30);

    POOL_REMOVE(&pool, i0);

    EXPECT_EQ(pool.count, 2);
    EXPECT_TRUE(POOL_IS_VALID(&pool, i1));
    EXPECT_TRUE(POOL_IS_VALID(&pool, i2));

    POOL_FREE(&pool);
}

TEST_F(PoolTest, RemoveTailElement) {
    Pool_int pool = {};

    size_t i0 = POOL_INSERT(&pool, 10);
    size_t i1 = POOL_INSERT(&pool, 20);
    size_t i2 = POOL_INSERT(&pool, 30);

    POOL_REMOVE(&pool, i2);

    EXPECT_EQ(pool.count, 2);
    EXPECT_EQ(pool.max_count, 2);
    EXPECT_TRUE(POOL_IS_VALID(&pool, i0));
    EXPECT_TRUE(POOL_IS_VALID(&pool, i1));

    POOL_FREE(&pool);
}

TEST_F(PoolTest, RemoveAndReuseIndex) {
    Pool_int pool = {};

    size_t i0 = POOL_INSERT(&pool, 10);
    size_t i1 = POOL_INSERT(&pool, 20);

    POOL_REMOVE(&pool, i0);
    size_t i2 = POOL_INSERT(&pool, 30);

    EXPECT_EQ(pool.count, 2);
    EXPECT_EQ(i2, 0); // Should reuse the removed index
    EXPECT_EQ(pool.buffer[i2].data, 30);

    POOL_FREE(&pool);
}

TEST_F(PoolTest, RemoveInvalidIndexDoesNothing) {
    Pool_int pool = {};

    POOL_INSERT(&pool, 10);
    POOL_INSERT(&pool, 20);
    size_t original_count = pool.count;

    POOL_REMOVE(&pool, 999); // Invalid index

    EXPECT_EQ(pool.count, original_count);

    POOL_FREE(&pool);
}

// ============================================================================
// POOL_IS_VALID Tests
// ============================================================================

TEST_F(PoolTest, IsValidReturnsTrueForValidIndices) {
    Pool_int pool = {};

    size_t i0 = POOL_INSERT(&pool, 10);
    size_t i1 = POOL_INSERT(&pool, 20);

    EXPECT_TRUE(POOL_IS_VALID(&pool, i0));
    EXPECT_TRUE(POOL_IS_VALID(&pool, i1));

    POOL_FREE(&pool);
}

TEST_F(PoolTest, IsValidReturnsFalseForRemovedIndices) {
    Pool_int pool = {};

    size_t i0 = POOL_INSERT(&pool, 10);
    POOL_REMOVE(&pool, i0);

    EXPECT_FALSE(POOL_IS_VALID(&pool, i0));

    POOL_FREE(&pool);
}

TEST_F(PoolTest, IsValidReturnsFalseForOutOfBoundsIndices) {
    Pool_int pool = {};

    POOL_INSERT(&pool, 10);

    EXPECT_FALSE(POOL_IS_VALID(&pool, 100));
    EXPECT_FALSE(POOL_IS_VALID(&pool, 999));

    POOL_FREE(&pool);
}

TEST_F(PoolTest, IsValidReturnsFalseForEmptyPool) {
    Pool_int pool = {};

    EXPECT_FALSE(POOL_IS_VALID(&pool, 0));

    POOL_FREE(&pool);
}

// ============================================================================
// POOL_CLEAR Tests
// ============================================================================

TEST_F(PoolTest, ClearResetsCount) {
    Pool_int pool = {};

    POOL_INSERT(&pool, 10);
    POOL_INSERT(&pool, 20);
    POOL_INSERT(&pool, 30);

    POOL_CLEAR(&pool);

    EXPECT_EQ(pool.count, 0);
    EXPECT_EQ(pool.max_count, 0);
    EXPECT_EQ(pool.head, 0);
    EXPECT_NE(pool.buffer, nullptr); // Buffer should still be allocated

    POOL_FREE(&pool);
}

TEST_F(PoolTest, ClearEmptyPoolIsSafe) {
    Pool_int pool = {};

    POOL_CLEAR(&pool);

    EXPECT_EQ(pool.count, 0);
    EXPECT_EQ(pool.max_count, 0);
    EXPECT_EQ(pool.head, 0);

    POOL_FREE(&pool);
}

TEST_F(PoolTest, ClearAndInsertAgain) {
    Pool_int pool = {};

    POOL_INSERT(&pool, 10);
    POOL_INSERT(&pool, 20);
    POOL_CLEAR(&pool);

    size_t i0 = POOL_INSERT(&pool, 30);

    EXPECT_EQ(pool.count, 1);
    EXPECT_EQ(pool.max_count, 1);
    EXPECT_EQ(i0, 0);
    EXPECT_EQ(pool.buffer[i0].data, 30);

    POOL_FREE(&pool);
}

// ============================================================================
// POOL_TRIM Tests
// ============================================================================

TEST_F(PoolTest, TrimReducesCapacity) {
    Pool_int pool = {};

    POOL_INSERT(&pool, 10);
    POOL_INSERT(&pool, 20);
    POOL_INSERT(&pool, 30);

    size_t original_capacity = pool.capacity;

    POOL_REMOVE(&pool, 1);
    POOL_REMOVE(&pool, 2);
    POOL_TRIM(&pool);

    EXPECT_LT(pool.capacity, original_capacity);
    EXPECT_EQ(pool.capacity, pool.max_count);

    POOL_FREE(&pool);
}

TEST_F(PoolTest, TrimWithNoRemovalsKeepsCapacity) {
    Pool_int pool = {};

    POOL_INSERT(&pool, 10);
    POOL_INSERT(&pool, 20);
    POOL_INSERT(&pool, 30);

    size_t original_capacity = pool.capacity;
    POOL_TRIM(&pool);

    EXPECT_EQ(pool.capacity, pool.max_count);
    EXPECT_EQ(pool.capacity, 3);
    EXPECT_LE(pool.capacity, original_capacity);

    POOL_FREE(&pool);
}

TEST_F(PoolTest, TrimEmptyPool) {
    Pool_int pool = {};

    POOL_INSERT(&pool, 10);
    POOL_REMOVE(&pool, 0);
    POOL_TRIM(&pool);

    EXPECT_EQ(pool.capacity, 0);
    EXPECT_EQ(pool.buffer, nullptr);

    POOL_FREE(&pool);
}

TEST_F(PoolTest, TrimAfterClear) {
    Pool_int pool = {};

    POOL_INSERT(&pool, 10);
    POOL_INSERT(&pool, 20);
    POOL_CLEAR(&pool);
    POOL_TRIM(&pool);

    EXPECT_EQ(pool.capacity, 0);
    EXPECT_EQ(pool.buffer, nullptr);

    POOL_FREE(&pool);
}

// ============================================================================
// POOL_FREE Tests
// ============================================================================

TEST_F(PoolTest, FreeReleasesMemory) {
    Pool_int pool = {};

    POOL_INSERT(&pool, 10);
    POOL_INSERT(&pool, 20);
    POOL_FREE(&pool);

    EXPECT_EQ(pool.buffer, nullptr);
    EXPECT_EQ(pool.capacity, 0);
    EXPECT_EQ(pool.count, 0);
    EXPECT_EQ(pool.max_count, 0);
    EXPECT_EQ(pool.head, 0);
}

TEST_F(PoolTest, FreeEmptyPoolIsSafe) {
    Pool_int pool = {};

    POOL_FREE(&pool);

    EXPECT_EQ(pool.buffer, nullptr);
    EXPECT_EQ(pool.capacity, 0);
}

TEST_F(PoolTest, DoubleFreeIsSafe) {
    Pool_int pool = {};

    POOL_INSERT(&pool, 10);
    POOL_FREE(&pool);
    POOL_FREE(&pool);

    EXPECT_EQ(pool.buffer, nullptr);
    EXPECT_EQ(pool.capacity, 0);
}

// ============================================================================
// Complex Operations
// ============================================================================

TEST_F(PoolTest, InsertRemoveInsertCycle) {
    Pool_int pool = {};

    size_t i0 = POOL_INSERT(&pool, 10);
    size_t i1 = POOL_INSERT(&pool, 20);
    POOL_REMOVE(&pool, i0);
    size_t i2 = POOL_INSERT(&pool, 30);

    EXPECT_EQ(pool.count, 2);
    EXPECT_EQ(i2, 0); // Reused index
    EXPECT_TRUE(POOL_IS_VALID(&pool, i1));
    EXPECT_TRUE(POOL_IS_VALID(&pool, i2));

    POOL_FREE(&pool);
}

TEST_F(PoolTest, MultipleRemovalsAndReuses) {
    Pool_int pool = {};

    size_t i0 = POOL_INSERT(&pool, 10);
    size_t i1 = POOL_INSERT(&pool, 20);
    size_t i2 = POOL_INSERT(&pool, 30);
    size_t i3 = POOL_INSERT(&pool, 40);

    POOL_REMOVE(&pool, i0);
    POOL_REMOVE(&pool, i2);

    size_t i4 = POOL_INSERT(&pool, 50);
    size_t i5 = POOL_INSERT(&pool, 60);

    EXPECT_EQ(pool.count, 4);
    EXPECT_EQ(i4, 2); // Reused i2
    EXPECT_EQ(i5, 0); // Reused i0

    POOL_FREE(&pool);
}

TEST_F(PoolTest, DenseRemovalPattern) {
    Pool_int pool = {};

    for (int i = 0; i < 10; i++) {
        POOL_INSERT(&pool, i);
    }

    EXPECT_EQ(pool.count, 10);

    // Remove every other element
    for (int i = 0; i < 10; i += 2) {
        POOL_REMOVE(&pool, i);
    }

    EXPECT_EQ(pool.count, 5);

    POOL_FREE(&pool);
}

// ============================================================================
// Type Tests
// ============================================================================

TEST_F(PoolTest, CharTypeWorks) {
    Pool_char pool = {};

    size_t i0 = POOL_INSERT(&pool, 'a');
    size_t i1 = POOL_INSERT(&pool, 'b');
    size_t i2 = POOL_INSERT(&pool, 'c');

    EXPECT_EQ(pool.count, 3);
    EXPECT_EQ(pool.buffer[i0].data, 'a');
    EXPECT_EQ(pool.buffer[i1].data, 'b');
    EXPECT_EQ(pool.buffer[i2].data, 'c');

    POOL_FREE(&pool);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(PoolTest, LargeNumberOfElements) {
    Pool_int pool = {};

    for (int i = 0; i < 1000; i++) {
        size_t index = POOL_INSERT(&pool, i);
        EXPECT_EQ(index, i);
    }

    EXPECT_EQ(pool.count, 1000);
    EXPECT_EQ(pool.max_count, 1000);

    // Verify elements
    for (int i = 0; i < 1000; i++) {
        EXPECT_TRUE(POOL_IS_VALID(&pool, i));
        EXPECT_EQ(pool.buffer[i].data, i);
    }

    POOL_FREE(&pool);
}

TEST_F(PoolTest, GrowthBehavior) {
    Pool_int pool = {};

    // Insert many elements to trigger growth
    for (int i = 0; i < 100; i++) {
        POOL_INSERT(&pool, i);
    }

    EXPECT_EQ(pool.count, 100);
    EXPECT_GE(pool.capacity, 100);

    POOL_FREE(&pool);
}

TEST_F(PoolTest, AccessViaStableIndex) {
    Pool_int pool = {};

    size_t indices[100];

    for (int i = 0; i < 100; i++) {
        indices[i] = POOL_INSERT(&pool, i * 10);
    }

    // Remove some elements
    POOL_REMOVE(&pool, indices[5]);
    POOL_REMOVE(&pool, indices[10]);
    POOL_REMOVE(&pool, indices[15]);

    // Verify remaining elements are still accessible via original indices
    for (int i = 0; i < 100; i++) {
        if (i == 5 || i == 10 || i == 15) {
            EXPECT_FALSE(POOL_IS_VALID(&pool, indices[i]));
        } else {
            EXPECT_TRUE(POOL_IS_VALID(&pool, indices[i]));
            EXPECT_EQ(pool.buffer[indices[i]].data, i * 10);
        }
    }

    POOL_FREE(&pool);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
