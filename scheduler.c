/**
 * See documentation in the header file.
 */
#include "scheduler.h"

int main(int argc, char* argv[])
{
    //ENSURE CORRECT AMOUNT OF COMMAND LINE ARGUMENTS ARE PRESENT
    if (argc != NUM_ARGS + 1)
    {
        fprintf(stderr, "ERROR: Invalid number of command line arguments.\n");
        fprintf(stderr, "Usage: ./scheduler [task file name] [queue size]\n");
        return -1;
    }

    //RENAME COMMAND LINE ARGUMENTS FOR READABILITY
    const char* taskFile = argv[1];
    char* endPtr;
    const int bufferSize = (int) strtol(argv[2], &endPtr, 10);

    //CHECK THAT bufferSize IS WITHIN A VALID RANGE
    if (*endPtr != '\0' || bufferSize < MIN_BUFFER_CAP || bufferSize > MAX_BUFFER_CAP)
    {
        fprintf(stderr, "ERROR: Buffer size must be an integer between 1 and 10.\n");
        return -1;
    }

    //INITIALISE GLOBAL VARIABLES FOR THREAD SHARING
    total_num_tasks = getNumTasks(taskFile);
    task_buffer = buffer_create(bufferSize);
    sim_log = log_create("simulation_log");
    if (sim_log->file == NULL)
    {
        perror("ERROR: The log file could not be opened/created.\n");
        buffer_free(task_buffer);
        log_free(sim_log);
        return -1;
    }
    cpu_info = schedulerInfo_create();

    //CREATE THREADS
    pthread_t* taskThread = (pthread_t*) malloc(sizeof(pthread_t));
    pthread_t* cpuThreads = (pthread_t*) malloc(sizeof(pthread_t) * NUM_CPUS);

    //EXECUTE THREADS
    int cpuID[] = {1, 2, 3};
    pthread_create(taskThread, NULL, task, (void*) taskFile);
    for (int i = 0; i < NUM_CPUS; i++)
    {
        pthread_create(&cpuThreads[i], NULL, cpu, &(cpuID[i]));
    }
    printf("Running...\n");

    //JOIN TASK AND CPU THREADS BACK INTO THE MAIN THREAD
    pthread_join(*taskThread, NULL);
    for (int i = 0; i < NUM_CPUS; i++)
    {
        pthread_join(cpuThreads[i], NULL);
    }
    printf("Done.\n");

    //LOG FINAL VALUES
    fprintf(sim_log->file, "Number of tasks: %d\n", cpu_info->num_tasks);
    fprintf(sim_log->file, "Average waiting time: %.3f\n",
            (float) cpu_info->total_waiting_time / (float) cpu_info->num_tasks);
    fprintf(sim_log->file, "Average turnaround time: %.3f\n",
            (float) cpu_info->total_turnaround_time / (float) cpu_info->num_tasks);

    //FREE RESOURCES
    buffer_free(task_buffer);
    log_free(sim_log);
    schedulerInfo_free(cpu_info);
    free(taskThread);
    free(cpuThreads);

    return 0;
}

void* task(void* taskFileName)
{
    FILE* taskFile = NULL;
    char line[MAX_LINE_SIZE + 1];
    char lines[MAX_NUM_TASKS][MAX_LINE_SIZE + 1];
    struct tm* currTime;
    int taskID, taskBurstTime;
    int tasksInserted = 0;
    bool lastSetOfTasks, oddNumOfTasks, shouldAddSecondTask;

    //OPENS THE TASKFILE FOR READING
    taskFile = fopen((char*) taskFileName, "r");
    if (taskFile == NULL)
    {
        perror("ERROR: The task file could not be opened ");
        buffer_free(task_buffer);
        log_free(sim_log);
        schedulerInfo_free(cpu_info);
        exit(-1);
    }

    //READS THE WHOLE FILE INTO AN ARRAY
    int numOfTasks = 0;
    while (numOfTasks < total_num_tasks)
    {
        fgets(line, MAX_LINE_SIZE + 1, taskFile);
        strncpy(lines[numOfTasks++], line, MAX_LINE_SIZE + 1);
    }
    fclose(taskFile);
    oddNumOfTasks = total_num_tasks % 2 != 0;

    //READS THE ARRAY TWO TASKS AT A TIME
    int currentTaskNum = 0;
    int numOfEmptySpacesNeeded = task_buffer->capacity > 1 ? 2 : 1;
    while (currentTaskNum < total_num_tasks)
    {
        Task* task1 = NULL, * task2 = NULL;
        //UP TO THE FINAL SET OF TASKS
        lastSetOfTasks = currentTaskNum > total_num_tasks - 2;

        //A SECOND TASK CANT BE ADDED WHEN THERE IS AN ODD NUMBER OF LINES AND
        // YOU ARE UP TO THE LAST SET OF TASKS. ALSO YOU CAN ONLY ADD TWO TASKS
        // IF THE BUFFER HAS A SIZE WHICH IS GREATER THAN 1
        shouldAddSecondTask = !(lastSetOfTasks && oddNumOfTasks) &&
                              task_buffer->capacity > 1;

        //PROCESS EACH LINE INTO TASK VARIABLES
        sscanf(lines[currentTaskNum++], "%d %d", &taskID, &taskBurstTime);
        task1 = task_create(taskID, taskBurstTime);
        if (shouldAddSecondTask)
        {
            sscanf(lines[currentTaskNum++], "%d %d", &taskID, &taskBurstTime);
            task2 = task_create(taskID, taskBurstTime);
        }

        //OBTAIN LOCK ON THE BUFFER
        pthread_mutex_lock(&task_buffer->mutex);
        //WAIT UNTIL THE BUFFER HAS AT LEAST THE FREE SLOTS REQUIRED
        while (buffer_numOfEmptySpaces(task_buffer) < numOfEmptySpacesNeeded)
        {
            //WHILE WAITING FOR REQUIRED EMPTY SLOTS, GIVE UP LOCK ON THE BUFFER
            pthread_cond_wait(&task_buffer->emptyCond, &task_buffer->mutex);
        }

        //RETRIEVE AND STORE ARRIVAL TIME FOR BOTH TASKS
        currTime = getCurrTime();
        memcpy(task1->arrivalT, currTime, sizeof(struct tm));
        if (shouldAddSecondTask)
        {
            memcpy(task2->arrivalT, currTime, sizeof(struct tm));
        }

        //INSERT TWO TASKS INTO THE BUFFER AT A TIME
        buffer_insertNext(task_buffer, task1);
        tasksInserted++;
        if (shouldAddSecondTask)
        {
            buffer_insertNext(task_buffer, task2);
            tasksInserted++;
        }

        //LOG ARRIVAL TIME OF BOTH TASKS TO FILE
        pthread_mutex_lock(&sim_log->mutex);
        logArrivalTime(sim_log->file, task1->id, task1->burst, task1->arrivalT);
        if (shouldAddSecondTask)
        {
            logArrivalTime(sim_log->file, task2->id, task2->burst, task2->arrivalT);
        }
        pthread_mutex_unlock(&sim_log->mutex);

        //RELEASE THE BUFFER LOCK AND SIGNALS ALL CPU'S THAT A FULL SLOT IS IN THE BUFFER
        pthread_mutex_unlock(&task_buffer->mutex);
        pthread_cond_broadcast(&task_buffer->fullCond);
    }

    //LOG TASK THREAD COMPLETION
    pthread_mutex_lock(&sim_log->mutex);
    fprintf(sim_log->file, "Number of tasks put into Ready-Queue: %d\n", tasksInserted);
    logTime(sim_log->file, "Terminate at", getCurrTime());
    fprintf(sim_log->file, "\n");
    pthread_mutex_unlock(&sim_log->mutex);

    pthread_exit(0);
}

void* cpu(void* cpuThreadID)
{
    Task* task;
    const int cpuID = *(int*) cpuThreadID;
    int tasksCompleted = 0;

    //CPU HAS WORK TO DO UNTIL EVERY TASK HAS BEEN PROCESSED
    while (schedulerInfo_getNumTasks(cpu_info) < total_num_tasks)
    {
        //OBTAIN LOCK ON THE BUFFER
        pthread_mutex_lock(&task_buffer->mutex);
        //WAIT UNTIL THE BUFFER HAS AT LEAST ONE TASK IN IT
        while (buffer_isEmpty(task_buffer))
        {
            //WHILE WAITING FOR A FULL SLOT, GIVE UP LOCK ON THE BUFFER
            pthread_cond_wait(&task_buffer->fullCond, &task_buffer->mutex);
        }

        //REMOVE TASK FROM BUFFER
        task = buffer_removeNext(task_buffer);

        //RETRIEVE AND STORE SERVICE TIME FOR THE TASK
        memcpy(task->serviceT, getCurrTime(), sizeof(struct tm));

        //LOG SERVICE TIME TO FILE
        pthread_mutex_lock(&sim_log->mutex);
        logServiceTime(sim_log->file, cpuID, task->id, task->arrivalT, task->serviceT);
        pthread_mutex_unlock(&sim_log->mutex);

        //RELEASE THE BUFFER LOCK AND SIGNAL THAT AN EMPTY SLOT IS IN THE BUFFER
        pthread_mutex_unlock(&task_buffer->mutex);
        pthread_cond_signal(&task_buffer->emptyCond);

        //UPDATE SHARED VALUES
        pthread_mutex_lock(&cpu_info->mutex);
        (cpu_info->num_tasks)++; //HAS TO BE UPDATED BEFORE THE CPU BURST
        pthread_mutex_unlock(&cpu_info->mutex);

        //CPU BURST
        usleep((__useconds_t) (task->burst * 1000000 / 5));

        //RETRIEVE AND STORE COMPLETION TIME FOR THE TASK
        memcpy(task->completionT, getCurrTime(), sizeof(struct tm));

        //LOG COMPLETION TIME TO FILE
        pthread_mutex_lock(&sim_log->mutex);
        logCompletionTime(sim_log->file, cpuID, task->id, task->arrivalT, task->completionT);
        pthread_mutex_unlock(&sim_log->mutex);

        //UPDATE SHARED VALUES
        pthread_mutex_lock(&cpu_info->mutex);
        cpu_info->total_waiting_time += timeDiffSecs(task->arrivalT, task->serviceT);
        cpu_info->total_turnaround_time += timeDiffSecs(task->arrivalT, task->completionT);
        pthread_mutex_unlock(&cpu_info->mutex);

        tasksCompleted++;
        printf("%d\n", task->id);
        task_free(task);
    }

    //LOG CPU TERMINATION
    pthread_mutex_lock(&sim_log->mutex);
    fprintf(sim_log->file, "CPU-%d terminates after servicing %d tasks.\n\n",
            cpuID, tasksCompleted);
    pthread_mutex_unlock(&sim_log->mutex);

    pthread_exit(0);
}

int getNumTasks(const char* const taskFile)
{
    FILE* file = NULL;
    int numTasks = 0;
    char line[MAX_LINE_SIZE + 1];

    //EXIT PROGRAM IF THE TASKFILE CAN NOT BE OPENED
    if ((file = fopen(taskFile, "r")) == NULL)
    {
        perror("ERROR: The task file could not be opened ");
        exit(-1);
    }

    //COUNT UP THE NUMBER OF LINES IN THE FILE
    while (fgets(line, MAX_LINE_SIZE + 1, file) != NULL)
    {
        numTasks++;
    }
    fclose(file);

    return numTasks;
}
