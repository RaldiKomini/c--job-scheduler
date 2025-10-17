#include "funcs.h"

int res3;

int jobCommander(string cmd) {
    pid_t pid; //pid of child for jobExecutorServer
    ifstream file;
    int res2; //descriptor for writing in pipe
    file.open("jobExecutorServer.txt");
    int first_time = 0; //if txt is not created already change it to 1
    if(!file) {
        first_time = 1; //so that i can fork to read whatever JES is writing
        int res1 = mkfifo("pipe1", 0777);
        if(res1 !=0)
            cout << "failed mkfifo" << endl;
        int res11 = mkfifo("pipe2", 0777);
        if(res11 !=0)
            cout << "failed mkfifo2" << endl;

        pid = fork();
        if(pid == 0) { 
            jobExecutorServer();
            perror("execlp");
            return -10;

        }
        else if(pid > 0){
        }
        else {
            perror("fork");
        }

    }
    else{
        file >> pid; //write the pid in the file
    }
 

    res3 = open("pipe2", O_RDONLY); //to read
    res2 = open("pipe1", O_WRONLY); // to write
  
    if(first_time == 1) { //if its first time fork to read from res3
        pid_t pid2 = fork();
        if(pid2 == 0) {
            char buf[500];
            int n;
            cout << "in child res " << res3 << endl;
            while((n = read(res3, buf, 500))> 0){
                cout << buf << endl;
            }
        }
    }
    write(res2, cmd.c_str(), 100); //write the command for JES
    kill(pid, SIGUSR1); //send signal so it reads it
    sleep(1); //helped me for debugging when inputing file with multiple commands, not neccessary to work correct

    close(res2);
    close(res3);

    return 0;
}



int main() {
    string input;
    while (getline(cin, input)) { //read each line

        jobCommander(input);
        if(strcmp(input.c_str(), "./jobCommander exit") == 0) //if exit stop
        break;
    }
    
    return 0;
}