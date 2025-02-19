#include <stdio.h>
#include <dirent.h>
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
	double total = 0.0;
	program curr_program;
	struct dirent *curr;
	/* pull this out and early return */
	if (!dp) {
		return 1;
	}
	printHeader();
	while((curr = readdir(dp))) {
		if(curr->d_type == DT_DIR && all_digits(curr->d_name) == 0) {
			sprintf(path, "/proc/%s", curr->d_name);
			read_proc(&curr_program, path);
			printSizes(curr_program);
			total += curr_program.private_mem + curr_program.shared_mem;
		}
	}
	printTotal(total);
	closedir(dp);

	return 0;

}



