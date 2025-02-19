typedef struct {
	double private_mem;
	double shared_mem;
	char name[512];
	int count;

} program;

void calculateSize(double);
void printSizes(program);
void printHeader(void);
void printTotal(double);

int read_proc(program*, const char *pid_dir);

int all_digits(const char *str);
