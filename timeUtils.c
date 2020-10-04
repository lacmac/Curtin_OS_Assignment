/**
 * See documentation in the header file.
 */
#include "timeUtils.h"

struct tm* getCurrTime()
{
    time_t rawTime;
    time(&rawTime);
    return localtime(&rawTime);
}

int timeDiffSecs(const struct tm* const before, const struct tm* const after)
{
    return (after->tm_hour * 3600 + after->tm_min * 60 + after->tm_sec) -
           (before->tm_hour * 3600 + before->tm_min * 60 + before->tm_sec);
}

void logArrivalTime(FILE* outFile, int id, int burstLength, const struct tm* const arrivalTime)
{
    fprintf(outFile, "Task #%d: %d\n", id, burstLength);
    logTime(outFile, "Arrival", arrivalTime);
    fprintf(outFile, "\n");
}

void logServiceTime(FILE* outFile, int cpuID, int taskID,
                    const struct tm* const arrival, const struct tm* const service)
{
    fprintf(outFile, "Statistics for CPU-%d\n", cpuID);
    fprintf(outFile, "Task #%d\n", taskID);
    logTime(outFile, "Arrival", arrival);
    logTime(outFile, "Service", service);
    fprintf(outFile, "\n");
}

void logCompletionTime(FILE* outFile, int cpuID, int taskID,
                       const struct tm* const arrival, const struct tm* const completion)
{
    fprintf(outFile, "Statistics for CPU-%d\n", cpuID);
    fprintf(outFile, "Task #%d\n", taskID);
    logTime(outFile, "Arrival", arrival);
    logTime(outFile, "Completion", completion);
    fprintf(outFile, "\n");
}

void logTime(FILE* outFile, char* timeType, const struct tm* const time)
{
    fprintf(outFile, "%s time: %d:%d:%d\n", timeType, time->tm_hour, time->tm_min, time->tm_sec);
}
