#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#include "list.h"

#define TABLE_QUERY "SELECT name FROM sqlite_master WHERE type = 'table';"
#define ALL_QUERY "SELECT * FROM %s;"
#define ALL_QUERY_SIZE 15

typedef struct IteratorArgs {
    sqlite3 *db;
    long grandTotal;
    char *format; 
} IteratorArgs;

typedef struct TableData {
    long size;
    char *name;
} TableData;

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



void printNode(ListNode *node, void *args){
    IteratorArgs *iArgs = (IteratorArgs *)args;
    TableData *data = (TableData *)node->value;

    if(iArgs->format == NULL || 0 == strcmp(iArgs->format, "--csv")){
        printf("%s,%lu,%lf\n", data->name, data->size, ((double)data->size * 100) / iArgs->grandTotal);
    }else if(0 == strcmp(iArgs->format, "--table")){
        printf("| %-30s | %-8lu | %-10lf |\n", data->name, data->size, ((double)data->size * 100) / iArgs->grandTotal);
    }
}

void completeNode(void *args){
    IteratorArgs *iArgs = (IteratorArgs *)args;
    printf("Grand Total: %lu\n", iArgs->grandTotal);
}

void getTableSize(ListNode *node, void *args){
    IteratorArgs *iArgs = args;

    // Prepare the statement
    sqlite3_stmt *stmt; 
    int col_count; 

    sqlite3_value *val;
    int col_type;
    const char *col_name;
    const unsigned char *col_val;
    long rowsize,cur_row_size,total; 
    total = 0;
    
    char *query = (char *)malloc(sizeof(char) * (ALL_QUERY_SIZE + strlen((char *)node->value)) + 1);
    sprintf(query, ALL_QUERY, (char *)node->value);
    
    // Here we will calculate the size of each table
    if(sqlite3_prepare_v2(iArgs->db, query, -1, &stmt, NULL) == SQLITE_OK){
        // Lets get how many columns we are looking at 
        col_count = sqlite3_column_count(stmt);

        // Loop through each of the rows
        while(sqlite3_step(stmt) == SQLITE_ROW){
            // reset our size counter
            rowsize = 0; 

            int i; 
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
           // printf("Rowsize = %lu bytes", rowsize);

           total += rowsize;
        }
        iArgs->grandTotal += total;
    }
    
    TableData *data = (TableData *)malloc(sizeof(TableData));
    data->size = total;
    data->name = (char *)node->value;
    node->value = data;
    free(query);
}

int 
main(int argc, char *argv[])
{
    int rc;
    sqlite3 *db;
    if(argc < 2){
        fprintf(stderr, "Usage : %s DATABASE \n", argv[0]);
        return 1;
    }

    if(NULL != argv[2] && strcmp(argv[2], "--table") && strcmp(argv[2], "--csv")){
        fprintf(stderr, "Unrecognized option %s", argv[2]);
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
    const unsigned char *col_val;
    unsigned char *col_val_cpy;
   
    ListNode *root = NULL;
    
    // I want a list of all tables
    if(sqlite3_prepare_v2(db, TABLE_QUERY, -1, &stmt, NULL) == SQLITE_OK){
        // Iterate through the list of table names 
        while(sqlite3_step(stmt) == SQLITE_ROW){
            // Get the value out
            col_val = sqlite3_column_text(stmt, 0);

            // Figure out the length of the string
            int val_length = strlen((char *)col_val);
           
            // Allocate some memory to put the copy of the string
            col_val_cpy = (unsigned char *)malloc(sizeof(unsigned char) * val_length + 1);

            // Copy the data over
            memcpy(col_val_cpy, col_val, val_length + 1);

            // Add to list
            addToList(&root, col_val_cpy);
        }
    }else{
        fprintf(stderr, "There was an error with the SQL statement");
        return 1;
    }

    // Finish off the first query with the table names
    sqlite3_finalize(stmt);

    //XXX: Bad code! We don't free the list or the args
    IteratorArgs *args = (IteratorArgs *)malloc(sizeof(IteratorArgs));
    args->db = db;
    args->grandTotal = 0L;
    args->format = argv[2]; // choose c for csv or t for table
    iterateList(root, args, &getTableSize, &completeNode);

    iterateList(root, args, &printNode, &completeNode);
    // Finish up and close the database
    sqlite3_close(db);
    return 0;
}
