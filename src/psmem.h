#define NAME_SIZE 256
#define PATH_SIZE 4096

/* program stores memory usage, a name and a count.
 * read_proc fills this struct, printSizes prints this struct.
*/
struct program {
	double private_mem;
	double shared_mem;
	char name[NAME_SIZE];
	int count;
};

/* printFormatSize prints a double representing
 * memory as bytes in the format MEM.X XiB
*/
void printFormatSize(double memoryAsBytes);

/* printSizes prints a line showing all the fields of the 
 * program struct in the following format
 * PRIVATE_MEM + SHARED_MEM = TOTAL_MEM  NAME (COUNT)
*/
void printSizes(struct program);

/* printHeader prints the header in the form of
 * Private + Shared  = Ram Used  Program
*/
void printHeader(void);

/*
 * printTotal prints the total memory usage in the format
 * --------------------------
 *                  MEM.X XiB
 * ==========================
*/
void printTotal(double);

/*
 * read_proc fills a program struct with the information 
 * available at pid_dir, a string of the pid directory in the
 * /proc/INT format.
 * return 0 on success, -1 on failure.
 */
int read_proc(struct program*, const char *pid_dir);

/*
 * getCmdName writes the command name of the process in the
 * given pid directory to the cmdName string.
*/
int getCmdName(char* cmdName, const char* pid_dir);

/*
 * all_digits reports if a string is all numbers.
 * returns 0 if the given string is all numbers and 1 if 
 * it contains any character that isn't a number.
 */
int all_digits(const char *str);

/* a node of programs, used for implementing a linked list. */
struct node {
	struct program prog;
	struct node *next;
};

/*
 * prog_list_add adds a program to the linked list.
 * If there exists a node in the linked list with the same
 * name their values will be combined,otherwise it is appended.
*/ 
int prog_list_add(struct node **list, struct program);

/*
 * prog_list_free frees the linked list.
*/
void prog_list_free(struct node **list);

/*
 * mergeSort sorts the linked list by the total memory usage
 * in ascending order.
*/
void mergeSort(struct node **list);
