#include <stdlib.h>
#include <stdio.h>

#include "list.h"

void
initNode(ListNode **node, const unsigned char *val)
{
    *node = (ListNode *)malloc(sizeof(ListNode));

    (*node)->value = val;
    (*node)->next = NULL;
}

void
addToList(ListNode **node, const unsigned char *value)
{
    ListNode *newNode;
    initNode(&newNode, value);

    newNode->next = *node;
    *node = newNode;
}

void
iterateList(ListNode *node, void(*callback)(ListNode *), void(*complete)(void))
{
    ListNode *cur = node;
    while(cur != NULL){
        callback(cur);
        cur = cur->next;
    }

    complete();
}
