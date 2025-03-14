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


int main(int argc, char **argv) {
	DIR* dp = opendir(PROC_DIRECTORY);
	char path[PATH_SIZE];
	char selfpath[PATH_SIZE];
	double total = 0.0;
	struct program curr_program;
	struct dirent *curr;
	struct node *head,*currnode;
	head = NULL;
	memset(&curr_program, 0, sizeof(curr_program)); 
	if (!dp) {
		return 1;
	}
	(void) argv;
	if (argc > 1) {
		printf("psmem version 1.0\n");
		printf("Usage: psmem [NO OPTIONS]\n\n");
		printf("report memory usage of running programs.\n");
		printf("running psmem with superuser permissions will provide more accurate results.\n");
		printf("\nTry `man psmem` for more information.\n");
		printf("Report bugs to <https://github.com/jastintime/psmem/issues>\n");
		return 0;
	}
	if (snprintf(selfpath,sizeof(selfpath), "%ld", (long) getpid()) < 0) {
		return 1;
	}
	printHeader();
	while((curr = readdir(dp))) {
		if (chdir(PROC_DIRECTORY) < 0) {
			return 1;
		}
		if(curr->d_type == DT_DIR && all_digits(curr->d_name) == 0) {
			if (snprintf(path,sizeof(path), PROC_DIRECTORY"%s", curr->d_name) < 0) {
				return 1;
			}
			if (strcmp(curr->d_name, selfpath) == 0) {
				continue;
			}
			if (read_proc(&curr_program, path) != 0) {
				continue;
			}
			if (prog_list_add(&head,curr_program) < 0) {
				return 1;
			}
			total += curr_program.private_mem + curr_program.shared_mem;
		}
	}
	mergeSort(&head);
	currnode = head;
	while (currnode) {
		printSizes(currnode->prog);
		currnode = currnode->next;
	}
	prog_list_free(&head);
	printTotal(total);
	if (closedir(dp) < 0) {
		return 1;
	}
	return 0;

}



