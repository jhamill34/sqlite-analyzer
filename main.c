#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#include "list.h"

#define TABLE_QUERY "SELECT name FROM sqlite_master WHERE type = 'table';"
#define ALL_QUERY "SELECT * FROM %s;"

int 
get_integer_size(sqlite_int64 value)
{
    sqlite_int64 mask = 1;
    if((((1 << 8) - 1) & value) == value){
        return 1;    
    } else if((((mask << 16) - 1) & value) == value){
        return 2;
    } else if((((mask << 24) - 1) & value) == value){
        return 3;
    } else if((((mask << 32) - 1) & value) == value){
        return 4; 
    } else if((((mask << 48) - 1) & value) == value){
        return 6;
    } else {
        return 8;
    }

    return 0;
}

void printNode(ListNode *node){
    printf("%s -> ", node->value);
}

void completeNode(){
    printf("NULL \n");
}

int 
main(int argc, char *argv[])
{
    sqlite3 *db;
    int rc;
    if(argc != 2){
        fprintf(stderr, "Usage : %s DATABASE \n", argv[0]);
        return 1;
    }

    // Open up the database
    rc = sqlite3_open(argv[1], &db);

    // Log some output if we fail to open it
    if(rc){
        fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    // Prepare the statement
    sqlite3_stmt *stmt; 
    int col_count, 
        i;

    sqlite3_value *val;
    int col_type;
    const char *col_name;
    const unsigned char *col_val;
    long rowsize,cur_row_size,total; 
    
    total = 0;
   
    ListNode *root;
    // I want a list of all tables
    if(sqlite3_prepare_v2(db, TABLE_QUERY, -1, &stmt, NULL) == SQLITE_OK){
        // Iterate through the list of table names 
        while(sqlite3_step(stmt) == SQLITE_ROW){
            val = sqlite3_column_value(stmt, 0);
            
            col_val = sqlite3_value_text(val);
            addToList(&root, col_val);

        }
        iterateList(root, &printNode, &completeNode);
    }
    // Finish off the first query with the table names
    sqlite3_finalize(stmt);


    // Here we will calculate the size of each table
    if(0){
    //if(sqlite3_prepare_v2(db, "", -1, &stmt, NULL) == SQLITE_OK){
        // Lets get how many columns we are looking at 
        col_count = sqlite3_column_count(stmt);

        // Loop through each of the rows
        while(sqlite3_step(stmt) == SQLITE_ROW){
            // reset our size counter
            rowsize = 0; 
            
            // Access each column for the row
            for(i = 0; i < col_count; i++){ 
               // pull out the value
               val = sqlite3_column_value(stmt, i);

               // Get the column type (used to calculate row size)
               col_type = sqlite3_value_type(val);
               switch(col_type){
                   case SQLITE_INTEGER: 
                       cur_row_size = get_integer_size(sqlite3_value_int64(val));
                       break;
                   case SQLITE_FLOAT:
                       cur_row_size = 8; 
                       break;
                   case SQLITE_NULL:
                       cur_row_size = 1;
                       break;
                   case SQLITE_TEXT:
                       cur_row_size = sqlite3_value_bytes(val);
                       break;
                   case SQLITE_BLOB:
                       cur_row_size = sqlite3_value_bytes(val);
                       break;
               }


               // Get the column name for displaying
               col_name = sqlite3_column_name(stmt, i);
               
               // convert column value to string to display
               col_val = sqlite3_value_text(val);
               //printf("%s = %s [%d][%lu byte(s)]\n", col_name, col_val, col_type, cur_row_size);

               rowsize += cur_row_size;
           }
           printf("Rowsize = %lu bytes", rowsize);

           total += rowsize;
           printf("\n");
        }
        printf("Total Query Size = %lu bytes", total);
    }

    // Finish up and close the database
    sqlite3_close(db);
    return 0;
}
