#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

/*
Notes:
└──╼ $xxd mynewdb.db 
00000000: 4c4c 4144 0001 0000 0000 000c 
*/

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t -n - create new database file");
    printf("\t -f - (required) path to database file\n");
    return;
}

int main(int argc, char *argv[]) { 

	int c;
    bool newfile = false;
    char *filepath = NULL;
    char *addstring = NULL;
    int dbfd = -1;
    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employees = NULL;

    while ((c = getopt(argc,argv,"nf:a:")) != -1) {
        switch (c) {
        case 'n': {
            newfile = true;
            break;
        }
        case 'f': {
            filepath = optarg;
            break;
        }
        case 'a': {
            addstring = optarg;
            break;
        }
        case '?': {
            printf("Unknown option -%c\n", c);
            break;
        }
            
        default:
            return -1;
        }
    }

    if (filepath == NULL) {
        printf("Filepath is a required argument\n");
        print_usage(argv);
    }

    if (newfile) {
        dbfd = create_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to create database file\n");
            return -1;
        }
        if (create_db_header(&dbhdr) == STATUS_ERROR){
            printf("Failed to create databse header");
            return -1;
        }
    }
    else { 
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to open database file\n");
            return -1;
        }
        if (validate_db_header(dbfd,&dbhdr) == STATUS_ERROR) {
            printf("Failed to validate database header\n");
            return -1;
        }
    }
    
    if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
        printf("Failed to read employees\n");
        if (dbhdr) {
            free(dbhdr);
        }
    }

    if (addstring) {
        dbhdr->count++;
        
        struct employee_t *tmp = realloc(employees, dbhdr->count * sizeof(struct employee_t));
        if (tmp == NULL) {
            printf("Failed to realloc\n");
            return -1;
        }
        employees = tmp;

        add_employee(dbhdr, &employees, addstring);
    }

    output_file(dbfd, dbhdr, employees);
    if (dbhdr) {
        free(dbhdr);
        dbhdr = NULL;
    }
    if (employees) {
        free(employees);
        employees = NULL;
    }
    return 0;
}