#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "psmem.h"

int all_digits(const char *str) {
	for(; *str != '\0'; ++str) {
		if((*str) < '0' || (*str) > '9') {
			return 1;
		}
	}
	return 0;
}


int main(void) {
	DIR* dp = opendir("/proc");
	char path[16384];
	char selfpath[16384];
	double total = 0.0;
	struct program curr_program;
	struct dirent *curr;
	struct node *head,*currnode;
	head = NULL;
	sprintf(selfpath, "/proc/%ld",(long) getpid());
	/*initalize our struct for safety, worst case scenario we print all zeroes */
	memset(&curr_program, 0, sizeof(curr_program)); 
	if (!dp) {
		return 1;
	}
	printHeader();
	while((curr = readdir(dp))) {
		if(curr->d_type == DT_DIR && all_digits(curr->d_name) == 0) {
			sprintf(path, "/proc/%s", curr->d_name);
			if (strcmp(path, selfpath) == 0) {
				continue;
			}
			if (read_proc(&curr_program, path) != 0) {
				continue;
			}
			prog_list_add(&head,curr_program);
			total += curr_program.private_mem + curr_program.shared_mem;
		}
	}
	mergeSort(&head);
	currnode = head;
	while (currnode != NULL) {
		printSizes(currnode->prog);
		currnode = currnode->next;
	}
	prog_list_free(&head);
	printTotal(total);
	closedir(dp);
	return 0;

}



