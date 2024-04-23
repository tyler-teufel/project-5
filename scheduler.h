/*
 * COMP 3500: Project 5 Scheduling
 * Tyler Teufel
 * Version 2.0  04/22/2024
 *
 *
 * scheduler.h: The header file of scheduler.c
 */
#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#define MAX_TASK_NUM 32

typedef unsigned int u_int;

typedef struct task_info {
    u_int pid;
    u_int arrival_time;
    u_int burst_time;
} task_t;

#endif