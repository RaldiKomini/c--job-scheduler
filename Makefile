SOURCE = jobExecutor.cpp jobExecutorServer.cpp funcs.cpp 
OBJS =  jobExecutor.o jobExecutorServer.o funcs.o
HEADER = funcs.h
OUT = jobCommander
FILES = jobExecutorServer.txt pipe1 pipe2
CC = g++
FLAGS = -Wall -Wextra


build: $(SOURCE) $(HEADER)
	$(CC) -o $(OUT) $(SOURCE)

clean:
	rm -f $(OBJS) $(OUT) $(FILES)

rebuild: clean build
