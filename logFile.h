/**
 * @headerfile logFile.h
 * @brief Defines the structure of the Log file that is shared between threads and
 * the basic functions of creating and destroying said structure.
 *
 * @author Lachlan Mackenzie
 * @date 01/05/19
 */
#ifndef LOGFILE_H
#define LOGFILE_H

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

/**
 * @brief This Log struct is used to store the file pointer and the mutex lock
 * used to access it safely between threads.
 *
 * @field file The output file that is shared between the task and cpu threads.
 * @field mutex The lock that ensures mutual exclusion on threads accessing the
 * file.
 */
typedef struct
{
    FILE* file;
    pthread_mutex_t mutex;
} Log;

/**
 * @brief Creates a Log struct and allocates memory to it on the heap.
 *
 * The file with the given filename is opened and the mutex is initialised.
 *
 * @param filename The name of the file to share between threads.
 * @return A pointer to the SchedulerInfo struct on the heap.
 */
Log* log_create(const char* filename);

/**
 * @brief Deallocates all memory associated with the specified Log struct.
 *
 * Closes the shared file and destroys the mutex lock and free's the whole
 * struct from memory.
 *
 * @param logfile The Log struct to deallocate from memory.
 */
void log_free(Log* logfile);

#endif
