/*	smash.c
 *******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "commands.h"
#include "signals.h"
#include "jobsArray.h"
/*******************************************************************/
#define MAX_LINE_SIZE 80
#define MAXARGS 20
/*******************************************************************/
extern int GPid; // PID (global)
extern int Last_Bg_Pid;
extern int Susp_Bg_Pid;
extern char* L_Fg_Cmd;
extern int susp; //is the process suspended: 0- no, 1- yes
/*******************************************************************/
char lineSize[MAX_LINE_SIZE];
Job jobs[10]; //Static array of 10 structs to handle the background jobs
/*****************************************************************************/
// function name: main
// Description: main function of smash. get command from user and calls command functions
int main(int argc, char *argv[]) {
	char cmdString[MAX_LINE_SIZE];

	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	setSignal(SIGTSTP,&ctrl_z);
	setSignal(SIGINT,&ctrl_c);

	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here
	/* add your code here */

	/************************************/

	/************************************/
	// Init globals 
	GPid = -1;
	Last_Bg_Pid = -1;
	Susp_Bg_Pid = -1;
	susp = 0;

	L_Fg_Cmd = (char*) malloc(sizeof(char) * (MAX_LINE_SIZE + 1));
	if (L_Fg_Cmd == NULL)
		exit(-1);
	L_Fg_Cmd[0] = '\0';

	while (1) {
		printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);
		cmdString[strlen(lineSize) - 1] = '\0';
		// perform a complicated Command
		if (!ExeComp(lineSize))
			continue;
		// background command
		if (!BgCmd(lineSize, jobs))
			continue;
		// built in commands
		ExeCmd(jobs, lineSize, cmdString);

		/* initialize for next line read*/
		lineSize[0] = '\0';
		cmdString[0] = '\0';
	}
	return 0;
}

