#include <iostream>
#include <fstream>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <queue>
#include <cstring>
#include <sys/wait.h> // For wait

using namespace std;

struct info {
    char jobID[10];
    char job[100];
    pid_t pid;
};

string mystring(char *s); //converts char array to string

//creates new job(depending on isnew) and runs it
int issuejob(vector<info*>& waitingq, vector<info*>& runningq, char* buffer, int* total, int res2, int con, int isnew); 

void jobExecutorServer();

string rmphrase(char*, string); //removes the phrase that is not needed