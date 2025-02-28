#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "psmem.h"

/*
 * return 0 on success,
 * errno setting error -1
 * -2 means unable to create smaps_rollup string
 */
int read_proc(struct program* prog, const char *pid_dir) {
/*	char path[16384]; */
	char line[BUFSIZ];
	/* pid_dir = /proc/3152 
	 * pid_max 32 bit is 32768 
	 * sp is smaps_rollup, cp is cmdline */
	FILE *sp;
	char smaps_rollup[512];
	double private = 0;
	double private_huge = 0;
	double shared_huge = 0;
	double pss = 0;
	double shared = 0;
	int have_pss = 0;
	char cmdName[512];
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
	/* limit this to name size %511s*/
	getCmdName(cmdName, pid_dir,0);
	strcpy(prog->name, cmdName);
	/* NOW WE NEED TO GET OUR PATH */
	return 0;
}

int getCmdName(char* cmdName, const char* pid_dir, int isParent) {
	/*TODO error handling misssing, removing all these nasty variables */
	char cmdline_filename[512];
	FILE *cp,*pp;
	char cmdline[512];
	char path_location[512];
	char path[512];
	char *exe;
	char status_filename[512];
	char proc_status[512];
	char cmd[512];
	char line[512];
	char p_exe[512];
	int ppid = 0;
	sprintf(cmdline_filename, "%s/cmdline", pid_dir);
	cp = fopen(cmdline_filename, "r");
	fgets(cmdline, sizeof(line), cp);
	/*missing while cmdline[-1] thing unsure of what it does */
	fclose(cp);

	sprintf(path_location, "%s/exe", pid_dir);
	readlink(path_location, path, sizeof(path));
	/*missing check for (deleted) and (updated) */
	exe = basename(path);
	if (isParent) {
		strcpy(cmdName, exe);
		return 0;
	}
	sprintf(status_filename, "%s/status", pid_dir);
	pp = fopen(status_filename, "r");
	fgets(proc_status, sizeof(proc_status), pp);
	sscanf(proc_status, "%*31[^:]: %s",cmd);
	while(fgets(line, sizeof(line), pp)) {
		char key[512];
		int value;
		sscanf(line, "%31[^:]: %d", key, &value);;
		if (strcmp(key, "PPid") == 0) { 
			ppid = value;
			break;
		}
	}
	if (ppid) {
		memset(&p_exe, 0, sizeof(p_exe)); 
		getCmdName(p_exe, pid_dir, 1);
	}
	if (strcmp(exe, p_exe)) {
		strcpy(cmd, exe);
	}
	fclose(pp);
	strcpy(cmdName, cmd);
	return 0;
	










	

	






	


}
