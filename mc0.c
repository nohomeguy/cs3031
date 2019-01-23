#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>

#define TRUE 1

/**
 *	 print the statistics
 *	
 **/
int print_stats(struct rusage pre_stats, struct rusage post_stats, struct timeval pre_time, struct timeval post_time){
	float time = ((float)post_time.tv_sec - (float)pre_time.tv_sec)*1000 + 
		     ((float)post_time.tv_usec - (float)pre_time.tv_usec)/1000;
	printf("-- Statistics ---\n");
	printf("Elapsed time: %.2f milliseconds\n", time);
	printf("Page Faults: %ld\n", (post_stats.ru_majflt-pre_stats.ru_majflt));
	printf("Page Faults (reclaimed): %ld\n\n", (post_stats.ru_minflt-pre_stats.ru_minflt));
	return 0;
}

/**
 *      Prints a screen that shows the options to the users
 *      @param: no param
 *      @return: return 0 if no error, non-zero if error
**/
int screen(){
        printf("====== Mid-Day Commander, v0 =====\n");
        printf("G'day, Commander! What command would you like to run?\n");
        printf("   0. whoami  : Prints out the result of the whoamicommand\n");
        printf("   1. last    : Prints out the result of the last command\n");
        printf("   2. ls      : Prints out the result of a listing on user-specified path\n");
        printf("Option?: ");
        return 0;
}

/**
 *      execute the command input
 *      @param int rc: a parameter to record if the process is a child process
 *      @param const char* command: the command to be execute
 *      @param const char* argv[]: the argument
 *      @return: 0 if no error, non-zero if error
**/
int command_executor(int rc, char *command, char *argv[]){
        if (rc == 0) { //if child
                execvp(command, argv);
        } else {
                wait(NULL);
        }
        return 0;
}

/**
 *	execute a command passed in by a string, no arguments
 *	@param char* command: the name of the command to be execute
 *	@return: 0 if no error, non-zero if error
**/
int exec_func1(char *command){
	struct rusage pre_stats;
	struct rusage post_stats;
	struct timeval start;
	struct timeval end;
	int rc = fork();	// make a copy of the process, rc is 0 if child, pid if parent
//	char* command = "whoami";
	getrusage(RUSAGE_CHILDREN, &pre_stats);
	gettimeofday(&start, NULL);
//	printf("%f", (float)start.tv_sec*1000000 + (float)start.tv_usec);
	char* argv[] = {command, NULL}; // store the arguments to execute the command
	command_executor(rc, command, argv);
	printf("\n");
	getrusage(RUSAGE_CHILDREN, &post_stats);
	gettimeofday(&end, NULL);
//	printf("%f", (float)end.tv_sec*1000000 + (float)end.tv_usec);
	print_stats(pre_stats, post_stats, start, end);
	return 0;
}

/**
 *	remove all the null elements in a string array
 *	@param char *args[]: string array to be removed
 *	@param int arg_num: number of elements in args[]
 *	@return return number of remaining strings 
**/
int remove_null_arg(char *args[], int arg_num, char *r_args[]){
	int i, r_flag = 0; 		// loop counter and the index flag for the result array
	int null_count = 0;	// null counter for the original array
	for (i = 0; i < arg_num; i ++){
		if (strcmp(args[i], "") == 0){
			null_count ++;
		} else {
			strcpy (r_args[r_flag], args[i]);
printf("%s", r_args[r_flag]);
			r_flag ++;
		}
	}
	return arg_num - null_count;	
}

/**
 *	execute a command passed in by a string, pass the arguments in an array
 *	@param char* command: the name of the command to be execute
 *	@param char* args[]: the arguments for the command
 *	@param int arg_num: number of arguments
 *	@return: 0 if no error, non-zero if error
**/
int exec_func2(char *command, char *o_args[], int o_arg_num){
	int i;			// index for loop
	char **args = (char**) malloc (16*256);
	for (i = 0; i < 16; i ++){
		args[i] = (char*) malloc(256);
	}
	int arg_num = remove_null_arg(o_args, o_arg_num, args);
	struct rusage pre_stats;
	struct rusage post_stats;
	struct timeval start;
	struct timeval end;
	int rc = fork();	// make a copy of the process, rc is 0 if child, pid if parent
	getrusage(RUSAGE_CHILDREN, &pre_stats);
	gettimeofday(&start, NULL);	
	char* argv[arg_num + 2];// store the arguments to execute the command
	
	argv[0] = command;
	for (i = 0; i < arg_num; i++){
		argv[i+1] = args[i];
	}
	argv[arg_num + 1] = NULL;
	command_executor(rc, command, argv);
	printf("\n");
	getrusage(RUSAGE_CHILDREN, &post_stats);
	gettimeofday(&end, NULL);
	print_stats(pre_stats, post_stats, start, end);
	free(args);
	return 0;
}


int main(int argc, char *argv[]){
	while(TRUE){
		screen();
		char *opt = (char*) malloc (256);
//		printf("sizeof char* = %d", sizeof(char*));
		char *a = (char*) malloc (256);
		char *p = (char*) malloc (256);
		size_t opt_size = 256;
		ssize_t nread = 0;
		char* gline_result = fgets(opt, 256, stdin);
//		printf("optsize = %d", (int) opt_size);
//		printf("%s", opt);
		if(gline_result == NULL){pause();}
		if(strcmp(opt, "0\n") != 0 && strcmp(opt, "1\n") != 0 && strcmp(opt, "2\n") != 0){
			printf("\nOption don't exist\n\n");
		} else {
			switch(atoi(opt)){
				case 0:
					printf("\n-- Who Am I? --\n");
					exec_func1("whoami");
					printf("\n");
					break;
				case 1:
					printf("\n-- Last Logins --\n");
					exec_func1("last");
					printf("\n");
					break;
				case 2:
					printf("\n-- Directory Listing --\nArguments?: ");
					nread = getline(&a, &opt_size, stdin);
					a[(int)nread-1] = 0;
					printf("Path:? ");
					nread = getline(&p, &opt_size, stdin);
					p[(int)nread-1] = 0;
					char *args[] = {a, p};
					exec_func2("ls", args, 2);
					printf("\n");
					break;
				default:
					printf("Option don't exist\n");
			}
		}
		free(opt); free(a); free(p);
	}
	
	return 0;
}
