/*
 * COMP 3500: Project 5 Scheduling
 * Tyler Teufel
 * Version 2.0  04/22/2024
 *
 *
 * How to compile using Makefile?
 * $make
 *
 * How to manually compile?
 * $gcc -c open.c
 * $gcc -c read.c
 * $gcc -c print.c
 * $gcc open.o read.o print.o scheduler.c -o scheduler
 *
 * How to run:
 * Case 1: no argument. Sample usage is printed
 * $./scheduler
 * Usage: scheduler <file_name>
 *
 * Case 2: file doesn't exist.
 * $./scheduler file1
 * File "file1" doesn't exist. Please try again...
 *
 * Case 3: Input file
 * $./scheduler task.list
 * data in task.list is printed below...
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scheduler.h"
#include "open.h"
#include "read.h"

/*


* wt[i] =  ( at[i – 1] + bt[i – 1] + wt[i – 1] ) – at[i]
*
*   where 
*       at[i] = arrival time of current process
*       wt[i] = waiting time of current process 
*       at[i-1] = arrival time of previous process 
*       bt[i-1] = burst time of previous process 
*       wt[i-1] = waiting time of previous process 
*       at[i] = arrival time of current process
*
* Average Waiting Time = (sum of all waiting time)/(Number of processes) 


*/

/* 
 * First Come First Serve (FCFS) Policy
 *
 * Processes are scheduled and executed in the order that they arrive.
 * Whichever process requests the CPU first is allocated the CPU first.
*/
float fcfs_policy(task_t task_array[], u_int count) {
    int i = 0,time = 0, idle_t = 0;
    //printf("FCFS Policy\n");
    
    while (i < count) {
        
        if(task_array[i].remaining_time == 0){
            printf("<time %i> process %i is finished...\n", time, task_array[i].pid);
            task_array[i].start_time = time; // Set start time to be the current time value on excecution.
            i++;
        }
        else if(task_array[i].arrival_time <= time){
            printf("<time %i> process %i is running\n", time, task_array[i].pid);
            task_array[i].remaining_time--;
            time++;
        }
        else{
            printf("<time %i> idle...\n", time);
            time++;
            idle_t++;
        }
        

    }

    printf("<time %i> all processes are finished\n", time);

    if(time == 0) {
        return 0;
    } else {
        return (float)(time - idle_t)/time;
    }

    return 0.0;
}

int main( int argc, char *argv[] )  {
    char *file_name; /* file name from the commandline */
    FILE *fp; /* file descriptor */
    task_t task_array[MAX_TASK_NUM];

    int error_code;
    u_int i;
    u_int count;
    float cpu_usage;

    if (argc == 1 || argc > 4) {
        printf("Usage: command file_name [FCFS|RR|SRFT] [time_quantum]\n\n");
        return 0;
    }

    if (argc < 2) {
        printf("Usage: input <file_name>\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[2], "RR") == 0) {
        if (argc == 4)
            printf("time_quantum is set to %s\n", argv[3]);  
        else printf("Please enter time_quantum for the RR policy!\n");
    }

    error_code = open_file(argv[1], &fp);
    if (error_code == 1)
        return EXIT_FAILURE;

    
    printf("Scheduling Policy: %s\n", argv[2]);

    read_file(fp, task_array, &count);

    printf("There are %d tasks loaded from \"%s\". Press enter to continue...", count, argv[1]);
    getchar();

    printf("==================================================================\n");

    if (strcmp(argv[2], "FCFS") == 0) {
        cpu_usage = fcfs_policy(task_array, count);
    } else if (strcmp(argv[2], "RR") == 0) {
        // cpu_usage = rr_policy(task_array, count, atoi(argv[3]));
        printf("RR Policy\n");
    } else if (strcmp(argv[2], "SRFT") == 0) {
        // cpu_usage = srft_policy(task_array, count);
        printf("SRFT Policy\n");
    } else {
        printf("Invalid scheduling policy: %s\n", argv[2]);
        return EXIT_FAILURE;
    }

   
    fclose(fp);
    return EXIT_SUCCESS;
}
