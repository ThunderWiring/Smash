// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"


extern int GPid;
//extern int Last_Bg_Pid;
extern int Susp_Bg_Pid;
extern int susp;
extern Job jobs[MAX_JOBS];
extern char* L_Fg_Cmd;
/******************************************************************************/
//terminates the process by sending a signal SIGINT
void ctrl_c() {
	printf("here");
	if(GPid>=0 && !waitpid(GPid,NULL,WNOHANG)) {
			if(kill(GPid,SIGINT) < 0)
				perror("could not send SIGINT signal\n");
			else {
				printf("smash > signal SIGINT was sent to pid %d\n", GPid);
				GPid = -1;
			}
		}
}
/******************************************************************************/
void ctrl_z() {
	if(GPid>=0 && !waitpid(GPid,NULL,WNOHANG)) {
		if(kill(GPid,SIGSTOP)<0)
			perror("problem sending the signal\n");
		else
			printf("smash > signal SIGTSTP was sent to pid %d\n", GPid);
		Susp_Bg_Pid = GPid;
		susp = 1;
		insertJob(jobs,L_Fg_Cmd,Susp_Bg_Pid,1);
		GPid=-1;
	}
}
/******************************************************************************/
void setSignal(int signal, sig_handler customSigHandler) {
	struct sigaction customSignal;
	sigset_t mask;
	sigfillset(&mask);
	customSignal.sa_flags = 0;
	customSignal.sa_mask = mask;
	customSignal.sa_handler = customSigHandler;
	if(sigaction(signal,&customSignal, NULL) == -1)
		perror("problem creating new signal\n");
}
/******************************************************************************/




