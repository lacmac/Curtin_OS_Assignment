/**
 * See documentation in the header file.
 */
#include "logFile.h"

Log* log_create(const char* filename)
{
    Log* logfile = malloc(sizeof(Log));
    logfile->file = fopen(filename, "w");
    pthread_mutex_init(&logfile->mutex, NULL);

    return logfile;
}

void log_free(Log* logfile)
{
    fclose(logfile->file);
    pthread_mutex_destroy(&logfile->mutex);
    free(logfile);
}
