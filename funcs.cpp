#include "funcs.h"

string mystring(char *s) {
    string b(s);
    return b;
}

int issuejob(vector<info*>& waitingq, vector<info*>& runningq, char* buffer, int* total,int res2, int con, int isnew) {

    //if it is a new job that is not saved, save it            
    if(isnew == 1){    
        info* newjob = new info; //new object
        strcpy(newjob->job, buffer); //copy cmd
        string temp = "job_" + to_string(*total); //makes string JobXX
        strcpy(newjob->jobID, temp.c_str()); //converts it to array and copies it
        waitingq.push_back(newjob); //push it in waiting
        (*total)++; //increase the total number of jobs that have been issued
        string to_return = mystring(newjob->jobID)+"," + mystring(newjob->job) +","+ to_string(waitingq.size()-1);
        write(res2,to_return.c_str(), 150);
    }
    //if it can run more jobs and there is at least one waiting, run it
    if((int) runningq.size() < con && waitingq.size() > 0) {
        runningq.push_back(waitingq[0]); //in running
        waitingq.erase(waitingq.begin()); //remove from waiting
        string tmp = rmphrase(buffer, "issuejob"); //store the command in tmp

        pid_t pid = fork(); //create new process
        if(pid == 0) { //if its the child run exec

            vector <char*> args; //stores the arguements of the cmd
            char* tok = strtok((char*) tmp.c_str(), " "); //tokens for spaces
            while(tok != nullptr) {
                args.push_back(tok); //pushes arguments in the vector
                tok = strtok(nullptr, " ");
            }
            args.push_back(nullptr); //has to end with NULL
            if (execvp(args[0], args.data()) == -1) { //executes the command
                    perror("execvp failed");
                    _exit(EXIT_FAILURE); // Terminate child process if execvp fails
                }
        }
        else {
            runningq[runningq.size()-1]->pid = pid; //save childs pid in running
        }
    }
    return 0;
}

string rmphrase(char* input, string cmd) { //removes ./jobCommander + (issuejob/stop etc) + " "
    string a(input);
    string to_rm = "./jobCommander ";
    a.erase(0, to_rm.length() + cmd.length() + 1); //+1 for the space gap at the end
    return a;

}