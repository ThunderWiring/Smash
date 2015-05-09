#ifndef _SIGS_H
#define _SIGS_H


#define _POSIX_SOURCE

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "jobsArray.h"
typedef void (*sig_handler)();
void setSignal(int signal, sig_handler customSigHandler);
void ctrl_z();
void ctrl_c();

#endif

