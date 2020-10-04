/**
 * See documentation in the header file.
 */
#include "schedulerInfo.h"

SchedulerInfo* schedulerInfo_create()
{
    SchedulerInfo* info = malloc(sizeof(SchedulerInfo));
    info->num_tasks = 0;
    info->total_waiting_time = 0;
    info->total_turnaround_time = 0;
    pthread_mutex_init(&info->mutex, NULL);

    return info;
}

void schedulerInfo_free(SchedulerInfo* info)
{
    pthread_mutex_destroy(&info->mutex);
    free(info);
}

int schedulerInfo_getNumTasks(SchedulerInfo* info)
{
    pthread_mutex_lock(&info->mutex);
    int numTasks = info->num_tasks;
    pthread_mutex_unlock(&info->mutex);

    return numTasks;
}
