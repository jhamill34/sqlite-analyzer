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
    
    initNode(&root, "Hello");
    addToList(&root, "World");
    addToList(&root, "Josh");

    iterateList(root, &nodePrint, &complete);
    return 0;
}

