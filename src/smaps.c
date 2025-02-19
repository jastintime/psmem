#include <stdio.h>
#include <string.h>
#include "psmem.h"

/*
 * return 0 on success,
 * errno setting error -1
 * -2 means unable to create smaps_rollup string
 */
int read_proc(program* prog, const char *pid_dir) {
/*	char path[16384]; */
	char line[BUFSIZ];
	/* pid_dir = /proc/3152 
	 * pid_max 32 bit is 32768 
	 * sp is smaps_rollup, cp is cmdline */
	FILE *sp,*cp;/**pp;*/
	char smaps_rollup[512], cmdline[512];
	double private = 0;
	double private_huge = 0;
	double shared_huge = 0;
	double pss = 0;
	double shared = 0;
	int have_pss = 0;
	/* need to deal with the case that we have a really long
	 * absolute directory, perhaps change to just cd'ing into
	 * /proc and doing all our magic there
	 * TODO: get rid of this sprintf, instead we will iterate over proc/%d in main
	 * and read our files with relative links.*/
	if (sprintf(smaps_rollup, "%s/smaps_rollup", pid_dir) < 0) {
		return -2;
	}
	sp = fopen(smaps_rollup, "r");
	if (!sp) {
		return -1;
	}
	while (fgets(line, sizeof(line), sp)) {
		char key[512];
		double value;
		/* read key until : ignoring :, then get our number*/
		int ref = sscanf(line, "%31[^:]: %lf", key, &value);
		if (ref == 2) {
			if (strcmp(key, "Private_Clean") == 0) { 
				private += value;
			}
			if (strcmp(key, "Private_Hugetlb") == 0) { 
				private_huge += value;
			}
			if (strcmp(key, "Private_Dirty") == 0) { 
				private += value;
			}
			if (strcmp(key, "Shared_Clean") == 0) { 
				shared += value;
			}
			if (strcmp(key, "Shared_Dirty") == 0) { 
				shared += value;
			}
			if (strcmp(key, "Shared_Hugetlb") == 0) { 
				shared_huge += value;
			}
			if (strcmp(key, "Pss") == 0) { 
				have_pss = 1;
				pss += value + 0.5; 
			}
	}
	}
	if (have_pss) {
		shared = pss - private;
	}
	/* missing shared_huge usage*/
	private += private_huge;
	prog->shared_mem = shared;
	prog->private_mem = private;
	prog->count = 0;
	if(fclose(sp) == EOF) {
		return -1;
	}

	if (sprintf(cmdline, "%s/cmdline", pid_dir) < 0) {
		return -2;
	}
	cp = fopen(cmdline, "r");
	if (!cp) {
		return -1;
	}

	if (fgets(line, sizeof(line), cp) == NULL) {
		return -2;
	}
	if (fclose(cp) == EOF) {
		return -1;
	}
	/* limit this to name size %511s*/
	sscanf(line, "%s", prog->name);
	/* NOW WE NEED TO GET OUR PATH */
	return 0;
}
