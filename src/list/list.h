#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdlib.h>

typedef struct _ListNode_char _ListNode_char;
struct _ListNode_char {
    _ListNode_char *next;
    char data;
};

typedef struct {
    size_t count;
    _ListNode_char *head;
} List_char;

char *list_insert(List_char *list, char value) {
    _ListNode_char *node = malloc(sizeof(_ListNode_char));
    if (node == nullptr) return nullptr;

    node->data = value;
    node->next = list->head;
    list->head = node;
    list->count++;

    return &node->data;
}

void list_pop(List_char *list) {
    if (list->head == NULL) return;

    auto old_head = list->head;
    list->head = list->head->next;
    list->count--;
    free(old_head);
}

void list_remove(List_char *list, char *value) {
    auto cur_node = list->head;
    _ListNode_char *pre_node = nullptr;
    while (cur_node != nullptr) {
        if (&cur_node->data == value) {
            if (pre_node != nullptr) {
                pre_node->next = cur_node->next;
            } else {
                list->head = cur_node->next;
            }

            list->count--;
            free(cur_node);

            return;
        }

        pre_node = cur_node;
        cur_node = cur_node->next;
    }
}

char *list_get(List_char *list, size_t index) {
    if (index >= list->count) return nullptr;

    auto cur_node = list->head;
    for (size_t i = 0; i < index; i++) {
        cur_node = cur_node->next;
    }

    return &cur_node->data;
}

void list_free(List_char *list) {
    auto cur_node = list->head;
    while (cur_node != nullptr) {
        auto next = cur_node->next;
        free(cur_node);
        cur_node = next;
    }

    list->head = nullptr;
    list->count = 0;
}

#endif
