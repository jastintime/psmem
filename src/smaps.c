#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "psmem.h"


int read_proc(struct program* prog, const char *pid_dir) {
	char line[BUFSIZ];
	/* pid_dir = /proc/3152 
	 * pid_max 32 bit is 32768 
	 * sp is smaps_rollup */
	FILE *sp;
	double private = 0;
	double private_huge = 0;
	/*double shared_huge = 0;*/
	double pss = 0;
	double shared = 0;
	int have_pss = 0;
	if (chdir(pid_dir) < 0) {
		return -1;
	}
	/* need to deal with the case that we have a really long
	 * absolute directory, perhaps change to just cd'ing into
	 * /proc and doing all our magic there */
	sp = fopen("smaps_rollup", "r");
	if (!sp) {
		return -1;
	}
	while (fgets(line, sizeof(line), sp)) {
		char key[32];
		double value;
		/* read key until : ignoring :, then get our number*/
		int ret = sscanf(line, "%31[^:]: %lf", key, &value);
		if (ret == 2) {
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
			/*	shared_huge += value;*/
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
	FILE *cp;
	char cmdline[BUFSIZ];
	char path[PATH_SIZE];
	memset(&path, 0, sizeof(path));
	if (chdir(pid_dir) < 0) {
		return -1;
	}
	cp = fopen("cmdline", "r");
	if (!cp) {
		return -1;
	}
	if (!fgets(cmdline, sizeof(cmdline), cp)) {
		fclose(cp);
		return -1;
	}
	/*missing while cmdline[-1] thing unsure of what it does */
	if (fclose(cp) == EOF) {
		return -1;
	}
	if (readlink("exe", path, sizeof(path)) == -1) {
		return -1;
	}
	/*missing check for (deleted) and (updated) */
	if (snprintf(cmdName, NAME_SIZE, "%s", basename(path)) < 0) {
		return -1;
	}
	return 0;
}

int getCmdName(char* cmdName, const char* pid_dir) {
	char cmdline[BUFSIZ];
	FILE *cp,*pp;
	char path[PATH_SIZE];
	char exe[PATH_SIZE];
	char proc_status[BUFSIZ];
	char cmd[BUFSIZ];
	char line[BUFSIZ];
	char p_exe[PATH_SIZE];
	int ppid = 0;
	if (chdir(pid_dir) < 0) {
		return -1;
	}
	memset(&p_exe, 0, sizeof(p_exe)); 
	memset(&path, 0, sizeof(path)); 
	cp = fopen("cmdline", "r");
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
	if (readlink("exe", path, sizeof(path)) == -1) {
		return -1;
	}

	/*missing check for (deleted) and (updated) */

	if (snprintf(exe,sizeof(exe), "%s", basename(path)) < 0) {
		return -1;
	}
	pp = fopen("status", "r");
	if (!pp) {
		return -1;
	}
	if (!fgets(proc_status, sizeof(proc_status), pp)) {
		fclose(pp);
		return -1;
	}
	if (sscanf(proc_status, "%*31[^:]: %s",cmd) == EOF) {
		fclose(pp);
		return -1;
	}
	if (strncmp(cmd,exe,strlen(cmd)) == 0) {
		if (snprintf(cmd, sizeof(cmd), "%s", exe) < 0) {
			fclose(pp);
			return -1;
		}
	} else {
		while(fgets(line, sizeof(line), pp)) {
			char key[32];
			int value;
			int ret = sscanf(line, "%31[^:]: %d", key, &value);
			if (ret == 2 && strcmp(key, "PPid") == 0) { 
				ppid = value;
				break;
			}
		}
		if (ppid) {
			int ret;
			char newPath[PATH_SIZE];
			if (snprintf(newPath,sizeof(newPath), "/proc/%d", ppid) < 0) {
				fclose(pp);
				return -1;
			}
			ret = getParentName(p_exe, newPath);
			if (ret == 0 && strcmp(exe,p_exe) == 0) {
				if (snprintf(cmd, sizeof(cmd), "%s", exe) < 0) {
					fclose(pp);
					return -1;
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
