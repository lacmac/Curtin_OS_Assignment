#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void createRandomTaskFile(const char* fileName, int num)
{
    srand((unsigned int) time(0));
    FILE* file = fopen(fileName, "w");
    if (file != NULL)
    {
        for (int i = 1; i <= num; i++)
        {
            fprintf(file, "%d %d\n", i, rand() % 49 + 1);
        }
        fclose(file);
    }
}

int main(void)
{
    createRandomTaskFile("task_file", 100);
}
