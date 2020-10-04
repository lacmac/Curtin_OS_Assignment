/**
 * @headerfile schedulerInfo.h
 * @brief Defines the structure of the SchedulerInfo and the basic functions
 * for creating and destroying said structure.
 *
 * @author Lachlan Mackenzie
 * @date 01/05/19
 */
#ifndef SCHEDULERINFO_H
#define SCHEDULERINFO_H

#include <pthread.h>
#include <stdlib.h>

//STRUCTS
/**
 * @brief This SchedulerInfo struct is used to store the variables that are
 * shared between the CPU threads executing cpu().
 *
 * @field num_tasks THe number of tasks tht have been executed so far by all CPU
 * threads.
 * @field total_waiting_time The sum of each tasks waiting time.
 * @field total_turnaround_time The sum of each tasks turnaround time.
 * @field mutex The lock that ensures mutual exclusion on threads accessing the
 * information.
 */
typedef struct
{
    int num_tasks;
    double total_waiting_time;
    double total_turnaround_time;
    pthread_mutex_t mutex;
} SchedulerInfo;

//FUNCTION PROTOTYPES
/**
 * @brief Creates a SchedulerInfo and allocates memory to it on the heap.
 *
 * The num_tasks, total_waiting_time, and total_turnaround_time are all
 * initialized to zero and the mutex lock for the struct is also initialised.
 *
 * @return A pointer to the SchedulerInfo struct on the heap.
 */
SchedulerInfo* schedulerInfo_create();

/**
 * @brief Deallocates all memory associated with the specified SchedulerInfo.
 *
 * Destroys the mutex lock and free's the whole struct from memory.
 *
 * @param info The SchedulerInfo to deallocate from memory.
 */
void schedulerInfo_free(SchedulerInfo* info);

int schedulerInfo_getNumTasks(SchedulerInfo* info);

#endif
