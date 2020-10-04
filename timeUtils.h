/**
 * @headerfile timeUtils.h
 * @brief File containing functions for printing different types of times
 * belonging to a scheduled task to a log file.
 *
 * @author Lachlan Mackenzie
 * @date 02/05/19
 */
#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <stdio.h>
#include <time.h>

/**
 * @brief Retrieves the current time of execution.
 *
 * @return The current time of type, @c tm*.
 */
struct tm* getCurrTime();

/**
 * @brief Calculates the number of seconds between two different times.
 *
 * @param before The time used to calculate the difference from.
 * @param after The time used to calculate the difference to.
 * @return The number of seconds between the times given.
 */
int timeDiffSecs(const struct tm* const before, const struct tm* const after);

/**
 * @brief Logs the arrival time of a specific task to the given file.
 *
 * @param outFile The file to write the given information to.
 * @param id The ID of the task.
 * @param burstLength The CPU burst time of the task.
 * @param arrivalTime The time the task arrived in the Ready Queue.
 */
void logArrivalTime(FILE* outFile, int id, int burstLength, const struct tm* const arrivalTime);

/**
 * @brief Logs the service time of a specific task to the given file.
 *
 * @param outFile The file to write the given information to.
 * @param cpuID The ID of the CPU that removed the task from the Ready Queue.
 * @param taskID The ID of the task that was removed from the Ready Queue.
 * @param arrival The time the task arrived in the Ready Queue.
 * @param service The time the task was removed by the CPU.
 */
void logServiceTime(FILE* outFile, int cpuID, int taskID,
                    const struct tm* const arrival, const struct tm* const service);

/**
 * @brief Logs the completion time of a specific task to the given file.
 *
 * @param outFile The file to write the given information to.
 * @param cpuID The ID of the CPU that executed the task.
 * @param taskID The ID of the task that was executed.
 * @param arrival The time the task arrived in the Ready Queue.
 * @param service The time the task had finished execution by the CPU.
 */
void logCompletionTime(FILE* outFile, int cpuID, int taskID,
                       const struct tm* const arrival, const struct tm* const completion);

/**
 * @brief Logs a @c tm* struct in the format of hh:mm:ss to a specified file.
 *
 * @param outFile The file to write the given information to.
 * @param timeType The string to be logged alongside the time.
 * @param time The time to be logged.
 */
void logTime(FILE* outFile, char* timeType, const struct tm* const time);
#endif
