#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "psmem.h"


int read_proc(struct program* prog, const char *pid_dir) {
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
	if (getCmdName(prog->name, pid_dir,0) != 0) {
		return -1;
	}
	return 0;
}


int getCmdName(char* cmdName, const char* pid_dir, int isParent) {
	/*TODO error handling misssing, removing all these nasty variables */
	char cmdline_filename[512];
	FILE *cp,*pp;
	char cmdline[512];
	char path_location[512];
	char path[512];
	char exe[512];
	char status_filename[512];
	char proc_status[512];
	char cmd[512];
	char line[512];
	char p_exe[512];
	int ppid = 0;
	memset(&p_exe, 0, sizeof(p_exe)); 
	memset(&path, 0, sizeof(path)); 
	sprintf(cmdline_filename, "%s/cmdline", pid_dir);
	cp = fopen(cmdline_filename, "r");
	if (!cp) {
		return -1;
	}
	fgets(cmdline, sizeof(cmdline), cp);
	/*missing while cmdline[-1] thing unsure of what it does */
	fclose(cp);

	sprintf(path_location, "%s/exe", pid_dir);
	if (readlink(path_location, path, sizeof(path)) == -1) {
		return -1;
	}
	/*missing check for (deleted) and (updated) */
	/*exe = basename(path);*/
	sprintf(exe, "%s", basename(path));
	if (isParent) {
		/*TODO: why does this work??? sizeof(cmdName) is 8?? */
		snprintf(cmdName, sizeof(cmdName), "%s", exe);
		return 0;
	}
	sprintf(status_filename, "%s/status", pid_dir);
	pp = fopen(status_filename, "r");
	if (!pp) {
		return -1;
	}
	fgets(proc_status, sizeof(proc_status), pp);
	sscanf(proc_status, "%*31[^:]: %s",cmd);
	if (strncmp(cmd,exe,strlen(cmd)) == 0) {
		/*strcpy(cmd, exe);*/
		snprintf(cmd, sizeof(cmd), "%s", exe);
	} else {
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
			int ret;
			char newPath[512];
			sprintf(newPath, "/proc/%d", ppid);
			ret = getCmdName(p_exe, newPath, 1);
			if (ret == 0) {
				if (strcmp(exe, p_exe) == 0) {
					snprintf(cmd, sizeof(cmd), "%s", exe);
				}
			}
		}
	}
	
	fclose(pp);
	snprintf(cmdName, NAME_SIZE, "%s", cmd);
	return 0;
}
