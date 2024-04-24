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
            task_array[i].finish_time = time;
            task_array[i].start_time = time; // Set start time to be the current time value on excecution.
            i++;
        }
        else if(task_array[i].arrival_time <= time){ // If the arrival time is less than the current time, execute the process.
            printf("<time %i> process %i is running\n", time, task_array[i].pid);
            if(task_array[i].start_time == -1){
                task_array[i].start_time = time;
            }
            task_array[i].remaining_time--;
            time++;
        }
        else{ // If the arrival time is greater than the current time, idle.
            printf("<time %i> idle...\n", time);
            time++;
            idle_t++;
        }
        

    }

    printf("<time %i> all processes are finished...\n", time);

    if(time == 0) {
        return 0;
    } else {
        return (float)(time - idle_t)/time;
    }

   
}

/*
 * Shortest Remaining Time First (SRTF) Policy
 *
 * The process with the smallest amount of time remaining until completion is selected to execute.
 * If a new process arrives with a shorter amount of time remaining than the current executing process, the current process is preempted.
*/
float srtf_policy(task_t task_array[], u_int count) {
    
    int time = 0, i = 0, j=0, finished = 0, idle_t = 0;

    while (finished < count) {
        int shortest_index = -1;
        int shortest_time = __INT_MAX__;

        // Find the process with the shortest remaining time
        for (j = 0; j < count; j++) {
            if (task_array[j].arrival_time <= time && task_array[j].remaining_time < shortest_time && task_array[j].remaining_time > 0) {
                shortest_index = j;
                shortest_time = task_array[j].remaining_time;
            }
        }

        // If no process is available, increment time and count wasted time
        if (shortest_index == -1) {
            printf("<time %d> No process available\n", time);
            time++;
            idle_t++;
            continue;
        }

        // Execute the process with the shortest remaining time
        task_t *current_process = &task_array[shortest_index];
        printf("<time %d> Process %d is running\n", time, current_process->pid);
        if (current_process->start_time == -1) {
            current_process->start_time = time;
        }
        current_process->remaining_time--;
        time++;

        // If the process has finished, update finish time and count finished processes
        if (current_process->remaining_time == 0) {
            current_process->finish_time = time;
            printf("<time %d> Process %d is finished\n", time, current_process->pid);
            finished++;
        }
    }

    printf("<time %d> All processes finished\n", time);
    return (time != 0) ? (float)(time - idle_t) / (float)time : 0;
}
    
    


    

/*
 * Round Robin (RR) Policy
 *
 * Each process is assigned a fixed time in cyclic order.
 * If a process is not finished in the time quantum, it is moved to the end of the queue.
*/
float rr_policy(task_t task_array[], u_int count, u_int quantum) {
    int time = 0, i = 0, j = 0, finished = 0, idle_t = 0;
    int process_queue[count]; // Queue to store indices of processes
    int front = 0, rear = -1;

    while (finished < count) {
        // Adding processes to the queue whose arrival time is less than or equal to current time
        while (i < count && task_array[i].arrival_time <= time) {
            process_queue[++rear] = i;
            i++;
        }

        // Checking if queue is not empty
        if (front <= rear) {
            int current_process_index = process_queue[front];
            task_t *current_process = &task_array[current_process_index];
            
            // If remaining time of current process is 0, process is finished
            if (current_process->remaining_time == 0) {
                printf("<time %d> process %d is finished...\n", time, current_process->pid);
                current_process->finish_time = time;
                front++; // Remove finished process from queue
                finished++;
                j = 0;
            } else {
                // If time quantum is not reached, run the process
                if (j < quantum) {
                    printf("<time %d> process %d is running\n", time, current_process->pid);
                    if (current_process->start_time == -1) {
                        current_process->start_time = time;
                    }
                    current_process->remaining_time--;
                    time++;
                    j++;
                } else {
                    // If time quantum is reached, move the process to the end of the queue
                    j = 0;
                    int k = 0;
                    int temp = process_queue[front];
                    for (k = front; k < rear; k++) {
                        process_queue[k] = process_queue[k + 1];
                    }
                    process_queue[rear] = temp;
                }
            }
        } else {
            // If no process is available, increment time and count wasted time
            printf("<time %d> no process is currently running\n", time);
            time++;
            idle_t++;
        }
    }

    printf("<time %d> All processes finished\n", time);
    return (time != 0) ? (float)(time - idle_t) / (float)time : 0;

}

/*
* Calculate the average waiting time, response time, and turnaround time for the processes
*
* wt[i] =  ( at[i – 1] + bt[i – 1] + wt[i – 1] ) – at[i]
*
*   where 
*       at[i] = arrival time of current process
*       wt[i] = waiting time of current process =  task_t[count] 
*       at[i-1] = arrival time of previous process 
*       bt[i-1] = burst time of previous process 
*       wt[i-1] = waiting time of previous process 
*       at[i] = arrival time of current process
*
* Average Waiting Time = (sum of all waiting time)/(Number of processes) 


*/
void stats(task_t task_list[], int size, float cpu_usage) {
    float wt = 0, rt = 0, trnt = 0;
    int i;
    for (i = 1; i < size; i++) {
        
        wt += (float)((task_list[i-1].arrival_time + task_list[i-1].burst_time + wt) - task_list[i].arrival_time);
        rt += (float)(task_list[i].start_time - task_list[i].arrival_time);
        trnt += (float)(task_list[i].finish_time - task_list[i].arrival_time);
     

        
    }
    wt = (size != 0) ? wt / (float)size : 0;
    rt = (size != 0) ? rt / (float)size : 0;
    trnt = (size != 0) ? (trnt / (float)size) : 0;


    printf("==================================================================\n");
    printf("Average waiting time:    %.2f\n", wt);
    printf("Average response time:   %.2f\n", rt);
    printf("Average turnaround time: %.2f\n", trnt);
    printf("Overall CPU usage:       %5.2f%%\n", cpu_usage * 100);
    printf("==================================================================\n");

}


/* 
 * Main function for executing the actual scheduling.
*/
int main( int argc, char *argv[] )  {
    char *file_name; /* file name from the commandline */
    FILE *fp; /* file descriptor */
    task_t task_array[MAX_TASK_NUM];

    int error_code;
    u_int i;
    u_int count;
    float cpu_usage;

    if (argc == 1 || argc > 4) {
        printf("Usage: command file_name [FCFS|RR|srtf] [time_quantum]\n\n");
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
        cpu_usage = rr_policy(task_array, count, atoi(argv[3]));
        printf("RR Policy\n");
    } else if (strcmp(argv[2], "SRFT") == 0) {
        cpu_usage = srtf_policy(task_array, count);
        
    } else {
        printf("Invalid scheduling policy: %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    stats(task_array, count, cpu_usage);

    fclose(fp);
    return EXIT_SUCCESS;
}
