#include <gtest/gtest.h>

extern "C" {
#include <slist/slist.h>
}

DEFINE_SLIST(int)
DEFINE_SLIST(float)
DEFINE_SLIST(char)

class SListTest : public ::testing::Test {
  protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ============================================================================
// SLIST_INSERT Tests
// ============================================================================

TEST_F(SListTest, InsertSingleElement) {
    SList_int list = {};

    int *result = SLIST_INSERT(&list, 42);

    EXPECT_EQ(list.count, 1);
    EXPECT_NE(list.head, nullptr);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*result, 42);
    EXPECT_EQ(list.head->data, 42);

    SLIST_FREE(&list);
}

TEST_F(SListTest, InsertMultipleElements) {
    SList_int list = {};

    SLIST_INSERT(&list, 1);
    SLIST_INSERT(&list, 2);
    SLIST_INSERT(&list, 3);

    EXPECT_EQ(list.count, 3);
    // Elements are inserted at head, so order is reversed
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 2);
    EXPECT_EQ(list.head->next->next->data, 1);

    SLIST_FREE(&list);
}

TEST_F(SListTest, InsertReturnsPointerToData) {
    SList_int list = {};

    int *ptr1 = SLIST_INSERT(&list, 10);
    int *ptr2 = SLIST_INSERT(&list, 20);

    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_EQ(*ptr1, 10);
    EXPECT_EQ(*ptr2, 20);

    SLIST_FREE(&list);
}

TEST_F(SListTest, InsertFloat) {
    SList_float list = {};

    SLIST_INSERT(&list, 1.5f);
    SLIST_INSERT(&list, 2.5f);
    SLIST_INSERT(&list, 3.5f);

    EXPECT_EQ(list.count, 3);
    EXPECT_FLOAT_EQ(list.head->data, 3.5f);
    EXPECT_FLOAT_EQ(list.head->next->data, 2.5f);
    EXPECT_FLOAT_EQ(list.head->next->next->data, 1.5f);

    SLIST_FREE(&list);
}

// ============================================================================
// SLIST_POP Tests
// ============================================================================

TEST_F(SListTest, PopRemovesHead) {
    SList_int list = {};

    SLIST_INSERT(&list, 1);
    SLIST_INSERT(&list, 2);
    SLIST_INSERT(&list, 3);

    SLIST_POP(&list);

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 2);
    EXPECT_EQ(list.head->next->data, 1);

    SLIST_FREE(&list);
}

TEST_F(SListTest, PopMultipleTimes) {
    SList_int list = {};

    for (int i = 0; i < 10; i++) {
        SLIST_INSERT(&list, i);
    }

    for (int i = 0; i < 5; i++) {
        SLIST_POP(&list);
    }

    EXPECT_EQ(list.count, 5);

    SLIST_FREE(&list);
}

TEST_F(SListTest, PopEmptyListIsSafe) {
    SList_int list = {};

    SLIST_POP(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

TEST_F(SListTest, PopAllElements) {
    SList_int list = {};

    SLIST_INSERT(&list, 1);
    SLIST_INSERT(&list, 2);
    SLIST_INSERT(&list, 3);

    SLIST_POP(&list);
    SLIST_POP(&list);
    SLIST_POP(&list);
    SLIST_POP(&list); // One extra pop should be safe

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

// ============================================================================
// SLIST_REMOVE Tests
// ============================================================================

TEST_F(SListTest, RemoveHead) {
    SList_int list = {};

    int *ptr1 = SLIST_INSERT(&list, 1);
    int *ptr2 = SLIST_INSERT(&list, 2);
    int *ptr3 = SLIST_INSERT(&list, 3);

    SLIST_REMOVE(&list, ptr3); // Remove head

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 2);
    EXPECT_EQ(list.head->next->data, 1);

    SLIST_FREE(&list);
}

TEST_F(SListTest, RemoveMiddle) {
    SList_int list = {};

    int *ptr1 = SLIST_INSERT(&list, 1);
    int *ptr2 = SLIST_INSERT(&list, 2);
    int *ptr3 = SLIST_INSERT(&list, 3);

    SLIST_REMOVE(&list, ptr2); // Remove middle

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 1);

    SLIST_FREE(&list);
}

TEST_F(SListTest, RemoveTail) {
    SList_int list = {};

    int *ptr1 = SLIST_INSERT(&list, 1);
    int *ptr2 = SLIST_INSERT(&list, 2);
    int *ptr3 = SLIST_INSERT(&list, 3);

    SLIST_REMOVE(&list, ptr1); // Remove tail

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 2);

    SLIST_FREE(&list);
}

TEST_F(SListTest, RemoveOnlyElement) {
    SList_int list = {};

    int *ptr = SLIST_INSERT(&list, 42);
    SLIST_REMOVE(&list, ptr);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

TEST_F(SListTest, RemoveInvalidPointerDoesNothing) {
    SList_int list = {};

    SLIST_INSERT(&list, 1);
    SLIST_INSERT(&list, 2);
    size_t original_count = list.count;

    int dummy = 999;
    SLIST_REMOVE(&list, &dummy); // Try to remove invalid pointer

    EXPECT_EQ(list.count, original_count);
    EXPECT_EQ(list.head->data, 2);

    SLIST_FREE(&list);
}

// ============================================================================
// SLIST_GET Tests
// ============================================================================

TEST_F(SListTest, GetHead) {
    SList_int list = {};

    SLIST_INSERT(&list, 1);
    SLIST_INSERT(&list, 2);
    SLIST_INSERT(&list, 3);

    int *result = SLIST_GET(&list, 0);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*result, 3);

    SLIST_FREE(&list);
}

TEST_F(SListTest, GetMiddle) {
    SList_int list = {};

    SLIST_INSERT(&list, 1);
    SLIST_INSERT(&list, 2);
    SLIST_INSERT(&list, 3);

    int *result = SLIST_GET(&list, 1);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*result, 2);

    SLIST_FREE(&list);
}

TEST_F(SListTest, GetTail) {
    SList_int list = {};

    SLIST_INSERT(&list, 1);
    SLIST_INSERT(&list, 2);
    SLIST_INSERT(&list, 3);

    int *result = SLIST_GET(&list, 2);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*result, 1);

    SLIST_FREE(&list);
}

TEST_F(SListTest, GetOutOfBoundsReturnsNull) {
    SList_int list = {};

    SLIST_INSERT(&list, 1);
    SLIST_INSERT(&list, 2);

    int *result = SLIST_GET(&list, 10);

    EXPECT_EQ(result, nullptr);

    SLIST_FREE(&list);
}

TEST_F(SListTest, GetFromEmptyListReturnsNull) {
    SList_int list = {};

    int *result = SLIST_GET(&list, 0);

    EXPECT_EQ(result, nullptr);
}

// ============================================================================
// SLIST_FREE Tests
// ============================================================================

TEST_F(SListTest, FreeReleasesMemory) {
    SList_int list = {};

    for (int i = 0; i < 10; i++) {
        SLIST_INSERT(&list, i);
    }

    SLIST_FREE(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

TEST_F(SListTest, FreeEmptyListIsSafe) {
    SList_int list = {};

    SLIST_FREE(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

TEST_F(SListTest, DoubleFreeIsSafe) {
    SList_int list = {};

    SLIST_INSERT(&list, 1);
    SLIST_FREE(&list);
    SLIST_FREE(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

// ============================================================================
// Complex Operations
// ============================================================================

TEST_F(SListTest, InsertThenRemoveThenInsert) {
    SList_int list = {};

    int *ptr1 = SLIST_INSERT(&list, 1);
    int *ptr2 = SLIST_INSERT(&list, 2);
    SLIST_REMOVE(&list, ptr1);
    SLIST_INSERT(&list, 3);

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 2);

    SLIST_FREE(&list);
}

TEST_F(SListTest, MixedOperations) {
    SList_int list = {};

    SLIST_INSERT(&list, 1);
    SLIST_INSERT(&list, 2);
    SLIST_POP(&list);
    SLIST_INSERT(&list, 3);
    int *ptr = SLIST_INSERT(&list, 4);
    SLIST_REMOVE(&list, ptr);

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 1);

    SLIST_FREE(&list);
}

// ============================================================================
// Type Tests
// ============================================================================

TEST_F(SListTest, CharTypeWorks) {
    SList_char list = {};

    SLIST_INSERT(&list, 'a');
    SLIST_INSERT(&list, 'b');
    SLIST_INSERT(&list, 'c');

    EXPECT_EQ(list.count, 3);
    EXPECT_EQ(list.head->data, 'c');
    EXPECT_EQ(list.head->next->data, 'b');
    EXPECT_EQ(list.head->next->next->data, 'a');

    SLIST_FREE(&list);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(SListTest, LargeNumberOfElements) {
    SList_int list = {};

    for (int i = 0; i < 1000; i++) {
        SLIST_INSERT(&list, i);
    }

    EXPECT_EQ(list.count, 1000);

    // Verify we can access elements via SLIST_GET
    int *first = SLIST_GET(&list, 0);
    int *last = SLIST_GET(&list, 999);

    EXPECT_NE(first, nullptr);
    EXPECT_NE(last, nullptr);
    EXPECT_EQ(*first, 999); // Last inserted is at head (index 0)
    EXPECT_EQ(*last, 0);    // First inserted is at tail (index 999)

    SLIST_FREE(&list);
}

TEST_F(SListTest, RemoveMultipleElements) {
    SList_int list = {};

    int *ptr1 = SLIST_INSERT(&list, 1);
    int *ptr2 = SLIST_INSERT(&list, 2);
    int *ptr3 = SLIST_INSERT(&list, 3);
    int *ptr4 = SLIST_INSERT(&list, 4);
    int *ptr5 = SLIST_INSERT(&list, 5);

    SLIST_REMOVE(&list, ptr2);
    SLIST_REMOVE(&list, ptr4);
    SLIST_REMOVE(&list, ptr5);

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 1);

    SLIST_FREE(&list);
}

TEST_F(SListTest, IterateUsingGet) {
    SList_int list = {};

    for (int i = 0; i < 10; i++) {
        SLIST_INSERT(&list, i);
    }

    // Elements are in reverse order due to head insertion
    int expected = 9;
    for (size_t i = 0; i < list.count; i++) {
        int *elem = SLIST_GET(&list, i);
        EXPECT_NE(elem, nullptr);
        EXPECT_EQ(*elem, expected--);
    }

    SLIST_FREE(&list);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
