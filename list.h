#ifndef LIST_H
#define LIST_H

typedef struct ListNode{
    void *value;
    struct ListNode *next;
} ListNode;

void initNode(ListNode **node, void *val);

void addToList(ListNode **node, void *val);

void iterateList(ListNode *node, void *args, void(*callback)(ListNode *, void *), void(*complete)(void *));

#endif
