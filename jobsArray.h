/*
 * jobsArray.h
 *
 *  Created on: Apr 19, 2015
 *      Author: bassam
 */

#ifndef JOBSARRAY_H_
#define JOBSARRAY_H_

#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>


#define MAX_JOBS 10
/*Enumeration for discrimination binary states*/
typedef enum {true, false} Bool;
/*****************************************************************************/
/*A struct that describes the contents of a single process
 * @param "processName" : Name of process
 * @param "ID: serial number of process  from 1 to 10.
 * @param "pID": Process ID, as given by OS
 * @param "suspended":Is process suspended(i.e stopped)
 * @param "done": indicates whether the job is done, if 'true' we can overwrite-
 * 				 this cell of the array, if 'false' means that the job is still-
 * 				 in the background and should not be overwritten.
 * @param "creationTime": When the process got created.
 */
typedef struct job_t {
	char* processName;
	int ID;
	int pID;
	int suspended;
	Bool done;
	time_t creationTime;
} Job;
/*****************************************************************************/
//API methods:
/*initializes all the elements in the array by assigning 'done = true'*/
void init(Job* jobs);
/*prints the jobs of the array*/
 void printJobs(Job* jobs);
 /*removes the done jobs from the array of jobs*/
 void cleanDoneJobs(Job* jobs);
 /*return how many undone jobs the array has*/
 int getSize(Job* jobs);
 /*insert a job to the array*/
 void insertJob(Job jobs[10], char* processName, int pID, int suspended);

#endif /* JOBSARRAY_H_ */
