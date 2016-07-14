#include <stdlib.h>
#include <stdio.h>

#include "list.h"

void
initNode(ListNode **node, void *val)
{
    *node = (ListNode *)malloc(sizeof(ListNode));

    (*node)->value = val;
    (*node)->next = NULL;
}

void
addToList(ListNode **node, void *value)
{
    ListNode *newNode;
    initNode(&newNode, value);

    newNode->next = *node;
    *node = newNode;
}

void
iterateList(ListNode *node, void *args, void(*callback)(ListNode *, void *), void(*complete)(void *))
{
    ListNode *cur = node;
    while(cur != NULL){
        callback(cur, args);
        cur = cur->next;
    }

    complete(args);
}
