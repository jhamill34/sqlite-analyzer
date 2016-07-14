#include <stdio.h>
#include <stdlib.h>

#include "list.h"

void nodePrint(ListNode *node){
    printf("%s -> ", node->value); 
}

void complete(){
    printf("NULL\n");
}

int
main(int argc, char *argv[])
{
    ListNode *root;

    const unsigned char *name = "Josh";

    addToList(&root, "Hello");
    addToList(&root, "World");
    addToList(&root, (void *)name);

    iterateList(root, &nodePrint, &complete);
    return 0;
}

