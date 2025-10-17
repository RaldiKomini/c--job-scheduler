#include "funcs.h"

//global so the handler can access them

int res2, res1; //file descriptors
int total = 1; //total jobs issued (helps with jobID)
int curCon = 1; //current concurrency
vector<info*> waitingq; //waiting queue
vector<info*> runningq; //running queue

void loop(int n) {

    char buffer[500];
    read(res1, buffer, 500); //read the buffer and find which case below
    
    if(strcmp(buffer, "./jobCommander exit") == 0) {//stop the loop
        //write the message
        string to_return = "jobExecutorServer terminated";
        write(res2,to_return.c_str(), 150);

        //Removes all waiting jobs
        int wsize = waitingq.size(); //store the size so it doesnt change in every iteration
        for(int i = 0; i < wsize;i++) {
            waitingq.erase(waitingq.begin()); //remove every time the first waiting element
        }
  
        //Terminates all running jobs
        int rsize = runningq.size(); //size of running
        vector<pid_t> temppids; //stores the pids of running jobs
        for(int i = 0; i <rsize;i++) {
            temppids.push_back(runningq[i]->pid);
        }
        for(int i = 0; i <rsize;i++) {
            kill(temppids[i], SIGTERM); //terminates the jobs
        }

        close(res1);
        close(res2);
        if(remove("jobExecutorServer.txt")!= 0)
            cout << "error deleting file" << endl;
        //delete the pipe
        remove("pipe1");
        remove("pipe2");

    }
    else if(strstr(buffer, "./jobCommander issueJob") != NULL){
        //issuejob with 1 at the end because its a new job, i want to save it
        issuejob(waitingq, runningq, buffer, &total, res2, curCon, 1);

    }
    else if(strstr(buffer, "./jobCommander setConcurrency") != NULL) {
        sscanf(buffer, "%*s %*s %d", &curCon);

        //if concurency changed to bigger value and there are jobs waiting run them
        while(runningq.size() < curCon && waitingq.size() > 0) {
            //with 0 at the end because jobs already are in waiting, just run them
            issuejob(waitingq, runningq, waitingq[0]->job, &total, res2, curCon, 0);
        }
        //then check if concurency got smaller
        //save the commands and their pids
        int currun = runningq.size();
        string a[currun];
        pid_t pids[currun];
        int j = 0;

        //if more running than should, and while j isnt getting more than running jobs
        while(currun > curCon && j <currun) { //save them
            a[j] = runningq[j]->job;
            pids[j] = runningq[j]->pid;
            j++;
        }
        j = 0;
        int dif = currun - curCon; //number of jobs that need to be put back in waiting
        while(j < dif) {
            //copies the commands in temporary buffer because issuejob() needs char*
            char mybuf[150];
            a[j].copy(mybuf, 149);
            mybuf[a[j].size()-1] = '\0'; //null terminate the end
            //issuejob with 1 because it needs to be saved again as a new one
            issuejob(waitingq, runningq, mybuf, &total, res2, curCon, 1);
            kill(pids[j], SIGTERM); //terminate the job
            j++;
        }
    }
    else if(strstr(buffer, "./jobCommander stop") != NULL) {
        string tmp = rmphrase(buffer, "stop");
        //first search for the job in running because it most of the times will have
        //smaller size than waiting
        for(int i = 0; i < (int) runningq.size(); i ++) {
            string tmp2(runningq[i]->jobID);
            if(tmp.compare(tmp2) == 0) { //if current ID is the same as the one we are looking for write message and terminate
                string to_return = mystring(runningq[i]->jobID)+ " terminated";
                write(res2,to_return.c_str(), 150);
                kill(runningq[i]->pid, SIGTERM);
                break; //found it stop the loop
            }
        }
        //else search for it in waiting with same logic
        for(int i = 0; i < (int) waitingq.size(); i ++) {
            string tmp2(waitingq[i]->jobID);
            if(tmp.compare(tmp2) == 0) {
                waitingq.erase(waitingq.begin() + i);
                string to_return = mystring(runningq[i]->jobID)+ " removed";
                write(res2,to_return.c_str(), 150);
                break;
            }
        }

    }
    else if(strstr(buffer, "./jobCommander poll running") != NULL) {
        string msg = "poll running: ";
        write(res2, msg.c_str(), 150);
        for(int i = 0; i <(int) runningq.size(); i ++) {
            string to_return = mystring(runningq[i]->jobID) + ", " + mystring(runningq[i]->job) + ", " + to_string(i);
            write(res2, to_return.c_str(), 150);
        }
    }
    else if(strstr(buffer, "./jobCommander poll queued") != NULL) { //an exei mono auto thelei srtcmp
        string msg = "poll queued: ";
        write(res2, msg.c_str(), 150);
        for(int i = 0; i <(int) waitingq.size(); i ++) {
            string to_return = mystring(waitingq[i]->jobID) + ", " + mystring(waitingq[i]->job) + ", " + to_string(i);
            write(res2, to_return.c_str(), 150);
        }
    }
    else {
        cout << "Wrong command!" << endl;
    }

            //empty buffer for next cmd
            memset(buffer, '\0', sizeof(buffer));
    return;


}

void handler(int n) {

  pid_t pid;
  pid = wait(NULL); //gets the pid of the child
    int pos = -1;
    for(int i = 0; i <(int) runningq.size(); i ++){
        if(runningq[i]->pid == pid)
            pos = i;
    }
    if(pos >= 0){ //if it found it it erases it from running
        runningq.erase(runningq.begin() + pos);
        if(waitingq.size() > 0){
            issuejob(waitingq, runningq, waitingq[0]->job, &total, res2, curCon, 0); //issues another job since there is space
        }
    }
    else {
        cout << "Not found" << endl;
    }
    return;
}

void jobExecutorServer() {

    res2 = open("pipe2", O_WRONLY | O_NONBLOCK); //writes the messages for JC


    //loop gets called when it receives a command
    struct sigaction lo;
    lo.sa_handler = loop;
    sigemptyset(&lo.sa_mask);
    lo.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &lo, NULL);

    //handler gets called when it receives a child
    struct sigaction si;
    si.sa_handler = handler;
    sigemptyset(&si.sa_mask);
    si.sa_flags = SA_RESTART;       
    sigaction(SIGCHLD, &si, NULL);
   
    //make file and write the pid
    ofstream myfile("jobExecutorServer.txt");
    myfile << getpid() <<endl;



    //open the pipe 
    res1 = open("pipe1", O_RDONLY | O_NONBLOCK); //reads the commands from res1


    int exit = 0; //dont stop until exit = 1


    //enters loop and pauses everytime waiting for a signal
    while(exit == 0) {
        pause();
    }


   
    
    return;
}










