#include <gtest/gtest.h>

extern "C" {
#include <list/list.h>
}

DEFINE_LIST(int)
DEFINE_LIST(float)
DEFINE_LIST(char)

class ListTest : public ::testing::Test {
  protected:
    void SetUp() override {}
    void TearDown() override {}
};

// ============================================================================
// LIST_INSERT Tests
// ============================================================================

TEST_F(ListTest, InsertSingleElement) {
    List_int list = {};

    int *result = LIST_INSERT(&list, 42);

    EXPECT_EQ(list.count, 1);
    EXPECT_NE(list.head, nullptr);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*result, 42);
    EXPECT_EQ(list.head->data, 42);

    LIST_FREE(&list);
}

TEST_F(ListTest, InsertMultipleElements) {
    List_int list = {};

    LIST_INSERT(&list, 1);
    LIST_INSERT(&list, 2);
    LIST_INSERT(&list, 3);

    EXPECT_EQ(list.count, 3);
    // Elements are inserted at head, so order is reversed
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 2);
    EXPECT_EQ(list.head->next->next->data, 1);

    LIST_FREE(&list);
}

TEST_F(ListTest, InsertReturnsPointerToData) {
    List_int list = {};

    int *ptr1 = LIST_INSERT(&list, 10);
    int *ptr2 = LIST_INSERT(&list, 20);

    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    EXPECT_EQ(*ptr1, 10);
    EXPECT_EQ(*ptr2, 20);

    LIST_FREE(&list);
}

TEST_F(ListTest, InsertFloat) {
    List_float list = {};

    LIST_INSERT(&list, 1.5f);
    LIST_INSERT(&list, 2.5f);
    LIST_INSERT(&list, 3.5f);

    EXPECT_EQ(list.count, 3);
    EXPECT_FLOAT_EQ(list.head->data, 3.5f);
    EXPECT_FLOAT_EQ(list.head->next->data, 2.5f);
    EXPECT_FLOAT_EQ(list.head->next->next->data, 1.5f);

    LIST_FREE(&list);
}

// ============================================================================
// LIST_POP Tests
// ============================================================================

TEST_F(ListTest, PopRemovesHead) {
    List_int list = {};

    LIST_INSERT(&list, 1);
    LIST_INSERT(&list, 2);
    LIST_INSERT(&list, 3);

    LIST_POP(&list);

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 2);
    EXPECT_EQ(list.head->next->data, 1);

    LIST_FREE(&list);
}

TEST_F(ListTest, PopMultipleTimes) {
    List_int list = {};

    for (int i = 0; i < 10; i++) {
        LIST_INSERT(&list, i);
    }

    for (int i = 0; i < 5; i++) {
        LIST_POP(&list);
    }

    EXPECT_EQ(list.count, 5);

    LIST_FREE(&list);
}

TEST_F(ListTest, PopEmptyListIsSafe) {
    List_int list = {};

    LIST_POP(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

TEST_F(ListTest, PopAllElements) {
    List_int list = {};

    LIST_INSERT(&list, 1);
    LIST_INSERT(&list, 2);
    LIST_INSERT(&list, 3);

    LIST_POP(&list);
    LIST_POP(&list);
    LIST_POP(&list);
    LIST_POP(&list); // One extra pop should be safe

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

// ============================================================================
// LIST_REMOVE Tests
// ============================================================================

TEST_F(ListTest, RemoveHead) {
    List_int list = {};

    int *ptr1 = LIST_INSERT(&list, 1);
    int *ptr2 = LIST_INSERT(&list, 2);
    int *ptr3 = LIST_INSERT(&list, 3);

    LIST_REMOVE(&list, ptr3); // Remove head

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 2);
    EXPECT_EQ(list.head->next->data, 1);

    LIST_FREE(&list);
}

TEST_F(ListTest, RemoveMiddle) {
    List_int list = {};

    int *ptr1 = LIST_INSERT(&list, 1);
    int *ptr2 = LIST_INSERT(&list, 2);
    int *ptr3 = LIST_INSERT(&list, 3);

    LIST_REMOVE(&list, ptr2); // Remove middle

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 1);

    LIST_FREE(&list);
}

TEST_F(ListTest, RemoveTail) {
    List_int list = {};

    int *ptr1 = LIST_INSERT(&list, 1);
    int *ptr2 = LIST_INSERT(&list, 2);
    int *ptr3 = LIST_INSERT(&list, 3);

    LIST_REMOVE(&list, ptr1); // Remove tail

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 2);

    LIST_FREE(&list);
}

TEST_F(ListTest, RemoveOnlyElement) {
    List_int list = {};

    int *ptr = LIST_INSERT(&list, 42);
    LIST_REMOVE(&list, ptr);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

TEST_F(ListTest, RemoveInvalidPointerDoesNothing) {
    List_int list = {};

    LIST_INSERT(&list, 1);
    LIST_INSERT(&list, 2);
    size_t original_count = list.count;

    int dummy = 999;
    LIST_REMOVE(&list, &dummy); // Try to remove invalid pointer

    EXPECT_EQ(list.count, original_count);
    EXPECT_EQ(list.head->data, 2);

    LIST_FREE(&list);
}

// ============================================================================
// LIST_GET Tests
// ============================================================================

TEST_F(ListTest, GetHead) {
    List_int list = {};

    LIST_INSERT(&list, 1);
    LIST_INSERT(&list, 2);
    LIST_INSERT(&list, 3);

    int *result = LIST_GET(&list, 0);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*result, 3);

    LIST_FREE(&list);
}

TEST_F(ListTest, GetMiddle) {
    List_int list = {};

    LIST_INSERT(&list, 1);
    LIST_INSERT(&list, 2);
    LIST_INSERT(&list, 3);

    int *result = LIST_GET(&list, 1);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*result, 2);

    LIST_FREE(&list);
}

TEST_F(ListTest, GetTail) {
    List_int list = {};

    LIST_INSERT(&list, 1);
    LIST_INSERT(&list, 2);
    LIST_INSERT(&list, 3);

    int *result = LIST_GET(&list, 2);

    EXPECT_NE(result, nullptr);
    EXPECT_EQ(*result, 1);

    LIST_FREE(&list);
}

TEST_F(ListTest, GetOutOfBoundsReturnsNull) {
    List_int list = {};

    LIST_INSERT(&list, 1);
    LIST_INSERT(&list, 2);

    int *result = LIST_GET(&list, 10);

    EXPECT_EQ(result, nullptr);

    LIST_FREE(&list);
}

TEST_F(ListTest, GetFromEmptyListReturnsNull) {
    List_int list = {};

    int *result = LIST_GET(&list, 0);

    EXPECT_EQ(result, nullptr);
}

// ============================================================================
// LIST_FREE Tests
// ============================================================================

TEST_F(ListTest, FreeReleasesMemory) {
    List_int list = {};

    for (int i = 0; i < 10; i++) {
        LIST_INSERT(&list, i);
    }

    LIST_FREE(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

TEST_F(ListTest, FreeEmptyListIsSafe) {
    List_int list = {};

    LIST_FREE(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

TEST_F(ListTest, DoubleFreeIsSafe) {
    List_int list = {};

    LIST_INSERT(&list, 1);
    LIST_FREE(&list);
    LIST_FREE(&list);

    EXPECT_EQ(list.count, 0);
    EXPECT_EQ(list.head, nullptr);
}

// ============================================================================
// Complex Operations
// ============================================================================

TEST_F(ListTest, InsertThenRemoveThenInsert) {
    List_int list = {};

    int *ptr1 = LIST_INSERT(&list, 1);
    int *ptr2 = LIST_INSERT(&list, 2);
    LIST_REMOVE(&list, ptr1);
    LIST_INSERT(&list, 3);

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 2);

    LIST_FREE(&list);
}

TEST_F(ListTest, MixedOperations) {
    List_int list = {};

    LIST_INSERT(&list, 1);
    LIST_INSERT(&list, 2);
    LIST_POP(&list);
    LIST_INSERT(&list, 3);
    int *ptr = LIST_INSERT(&list, 4);
    LIST_REMOVE(&list, ptr);

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 1);

    LIST_FREE(&list);
}

// ============================================================================
// Type Tests
// ============================================================================

TEST_F(ListTest, CharTypeWorks) {
    List_char list = {};

    LIST_INSERT(&list, 'a');
    LIST_INSERT(&list, 'b');
    LIST_INSERT(&list, 'c');

    EXPECT_EQ(list.count, 3);
    EXPECT_EQ(list.head->data, 'c');
    EXPECT_EQ(list.head->next->data, 'b');
    EXPECT_EQ(list.head->next->next->data, 'a');

    LIST_FREE(&list);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_F(ListTest, LargeNumberOfElements) {
    List_int list = {};

    for (int i = 0; i < 1000; i++) {
        LIST_INSERT(&list, i);
    }

    EXPECT_EQ(list.count, 1000);

    // Verify we can access elements via LIST_GET
    int *first = LIST_GET(&list, 0);
    int *last = LIST_GET(&list, 999);

    EXPECT_NE(first, nullptr);
    EXPECT_NE(last, nullptr);
    EXPECT_EQ(*first, 999); // Last inserted is at head (index 0)
    EXPECT_EQ(*last, 0);    // First inserted is at tail (index 999)

    LIST_FREE(&list);
}

TEST_F(ListTest, RemoveMultipleElements) {
    List_int list = {};

    int *ptr1 = LIST_INSERT(&list, 1);
    int *ptr2 = LIST_INSERT(&list, 2);
    int *ptr3 = LIST_INSERT(&list, 3);
    int *ptr4 = LIST_INSERT(&list, 4);
    int *ptr5 = LIST_INSERT(&list, 5);

    LIST_REMOVE(&list, ptr2);
    LIST_REMOVE(&list, ptr4);
    LIST_REMOVE(&list, ptr5);

    EXPECT_EQ(list.count, 2);
    EXPECT_EQ(list.head->data, 3);
    EXPECT_EQ(list.head->next->data, 1);

    LIST_FREE(&list);
}

TEST_F(ListTest, IterateUsingGet) {
    List_int list = {};

    for (int i = 0; i < 10; i++) {
        LIST_INSERT(&list, i);
    }

    // Elements are in reverse order due to head insertion
    int expected = 9;
    for (size_t i = 0; i < list.count; i++) {
        int *elem = LIST_GET(&list, i);
        EXPECT_NE(elem, nullptr);
        EXPECT_EQ(*elem, expected--);
    }

    LIST_FREE(&list);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
