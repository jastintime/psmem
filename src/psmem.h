#define NAME_SIZE 512

struct program {
	double private_mem;
	double shared_mem;
	char name[NAME_SIZE];
	int count;
};

void calculateSize(double);
void printSizes(struct program);
void printHeader(void);
void printTotal(double);

int read_proc(struct program*, const char *pid_dir);
int getCmdName(char* cmdName, const char* pid_dir, int isParent);

int all_digits(const char *str);

struct node {
	struct program prog;
	struct node *next;
};

int prog_list_add(struct node ** head, struct program prog);
void prog_list_free(struct node **head);
