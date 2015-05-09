/*
 * jobsArray.c
 *
 *  Created on: Apr 19, 2015
 *      Author: bassam
 */

//SUMMARY: simplement the API of jobsArray.h

#include <stdio.h>
#include <stdlib.h>
#include "jobsArray.h"

#define INVALID -1
/*****************************************************************************/
/*static declarations*/
static int aux_getId(Job jobs[MAX_JOBS]);
static void aux_sortJobs(Job jobs[MAX_JOBS]);
/*****************************************************************************/
void init(Job* jobs) {
	for(int i = 0; i < MAX_JOBS; i++) {
		(jobs + i)->done = true;
		(jobs + i)->ID = 1;
	}
}
/*****************************************************************************/
 void printJobs(Job* jobs) {
	 aux_sortJobs(jobs);
	 for(int i = 0; i < MAX_JOBS; i++) {
		 if((jobs + i)->done == false) {
			 printf("[%d] %s : %d %d secs",(jobs + i)->ID, (jobs + i)->processName,
					 (jobs + i)->pID, (int) difftime(time(NULL), (jobs + i)->creationTime));
			 printf("\n");
		 }
	 }
 }
 /*****************************************************************************/
 void cleanDoneJobs(Job* jobs) {
	 if(jobs == NULL) {
		 return;
	 }
	int id, pid;
	for(int i = 0; i < MAX_JOBS; i++) {
		if((jobs + i)->done == true) {
			continue;
		}
		id = (jobs + i)->ID;
		pid = (jobs + i)->pID;
		if (waitpid((pid), NULL, WNOHANG) > 0) {
			(jobs + i)->done = true; //Delete the process from the array
		}
	}
 }
 /*****************************************************************************/
 int getSize(Job* jobs) {
	 if(jobs == NULL) {
		 return 0;
	 }
	 int counter = 0;
	 for(int i = 0; i < MAX_JOBS; i++) {
		 if((jobs + i)->done == false) {
			 counter++;
		 }
	 }
	 return counter;
 }
 /*****************************************************************************/
 void insertJob(Job jobs[10], char* processName, int pID, int suspended) {
	 int id = aux_getId(jobs);
	 for(int i = 0; i < MAX_JOBS; i++) {
		 if((jobs + i)->done == true) {
			 (jobs + i)->ID = id;
			 (jobs + i)->creationTime = time(NULL);
			 (jobs + i)->done = false;
			 (jobs + i)->pID = pID;
			 (jobs + i)->processName = processName;
			 (jobs + i)->suspended = suspended;
			 return;
		 }
	 }
 }
 /*****************************************************************************/
 // return an ID number for a newly inserted job to the array
 static int aux_getId(Job jobs[MAX_JOBS]) {
	 if(getSize(jobs) == 0) {
		 return 1;
	 }
	 int maxId = INVALID;
	 for(int i = 0; i < MAX_JOBS; i++) {
		 if(((jobs + i)->done == false) && ((jobs + i)->ID > maxId)) {
			 maxId = (jobs + i)->ID;
		 }
	 }
	 return maxId + 1;
 }
 /*****************************************************************************/
 static void aux_sortJobs(Job jobs[MAX_JOBS]) {
	 for (int i = 0; i < MAX_JOBS; ++i) {
		   for (int j = 0; j < MAX_JOBS - 1 - i; ++j ) {
				if ((jobs + j)->ID > (jobs + j + 1)->ID) {
					 Job temp = jobs[j+1];
					 jobs[j+1] = jobs[j];
					 jobs[j] = temp;
				}
		   }
	  }
 }





