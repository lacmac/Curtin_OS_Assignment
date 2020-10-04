/**
 * See documentation in the header file.
 */
#include "buffer.h"

Buffer* buffer_create(int capacity)
{
    Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
    buffer->tasks = (Task**) malloc(sizeof(Task*) * capacity);
    buffer->capacity = capacity;
    buffer->occupied = 0;
    buffer->in = 0;
    buffer->out = 0;
    pthread_mutex_init(&buffer->mutex, NULL);
    pthread_cond_init(&buffer->fullCond, NULL);
    pthread_cond_init(&buffer->emptyCond, NULL);

    return buffer;
}

void buffer_free(Buffer* buffer)
{
    free(buffer->tasks);
    pthread_mutex_destroy(&buffer->mutex);
    pthread_cond_destroy(&buffer->fullCond);
    pthread_cond_destroy(&buffer->emptyCond);
    free(buffer);
}

void buffer_insertNext(Buffer* buffer, Task* task)
{
    buffer->tasks[buffer->in++] = task;
    buffer->in %= buffer->capacity;
    (buffer->occupied)++;
}

Task* buffer_removeNext(Buffer* buffer)
{
    Task* task = buffer->tasks[buffer->out++];
    buffer->out %= buffer->capacity;
    (buffer->occupied)--;

    return task;
}

bool buffer_isEmpty(const Buffer* const buffer)
{
    return buffer->occupied == 0;
}

int buffer_numOfEmptySpaces(const Buffer* const buffer)
{
    return buffer->capacity - buffer->occupied;
}
