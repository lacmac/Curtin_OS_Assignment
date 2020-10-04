/**
 * @headerfile scheduler.h
 * @brief This file is an implementation of a preemptive CPU scheduler.
 *
 * The tasks to be scheduled are stored in a file that is given by the user
 * through the command line arguments. The task file is to be given in the
 * following format: task# cpu_burst_length
 * For this scheduler there is one thread placing tasks into the buffer and three
 * 'CPU' threads retrieving tasks from the buffer and 'executing' them. All while
 * avoiding the possible race conditions where no progress can occur.
 * More details on what each does below.
 *
 * @author Lachlan Mackenzie
 * @date 31/03/19
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>
#include <unistd.h>
#include <stdbool.h>
#include "buffer.h"
#include "task.h"
#include "logFile.h"
#include "schedulerInfo.h"
#include "timeUtils.h"

//CONSTANTS
/**
 * The number of additional command line arguments.
 */
#define NUM_ARGS 2

/**
 * The number of CPU threads.
 */
#define NUM_CPUS 3

/**
 * The maximum number of tasks to be scheduled. This is the size of the array
 * of tasks that are read from the given file.
 */
#define MAX_NUM_TASKS 10000

/**
 * The smallest buffer capacity.
 */
#define MIN_BUFFER_CAP 1

/**
 * The largest buffer capacity.
 */
#define MAX_BUFFER_CAP 10

/**
 * The largest size of each line of the input file in characters. Ten should be
 * plenty.
 */
#define MAX_LINE_SIZE 10

//GLOBAL VARIABLES
/**
 * @brief This buffer is used as a Ready Queue for the task and CPU threads to
 * share while avoiding race conditions.
 *
 *  This buffer is used between both the task and CPU threads. The task thread
 * is inserting tasks into the buffer while the CPU threads are removing tasks
 * from the buffer for execution.
 *  The buffer uses a mutex lock to ensure no other thread is accessing the
 * buffer simultaneously, ensuring mutual exclusion.
 *  The task thread is blocked if the buffer does not have enough empty spaces
 * to insert a number of tasks and waits for a signal from the CPU threads when
 * a task is removed from the buffer.
 *  The CPU threads are blocked when the buffer is empty as it cannot execute a
 * task if there are none, these threads are awaiting a signal from the task
 * thread when a task is inserted into the buffer, ready for execution. Ensuring
 * progress is made.
 *
 * @see buffer.h for details on the datatype's structure.
 */
Buffer* task_buffer;

/**
 * @brief This log file is used by all threads to output their respective
 * data that needs to be logged.
 *
 *  This log file is used by both the task threads and the CPU threads.
 *  The task thread logs the arrival time of each task to file and also when the
 * thread has inserted all tasks into the buffer, it logs its termination time
 * and how many tasks were inserted.
 *  The CPU threads log the service time and completion time of the task to file.
 * When the CPU threads terminate, they will also log how many task they have
 * serviced.
 *
 * @see logFile.h for details on the datatype's structure.
*/
Log* sim_log;

/**
 * @brief This structure is used to store the statistics of the CPU scheduler.
 *
 * This struct is only used by the CPU threads and then the main thread when the
 * CPU's have terminated. Each CPU thread updates these statistics every time a
 * task has been serviced or completed. The only race conditions that can occur
 * are between the CPU threads.
 *
 * @see schedulerInfo.h for details on the datatype's structure.
*/
SchedulerInfo* cpu_info;

/**
 * @brief This variable stores the number of tasks found in the input file.
 *
 * This global variable is used by both the task and cpu threads. The task thread
 * uses this to know when all tasks have been entered into the buffer. The CPU
 * threads use this variable to know when to exit their thread. No race
 * conditions can occur for the global variable as it was only written to when
 * only one thread existed, after that it is only read by the other reads.
*/
int total_num_tasks;

//FUNCTION PROTOTYPES
/**
 * @brief The function that the task thread executes on creation. Responsible
 * for inserting tasks into the buffer.
 *
 * See more info on this function in the inline documentation.
 *
 * @param taskFileName The name of the task file to read the task from.
 */
void* task(void* taskFileName);

/**
 * @brief The function that the CPU threads execute on creation. It is responsible
 * for removing tasks from the buffer and 'executing' them.
 *
 * See more info on this function in the inline documentation.
 *
 * @param cpuThreadID 
 */
void* cpu(void* cpuThreadID);

/**
 * @brief Calculates the number of tasks from a given file.
 *
 * The function loops over every line in the file, while incrementing a counter
 * for each line.
 *
 * @param taskFile The name of the file the tasks are stored in.
 * @return The number of lines in the given file i.e. the number of tasks.
 */
int getNumTasks(const char* const taskFile);

#endif
