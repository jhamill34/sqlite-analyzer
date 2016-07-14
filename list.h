#ifndef LIST_H
#define LIST_H

typedef struct ListNode{
    const unsigned char *value;
    struct ListNode *next;
} ListNode;

void initNode(ListNode **node, const unsigned char *val);

void iterateList(ListNode *node, void(*callback)(ListNode *), void(*complete)(void));

void addToList(ListNode **node, const unsigned char *val);

#endif
