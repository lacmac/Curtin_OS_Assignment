/**
 * See documentation in the header file.
 */
#include "task.h"

Task* task_create(int id, int burstLength)
{
    Task* task = (Task*) malloc(sizeof(Task));
    task->id = id;
    task->burst = burstLength;
    task->arrivalT = malloc(sizeof(struct tm));
    task->serviceT = malloc(sizeof(struct tm));
    task->completionT = malloc(sizeof(struct tm));

    return task;
}

void task_free(Task* task)
{
    free(task->arrivalT);
    free(task->serviceT);
    free(task->completionT);
    free(task);
}
