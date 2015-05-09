/*
 * commands.c - SUMMARY:
 * This source file implements the functions that execute the commands which-
 * entered by the user.
 *
 * Supports the following build-in commands:
 * cd, pwd, mkdir, jobs, showpid, fg, bg, quit
 */
/******************************************************************************/
#include "commands.h"


/******************************************************************************/
/*DEFINE constants to be used:*/
#define NO_ARGS 0
#define CD_SUCCESS 0
#define MKDIR_FAIL -1
#define MAX_LEGAL_SIGNUM 31
#define ILLEGAL_SIGNAL -1
#define ILLEGAL_JOB_NUMBER -1
#define SENDING_SIGNAL_FAIL -1
#define NO_SUSPENDED -1
#define SUSPENDED 1
#define ACTIVATED 0
/******************************************************************************/
extern char *stpcpy (char *__dest, const char *__src);
//aux functions declaration:
static void aux_run_fg(Job jobs[], int jobIndex);
static int aux_get_last_bg_process(Job jobs[]);
static int aux_get_last_suspended(Job jobs[]);
static void aux_resume_suspended(Job jobs[], int suspendedIndex);
/******************************************************************************/
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
int ExeCmd(Job jobs[], char* lineSize, char* cmdString) {
	char* cmd;
	char* args[MAX_ARG];
	//char* val;
	char pwd[MAX_LINE_SIZE]; //buffer to store the currentWorkingDirectory path.
	char* delimiters = " \t\n";
	int i = 0, num_arg = 0; //pID = 0;
	bool illegal_cmd = FALSE; // illegal command
	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL) {
		return 0;
	}
	args[0] = cmd;
	for (i = 1; i < MAX_ARG; i++) {
		args[i] = strtok(NULL, delimiters);
		if (args[i] != NULL) {
			num_arg++;
		}
	}
	/*************************************************/
// Built in Commands
	/*************************************************/
	if (!strcmp(cmd, "cd")) {
		//Change the current directory to the given path in args[1].
		if (num_arg == 1) {
			int result;
			if (!strcmp(args[1], "..")) {
				result = chdir("..");

			} else {
				result = chdir(args[1]);
				if (result != CD_SUCCESS) {
					printf("smash error: > \"%s\" - path not found", args[1]);
				}
			}
		} else {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) {
		//Printing the name of the current working directory.
		if (num_arg == NO_ARGS) {
			if (getcwd(pwd, MAX_LINE_SIZE)) {
				printf("%s\n", pwd);
			} else if (errno == EACCES){
				perror("Cannot access directory");
			}
		} else {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "mkdir")) {
		//Creates a new directory in cwd with the given name.
		int result;
		if (num_arg == 1) {
			result = mkdir(args[1], 0777);
			if (result == MKDIR_FAIL) {
				if (errno == EEXIST) {
					printf("smash error: > \"%s\" – directory already exists",
							args[1]);
				} else {
					printf("smash error: > \"%s\" – cannot create directory",
							args[1]);
				}
			}
		} else {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) {
		//Prints all the processes that run in the background
		if (num_arg == NO_ARGS) {
			cleanDoneJobs(jobs);
			printJobs(jobs);
		} else {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill")) {
		//"kill -<signum>" <job>
		//send a signal(signum) to the job whoes is equals to 'job'
		int sig_number = ILLEGAL_SIGNAL;
		int job_number = ILLEGAL_JOB_NUMBER;
		if ((num_arg >= 1)) {
			sig_number = (-1) * atoi(args[1]);
			if ((num_arg == 2)) {
				job_number = atoi(args[2]);
			}
		}
		if ((sig_number > MAX_LEGAL_SIGNUM) || (sig_number <= 0)
				|| (job_number < 0) || (job_number > MAX_JOBS) ||
				(num_arg != 2)) {
			illegal_cmd = TRUE;
		} else {
			if (num_arg == 2) {
				for (int i = 0; i < MAX_JOBS; i++) {
					if (((jobs + i)->ID != job_number)
							|| ((jobs + i)->done == true)) {
						continue;
					}
					int res = kill(((jobs + i)->pID), sig_number);
					if (res == SENDING_SIGNAL_FAIL) {
						printf("smash error:> kill %d - cannot send signal\n",
								job_number);
						break;
					} else {
						printf("smash error: > kill %d – job does not exist\n",
								job_number);
					}
				}
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) {
		//print the pid of the smash itself
		if (num_arg == NO_ARGS) {
			printf("smash pid is %d\n", (int) getpid());
		} else {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) {
		cleanDoneJobs(jobs);
		if ((jobs == NULL) || (getSize(jobs) == 0)) {
			printf("smash error: > no background processes\n");
			return 0;
		}
		if (num_arg == NO_ARGS) {
			aux_run_fg(jobs, aux_get_last_bg_process(jobs));
		} else if (num_arg == 1) {
			int id = atoi(args[1]);
			for (int i = 0; i < MAX_JOBS; i++) {
				if (((jobs + i)->ID != id) || ((jobs + i)->done == true)) {
					continue;
				} else {
					aux_run_fg(jobs, i);
					break;
				}
				printf("No such process exists\n");
			}
		} else {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg")) {
		//run a process in the background.
		cleanDoneJobs(jobs);
		if (num_arg == NO_ARGS) {
			int lastSuspended = aux_get_last_suspended(jobs);
			if (lastSuspended == NO_SUSPENDED) {
				printf("There are no suspended processes to run\n");
				return 0;
			}
			aux_resume_suspended(jobs, lastSuspended);
		} else if (num_arg == 1) {
			int givenJobID = atoi(args[1]);
			int jobIndex = -1;
			for (int i = 0; i < MAX_JOBS; i++) {
				if (((jobs + i)->done == true)
						|| ((jobs + i)->ID != givenJobID)) {
					continue;
				}
				jobIndex = i;
				break;
			}
			if (jobIndex == -1) {
				perror("No job exists with this ID");
			} else if ((jobs + jobIndex)->suspended == ACTIVATED) {
				perror("Process already in the background");
			} else {
				aux_resume_suspended(jobs, jobIndex);
			}
			return 0;
		} else {
			illegal_cmd = TRUE;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit")) {
		//exit smash program
		if (num_arg == NO_ARGS) {
			free(L_Fg_Cmd);
			exit(0);
		} else if (num_arg == 1 && !strcmp("kill", args[1])) {
			cleanDoneJobs(jobs);
			//iterate over the jobs in the background:
			for (int i = 0; i < MAX_JOBS; i++) {
				if ((jobs + i)->done == true) {
					continue;
				}
				if (waitpid((jobs + i)->pID, NULL, WNOHANG) == ACTIVATED) {
					printf("[%d] %s - sending SIGTERM... ", (jobs + i)->ID,
							(jobs + i)->processName);
					if (kill((jobs + i)->pID, SIGTERM) < 0) {
						perror("cannot send SIGTERM signal");
					}
					//wait 5 seconds
					for (i = 0; i < 5; i++) {
						if (waitpid((jobs + i)->pID, NULL, WNOHANG) == ACTIVATED) {
							sleep(1);
						} else {
							break;
						}
					}
					//send sigkill if not terminated already:
					if (waitpid((jobs + i)->pID, NULL, WNOHANG) == ACTIVATED) {
						printf("(%d sec passed) Sending SIGKILL... ", 5);
						if (kill((jobs + i)->pID, SIGKILL) < 0) {
							perror("\nCannot send SIGKILL signal");
						}
					}

					while (1) {
						if (waitpid((jobs + i)->pID, NULL, WNOHANG) != 0) {
							break;
						}
					}
					printf("Done.\n");
				}
			}
			free(L_Fg_Cmd);
			exit(0);
		} else {
			illegal_cmd = TRUE;
		}
	}

	/*************************************************/
	else // external command
	{
		ExeExternal(args, cmdString);
		return 0;
	}
	if (illegal_cmd == TRUE) {
		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
	return 0;
}
//*****************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//*****************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString) {
	int pID;
	switch (pID = fork()) {
	case -1:
		// Add your code here (error)
		perror("failed to create a process");
		exit(1);
		break;
	case 0:
		// Child Process
		setpgid (0, 0);
		execvp(args[0], args);
		perror("process execution failed");
		exit(1);
		break;
	default:
		strcpy(L_Fg_Cmd , cmdString);
		GPid = pID;
		waitpid(pID,NULL,WUNTRACED);
		return;
	}
}
////**************************************************************************************
//// function name: ExeComp
//// Description: executes complicated command
//// Parameters: command string
//// Returns: 0- if complicated -1- if not
////**************************************************************************************
int ExeComp(char* lineSize) {
	int pID;
	char *args[MAX_ARG];
	if ((strstr(lineSize, "|")) || (strstr(lineSize, "<"))
			|| (strstr(lineSize, ">")) || (strstr(lineSize, "*"))
			|| (strstr(lineSize, "?")) || (strstr(lineSize, ">>"))
			|| (strstr(lineSize, "|&"))) {

		args[0] = "csh";
		args[3] = NULL;
		args[2] = lineSize;
		args[1] = "-fc";
		switch (pID = fork()) {
		case -1:   //error
			perror("Process creation failed");
			exit(1);
			break;
		case 0:    //child
//			setpgrp();
			setpgid (0, 0);
			execvp(args[0], args);
			perror("process execution error");
			exit(1);
			break;
		default:   //parent
			strcpy(L_Fg_Cmd, lineSize);
			GPid = pID;
			waitpid(pID, NULL, WUNTRACED);
			break;
		}
		return 0;
	}
	return -1;
}
////**************************************************************************************
//// function name: BgCmd
//// Description: if command is in background, insert the command to jobs
//// Parameters: command string, pointer to jobs
//// Returns: 0- BG command -1- if not
////**************************************************************************************
int BgCmd(char* lineSize, Job jobs[]) {
	int pID;
	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize) - 2] == '&') {
		lineSize[strlen(lineSize) - 2] = '\0';
		char processName[MAX_LINE_SIZE];
		strcpy(processName, lineSize);
		Command = strtok(lineSize, delimiters);
		if (Command == NULL) {
			return 0;
		}
		args[0] = Command;
		int i;
		for (i = 1; i < MAX_ARG; i++) {
			if (!(args[i] = strtok(NULL, delimiters))) {
				break;
			}
		}
		switch (pID = fork()) {
		case -1:
			perror("failed to create process");
			exit(1);
			break;
		case 0:
//			setpgrp();
			setpgid (0, 0);
			execvp(args[0], args);
			perror("failed to execute process");
			exit(1);
			break;
		default:
			insertJob(jobs, processName, pID, ACTIVATED);
		}

	}
	return -1;
}
/******************************************************************************/
//runs a given process in the foreground
static void aux_run_fg(Job jobs[], int jobIndex) {
	stpcpy(L_Fg_Cmd, (jobs + jobIndex)->processName);
	GPid = (jobs + jobIndex)->pID;
	int suspended = (jobs + jobIndex)->suspended;
//delete process from jobs array:
	for (int i = 0; i < MAX_JOBS; i++) {
		if ((jobs + i)->pID == GPid) {
			(jobs + i)->done = true;
		}
	}
	printf("%s\n", L_Fg_Cmd);
	if (suspended == 1) {
		if ((kill(GPid, SIGCONT)) < 0) {
			printf("smash error: > can't send sigcont signal\n");
		} else {
			printf("smash > signal SIGCONT was sent to pid %d\n", GPid);
		}
	}
	if (waitpid(GPid, NULL, WUNTRACED) < 0) {
		perror("Cannot execute process");
	}
}
/******************************************************************************/
/*returns the index(in jobs) that had the process with the largest ID,
 means the last process which got last into the array*/
static int aux_get_last_bg_process(Job jobs[]) {
	int lastJobIndex = 0;
	int index = 0;
	for (int i = 0; i < MAX_JOBS; i++) {
		if (((jobs + i)->done == true)) {
			continue;
		} else if ((jobs + i)->ID >= lastJobIndex) {
			lastJobIndex = (jobs + i)->ID;
			index = i;
		}
	}
	return index;
}
/******************************************************************************/
/*returns the index(in jobs[]) of the recently suspended process
 * return (NO_SUSPENDED = -1) if there is no suspended processes.*/
static int aux_get_last_suspended(Job jobs[]) {
	if (jobs == NULL) {
		return NO_SUSPENDED;
	}
	int index = NO_SUSPENDED;
	int maxId = -1;
	int histograma[MAX_JOBS];
//initialize histogram:
	for (int i = 0; i < MAX_JOBS; i++) {
		histograma[i] = -1;
	}

	for (int i = 0; i < MAX_JOBS; i++) {
		if (((jobs + i)->done == true) || ((jobs + i)->suspended) != SUSPENDED) {
			continue;
		}
		histograma[i] = (jobs + i)->ID;
	}

	for (int i = 0; i < MAX_JOBS; i++) {
		if (histograma[i] >= maxId) {
			maxId = histograma[i];
			index = i;
		}
	}
	return index;
}
/******************************************************************************/
/*resumes the run of the last suspendeds process whos index in passed to the-
 * function*/
static void aux_resume_suspended(Job jobs[], int suspendedIndex) {
	if (kill((jobs + suspendedIndex)->pID, SIGCONT) != 0) {
		perror("Could not continue running suspended process (SIGCONT failed)");
	} else {
		printf("smash > signal SIGCONT was sent to pid %d",
				(jobs + suspendedIndex)->pID);
	}
	(jobs + suspendedIndex)->suspended = ACTIVATED;
}

