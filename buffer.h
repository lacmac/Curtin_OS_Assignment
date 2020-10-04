/**
 * @headerfile buffer.h
 * @brief Defines the structure of the buffer that is shared between the threads.
 *
 * @author Lachlan Mackenzie
 * @date 22/04/19
 */
#ifndef BUFFER_H
#define BUFFER_H

#include <pthread.h>
#include <stdbool.h>
#include "task.h"

//STRUCTS
/**
 * @brief This Buffer struct is used to store all of the scheduled tasks.
 *
 * This Buffer struct is shared between multiple threads and therefore needs a
 * mutual exclusion lock. The buffer is a circular queue wrapped over an array,
 * hence it keeps track of the head and tail of the queue. The buffer is
 * configured to store Task struct pointers, hence it is used for the scheduling
 * of tasks. It also uses pthread conditions to let the other threads know when
 * they can successfully perform their tasks. More details are in the scheduler.h
 * documentation.
 *
 * @field tasks The queue of task structs ready to be executed by the CPU threads.
 * @field capacity How many tasks the buffer can hold at once.
 * @field occupied How many spaces in the buffer have a task in them.
 * @field in The index of the next task to be inserted i.e. the tail of the buffer.
 * @field out The index of the next task to be removed i.e. the head of the buffer.
 * @field mutex The lock that ensures mutual exclusion on threads accessing the
 * buffer.
 * @field fullCond The condition that lets the CPU threads know that there is a
 * task in the buffer ready to be processed.
 * @field emptyCond The condition that lets the task thread know that there is
 * at least one empty space in the buffer for a task to be inserted in to.
 */
typedef struct
{
    Task** tasks;
    int capacity;
    int occupied;
    int in, out;
    pthread_mutex_t mutex;
    pthread_cond_t fullCond;
    pthread_cond_t emptyCond;
} Buffer;

//FUNCTION PROTOTYPES
/**
 * @brief Creates a Buffer of size 'capacity; and allocates memory to it on the
 * heap.
 *
 * This function allocates memory to the overall struct and the array of tasks
 * inside. The size of the buffer is set to the imported capacity. Number of 
 * occupied is initialized to 0, as well as the head and tail indices. The mutex
 * and pthread conditions are also initialized.
 *
 * @param capacity The maximum number of tasks the buffer can hold.
 * @return A pointer to the Buffer struct on the heap.
 */
Buffer* buffer_create(int capacity);

/**
 * @brief Deallocates all memory associated with the specified Buffer.
 *
 * Free's the array of tasks inside the buffer and destroys the pthread mutex and
 * conditions. Then frees the overall struct.
 *
 * @param buffer The Buffer to deallocate from memory.
 */
void buffer_free(Buffer* buffer);

/**
 * @brief Inserts a task struct into the next available spot in the buffer.
 * 
 * This function inserts a task at the index @c in. @c in is then incremented to
 * be ready for the next insertion, @c is also wrapped around to the start of the
 * array if it goes past then end of the buffer. The number of occupied spots is
 * also incremented.
 * 
 * @param buffer The buffer to insert the task in to.
 * @param task The task to be inserted.
 */
void buffer_insertNext(Buffer* buffer, Task* task);

/**
 * @brief Removes the next task from the buffer to be executed.
 *
 * This functions removes the task at the index @c out, which is then decremented.
 * If @c out is not in the array bounds, it is wrapped around to the end of the
 * buffer. The number of occupied spots is also decremented.
 *
 * @param buffer The buffer to remove the next task from.
 * @return The task that was removed from the buffer.
 */
Task* buffer_removeNext(Buffer* buffer);

/**
 * @brief Returns true if there are no tasks currently in the buffer.
 *
 * Checks if the number of occupied spots is equal to zero.
 *
 * @param buffer The buffer to check for emptiness.
 * @return True if there are no tasks currently in the buffer.
 */
bool buffer_isEmpty(const Buffer* const buffer);

/**
 * @brief Returns the number of spots that are not occupied in the buffer.
 *
 * The number of spots that are not occupied is equal to the capacity of the
 * buffer minus the number of occupied spots.
 *
 * @param buffer The buffer to calculate the number of empty spots in.
 * @return The number of spots that are not occupied in the given buffer.
 */
int buffer_numOfEmptySpaces(const Buffer* const buffer);

#endif
