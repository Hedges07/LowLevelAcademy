#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

	int realcount = dbhdr->count;

	dbhdr->magic = htonl(dbhdr->magic);
	dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
	dbhdr->count = htons(dbhdr->count);
	dbhdr->version = htons(dbhdr->version);

	lseek(fd, 0, SEEK_SET);

	write(fd, dbhdr, sizeof(struct dbheader_t));

	int i = 0;
	for (; i < realcount; i++) {
		employees[i].hours = htonl(employees[i].hours);
		write(fd, &employees[i], sizeof(struct employee_t));
	}

	return STATUS_SUCCESS;
}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("Malloc failed create a db header\n");
		return STATUS_ERROR;
	}

	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}

	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if (header->magic != HEADER_MAGIC) {
		printf("Impromper header magic\n");
		free(header);
		return -1;
	}


	if (header->version != 1) {
		printf("Impromper header version\n");
		free(header);
		return -1;
	}

	struct stat dbstat = {0};
	fstat(fd, &dbstat);
	if (header->filesize != dbstat.st_size) {
		printf("Corrupted database\n");
		free(header);
		return -1;
	}

	*headerOut = header;
    return STATUS_SUCCESS; 
}

int create_db_header(struct dbheader_t **headerOut) {
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("Calloc failed to create db header\n");
		return STATUS_ERROR;
	}

    if (headerOut == NULL) {
		printf("Calloc failed to create db header\n");
		return STATUS_ERROR;
	}

	header->version = 0x1;
	header->count = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t);

	*headerOut = header;
	return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) { 
	if (fd < 0) {
		printf("Got a bad FD from the user\n");
		return STATUS_ERROR;
	}

	int count = dbhdr->count;
	struct employee_t *employees = calloc(count, sizeof(struct employee_t));
	if (employees == NULL) {
		printf("Calloc failed\n");
		return STATUS_ERROR;
	}
	
	read(fd, employees, count*sizeof(struct employee_t));

	int i = 0;
	for(; i < count; i++) {
		employees[i].hours = ntohl(employees[i].hours);
	} 

	*employeesOut = employees;
	return STATUS_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STATUS_SUCCESS 0
#define STATUS_FAILURE 1

struct dbheader_t {
    int count;
};

struct employee_t {
    char name[50];
    char address[100];
    int hours;
};

int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring) {
    printf("%s\n", addstring);

    // Tokenize input string
    char *name = strtok(addstring, ",");
    char *addr = strtok(NULL, ",");
    char *hours_str = strtok(NULL, ",");

    if (!name || !addr || !hours_str) {
        return STATUS_FAILURE; // Invalid input
    }

    // Increase count
    dbhdr->count++;

    // Reallocate memory for employees array
    struct employee_t *new_array = realloc(*employees, dbhdr->count * sizeof(struct employee_t));
    if (!new_array) {
        dbhdr->count--; // Rollback count if allocation fails
        return STATUS_FAILURE;
    }
    *employees = new_array;

    // Add new employee at the end
    struct employee_t *new_emp = &(*employees)[dbhdr->count - 1];
    strncpy(new_emp->name, name, sizeof(new_emp->name) - 1);
    new_emp->name[sizeof(new_emp->name) - 1] = '\0';

    strncpy(new_emp->address, addr, sizeof(new_emp->address) - 1);
    new_emp->address[sizeof(new_emp->address) - 1] = '\0';

    new_emp->hours = atoi(hours_str);

    return STATUS_SUCCESS;
}
