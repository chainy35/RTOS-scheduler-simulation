//simulate the EDF(Earliest Deadline First) schedule policy 
//for tasks executed in Real Time System
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>//for INT_MAX

//the default value of number of tasks
#define JOB_NUM 2

//get gcd of two number
int gcd(int x, int y){
	if (y == 0) return x;
	return gcd(y, x%y);
}

//caculate lcm to get hyperperiod of tasks
int lcm(int x, int y) {
	return (x*y/gcd(x,y));
}

//function to get hyperperiod of given tasks
int hyperperiod(int **jobs, int jobs_num) {
	int lcm_val = jobs[0][0];
	for (int i =1;i<jobs_num;i++) {
		lcm_val = lcm(lcm_val, jobs[i][0]);
	}
	return lcm_val;
}

//get EDF priority task
int edf_priority_task(int **jobs, int jobs_num){
	int earliest = INT_MAX, edf_task, all_done = 1;
	
	for (int i =0;i < jobs_num;i++) {
		if(jobs[i][2] < earliest && jobs[i][3] != 0) {
			earliest = jobs[i][2];
			edf_task = i+1;// pluse 1 since task number start from 1
		}
		if (jobs[i][3] != 0) {
			all_done = 0;
		}
		
	}
	return all_done == 1? 0 : edf_task;
}

void main() {
	FILE *fp, *fp1;
	int **jobs = (int**)malloc(JOB_NUM * sizeof(int*));
	
	//open the input file
    fp = fopen("input.txt","r");
    if (!fp){
        printf("\nOpen file failed!\n");
        exit(1);
    }

	//record number of jobs
	int jobs_num = 0;
	//read the input.txt file until reach end of file
	while (!feof(fp)) {	
		//when number of tasks is larger than default value JOB_NUM, resize the jobs pointer
		if (jobs_num > JOB_NUM) {
			
			int **new_jobs = (int**)realloc(jobs,(jobs_num+1) * sizeof(int*));
			jobs = new_jobs;
		}
		
		/* use integer array jobs[i][4] to store job parameters
		 * jobs[i][0] represents period  
		 * jobs[i][1] represents job execution time
		 * jobs[i][2] represents deadline of job
		 * jobs[i][3] represents time needed to complete the job  
		 */		
		*(jobs+jobs_num) = (int *)malloc(4*sizeof(int));
		for (int j=0;j<2;j++){
			// read in the task parameters from input.txt
			fscanf(fp,"%d",&(jobs[jobs_num][j]));		
		}
		
		jobs_num++;
	
	}
	jobs_num--;//minus the extra add 1 in while loop
	
	int hp = hyperperiod(jobs, jobs_num);//calculate hyperperiod
	
	int sched_jobs[hp] ;//store the schedule result of task in hyperperiod
	
	
	//initialzie the task parameters
	//assume every tasks start at T=0
	for (int i=0;i < jobs_num;i++) {
		//assign the next task release time
		jobs[i][2] = jobs[i][0];
		
		//assign the left needed execuation time to complete the task
		jobs[i][3] = jobs[i][1];
	}
	
	//go through the hyperperiod from time=0 to determine
	//which task to schedule in each time unit
	int failure = 0;//used to report failed
	for (int i=0;i < hp;i++) {
		
		for (int j = 0;j < jobs_num;j++) {
			//update the deadline of task
			if (i == jobs[j][2]) {
				//set the new deadline of task based on its period
				jobs[j][2] += jobs[j][0];
				//check if any task not been completed before deadline
				//if yes, break loop and report Failed
				if (jobs[j][3] != 0) {
					failure = 1;
					break;
				} else {
					jobs[j][3] = jobs[j][1];
				}
			}
			
		
		}
		if (failure == 1) break;
		
		//get the EDF priority of each task to assign task to be 
		//execuated for the current time slot i		
		sched_jobs[i] = edf_priority_task(jobs, jobs_num);
		//update the left time needed to complete the task
		if (sched_jobs[i] != 0) jobs[sched_jobs[i]-1][3]--;
	}

	//open output.txt file to write result
	fp1 = fopen("output.txt","w");
    if (!fp1){
        printf("\nOpen file failed!\n");
        exit(1);
    }
	
	//print the result to Stdout and output.txt file
	if (failure == 1) {
		printf("Fail\n");
		fprintf(fp1,"Fail");
	} else {
		printf("Succeed\n");
		printf("%d\n",hp);
		fprintf(fp1,"Succeed\n");
		fprintf(fp1,"%d\n",hp);
		//printf("\nschedule result:\n");
		for (int x=0;x<hp;x++) {
			printf("%d ",sched_jobs[x]);
			fprintf(fp1,"%d ",sched_jobs[x]);
		}
		printf("\n");
	}
	

	free (jobs);
	fclose(fp);
    fclose(fp1);

}
