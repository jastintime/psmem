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
	char smaps_rollup[PATH_SIZE + sizeof("smaps_rollup")];
	double private = 0;
	double private_huge = 0;
	double shared_huge = 0;
	double pss = 0;
	double shared = 0;
	int have_pss = 0;
	/* need to deal with the case that we have a really long
	 * absolute directory, perhaps change to just cd'ing into
	 * /proc and doing all our magic there */
	if (snprintf(smaps_rollup,sizeof(smaps_rollup), "%s/smaps_rollup", pid_dir) < 0) {
		return -1;
	}
	sp = fopen(smaps_rollup, "r");
	if (!sp) {
		return -1;
	}
	while (fgets(line, sizeof(line), sp)) {
		char key[32];
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
	if (getCmdName(prog->name, pid_dir) != 0) {
		return -1;
	}
	return 0;
}

static int getParentName(char* cmdName, const char* pid_dir) {
	char cmdline_filename[PATH_SIZE + sizeof("cmdline")];
	FILE *cp;
	char cmdline[BUFSIZ];
	char path_location[PATH_SIZE + sizeof("exe")];
	char path[sizeof(path_location)];
	char exe[sizeof(path_location)];
	memset(&path, 0, sizeof(path));
	if ((snprintf(cmdline_filename, sizeof(cmdline_filename),"%s/cmdline", pid_dir) < 0)) {
		return -1;
	}
	cp = fopen(cmdline_filename, "r");
	if (!cp) {
		return -1;
	}
	if (!fgets(cmdline, sizeof(cmdline), cp)) {
		return -1;
	}
	/*missing while cmdline[-1] thing unsure of what it does */
	if (fclose(cp) == EOF) {
		return -1;
	}

	if (snprintf(path_location,sizeof(path_location), "%s/exe", pid_dir) < 0) {
		return -1;
	}
	if (readlink(path_location, path, sizeof(path)) == -1) {
		return -1;
	}
	/*missing check for (deleted) and (updated) */
	if (snprintf(exe,sizeof(exe), "%s", basename(path)) < 0) {
		return -1;
	}
	if (snprintf(cmdName, sizeof(cmdName), "%s", exe) < 0) {
		return -1;
	}
	return 0;
}

int getCmdName(char* cmdName, const char* pid_dir) {
	char cmdline_filename[PATH_SIZE + sizeof("cmdline")];
	char cmdline[BUFSIZ];
	FILE *cp,*pp;
	char path[PATH_SIZE + sizeof("exe")];
	char path_location[sizeof(path)];
	char exe[sizeof(path)];
	char status_filename[PATH_SIZE + sizeof("status")];
	char proc_status[BUFSIZ];
	char cmd[BUFSIZ];
	char line[BUFSIZ];
	char p_exe[sizeof(path)];
	int ppid = 0;
	memset(&p_exe, 0, sizeof(p_exe)); 
	memset(&path, 0, sizeof(path)); 
	if (snprintf(cmdline_filename,sizeof(cmdline_filename), "%s/cmdline", pid_dir) < 0) {
		return -1;
	}
	cp = fopen(cmdline_filename, "r");
	if (!cp) {
		return -1;
	}
	if(!fgets(cmdline, sizeof(cmdline), cp)) {
		return -1;
	}
	/*missing while cmdline[-1] thing unsure of what it does */
	if (fclose(cp) == EOF) {
		return -1;
	}

	if (snprintf(path_location,sizeof(path_location), "%s/exe", pid_dir) < 0) {
		return -1;
	}
	if (readlink(path_location, path, sizeof(path)) == -1) {
		return -1;
	}

	/*missing check for (deleted) and (updated) */

	if (snprintf(exe,sizeof(exe), "%s", basename(path)) < 0) {
		return -1;
	}
	if (snprintf(status_filename,sizeof(status_filename), "%s/status", pid_dir) < 0) {
		return -1;
	}
	pp = fopen(status_filename, "r");
	if (!pp) {
		return -1;
	}
	if (!fgets(proc_status, sizeof(proc_status), pp)) {
		return -1;
	}
	if (sscanf(proc_status, "%*31[^:]: %s",cmd) == EOF) {
		return -1;
	}
	if (strncmp(cmd,exe,strlen(cmd)) == 0) {
		if (snprintf(cmd, sizeof(cmd), "%s", exe) < 0) {
			return -1;
		}
	} else {
		while(fgets(line, sizeof(line), pp)) {
			char key[32];
			int value;
			sscanf(line, "%31[^:]: %d", key, &value);;
			if (strcmp(key, "PPid") == 0) { 
				ppid = value;
				break;
			}
		}
		if (ppid) {
			int ret;
			char newPath[PATH_SIZE];
			snprintf(newPath,sizeof(newPath), "/proc/%d", ppid);
			ret = getParentName(p_exe, newPath);
			if (ret == 0) {
				if (strcmp(exe, p_exe) == 0) {
					snprintf(cmd, sizeof(cmd), "%s", exe);
				}
			}
		}
	}
	
	if (fclose(pp) == EOF) {
		return -1;
	}
	if (snprintf(cmdName, NAME_SIZE, "%s", cmd) < 0) {
		return -1;
	}
	return 0;
}
