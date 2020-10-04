CC = clang
CFLAGS = -Wall -pedantic -ansi -std=c11 -g

EXEC = scheduler
OBJ = scheduler.o buffer.o task.o logFile.o schedulerInfo.o timeUtils.o

$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -lpthread

scheduler.o : scheduler.c scheduler.h buffer.h task.h logFile.h schedulerInfo.h timeUtils.h
	$(CC) -c scheduler.c $(CFLAGS)

buffer.o : buffer.c buffer.h task.h
	$(CC) -c buffer.c $(CFLAGS)

task.o : task.c task.h
	$(CC) -c task.c $(CFLAGS)

logFile.o : logFile.c logFile.h
	$(CC) -c logFile.c $(CFLAGS)

schedulerInfo.o : schedulerInfo.c schedulerInfo.h
	$(CC) -c schedulerInfo.c $(CFLAGS)

timeUtils.o : timeUtils.c timeUtils.h
	$(CC) -c timeUtils.c $(CFLAGS)


clean:
	$(RM) $(EXEC) $(OBJ) simulation_log
