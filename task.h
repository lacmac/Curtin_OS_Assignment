/**
 * @headerfile task.h
 * @brief Defines the structure of a Task and the basic functions for creating
 * and destroying said structure.
 *
 * @author Lachlan Mackenzie
 * @date 01/04/19
 */
#ifndef TASK_H
#define TASK_H

#include <stdlib.h>
#include <time.h>

//STRUCTS
/**
 * @brief This Task struct is used to store information about each task in the
 * CPU scheduler.
 *
 * This struct is used to contain all the important information regarding a task
 * used in a CPU scheduler. The struct is initialized by the task thread and
 * then inserted into the bounded Ready Queue. The three CPU threads executing
 * cpu() will then remove this struct from the Ready Queue and 'execute' it.
 * After the task has been executed it is then free'd from memory.
 *
 * @field id The identifier of the task.
 * @field burst The length of the task execution in seconds.
 * @field arrivalT The time the task arrived in the Ready Queue.
 * @field serviceT The time the task was removed from the buffer.
 * @field completionT The time the task had finished execution.
 */
typedef struct
{
    int id;
    int burst;
    struct tm* arrivalT;
    struct tm* serviceT;
    struct tm* completionT;
} Task;

//FUNCTION PROTOTYPES
/**
 * @brief Creates a Task struct with given ID and burst length and allocates
 * memory to it on the heap.
 *
 * The id and burstLength of the Task are assigned to their respective fields and
 * the arrivalTime, serviceTime, and completionTime are all allocated to memory
 * on the heap.
 *
 * @param id The identifier of the task.
 * @param burstLength The length of the task execution in seconds.
 * @return A pointer to the Task struct on the heap.
 */
Task* task_create(int id, int burstLength);

/**
 * @brief Deallocates all memory associated with the specified Task.
 *
 * Free's each time value and destroys the mutex lock assigned to the struct.
 *
 * @param task The Task to deallocate from memory.
 */
void task_free(Task* task);

#endif
