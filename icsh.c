#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
//Gorramuth P.
//6280632
//Somehow the code is still outputting into input. I have no idea how to fix that.
//NOTE: 2 waitpid can cause trouble, but none that I can find so far

//Tic-tac-toe code
//BUG: Please only enter 1-9, enter literally anything else will break the game

bool isTied(int board[]){
    if(board[0]!=0 &&
       board[1]!=0 &&
       board[2]!=0 &&
       board[3]!=0 &&
       board[4]!=0 &&
       board[5]!=0 &&
       board[6]!=0 &&
       board[7]!=0 &&
       board[8]!=0){
        return true;
       }
       return false;
}

bool checkBoard(int board[]){
    if((board[0]==board[1] && board[1]==board[2] && board[0]!=0) ||
       (board[3]==board[4] && board[4]==board[5] && board[3]!=0) ||
       (board[6]==board[7] && board[7]==board[8] && board[6]!=0) ||
       (board[0]==board[3] && board[3]==board[6] && board[0]!=0) ||
       (board[1]==board[4] && board[4]==board[7] && board[1]!=0) ||
       (board[2]==board[5] && board[5]==board[8] && board[2]!=0) ||
       (board[0]==board[3] && board[3]==board[6] && board[0]!=0) ||
       (board[0]==board[4] && board[4]==board[8] && board[0]!=0) ||
       (board[2]==board[4] && board[4]==board[6] && board[2]!=0)){
           printf("Game Over\n");
        return true;
       }
    return false;
}

void printXO(int value){
    if(value==1){
        printf("x");
    }
    else if(value==2){
        printf("o");
    }
    else{
        printf(" ");
    }
}

void printBoard(int board[]){

    printf("  ");
    printXO(board[0]);
    printf("  |  ");
    printXO(board[1]);
    printf("  |  ");
    printXO(board[2]);
    printf("  \n");
    printf("-----------------\n");

    printf("  ");
    printXO(board[3]);
    printf("  |  ");
    printXO(board[4]);
    printf("  |  ");
    printXO(board[5]);
    printf("  \n");
    printf("-----------------\n");

    printf("  ");
    printXO(board[6]);
    printf("  |  ");
    printXO(board[7]);
    printf("  |  ");
    printXO(board[8]);
    printf("  \n");

}

void playGame(){
    printf("Welcome to tic-tac-toe. Enter 1-9 to play, enter -1 to quit.\n");
    int board[9] = {0};
    bool playerTurn = true;
    bool aiTurn = false;
    bool gameOver = false;

    while(!gameOver){
        printBoard(board);
        printf("\n\n");
        if(playerTurn){
            int cell;
            printf("Please enter cell: ");
            scanf("%d",&cell);
            if(cell==-1){
                return;
            }
            if(cell<1 || cell>9){
                    return;
                }

            while(board[cell-1]!=0){
                printf("Please enter valid cell: ");
                scanf("%d",&cell);
                if(cell==-1){
                    return;
                }
                if(cell<1 || cell>9){
                    return;
                }
            }

            board[cell-1]=1;
            if(checkBoard(board)){
                printf("\n\n");
                printBoard(board);
                printf("You win!\n");
                break;
            }
            if(isTied(board)){
                printf("Tied\n");
                break;
            }
            playerTurn=false;
            aiTurn=true;
            continue;
        }
        else if(aiTurn){
            int aiCell=(rand()%(9-1+1))+1;

            while(board[aiCell-1]!=0){
                aiCell=(rand()%(9-1+1))+1;
            }

            board[aiCell-1]=2;
            if(checkBoard(board)){
                printf("\n\n");
                printBoard(board);
                printf("AI wins\n");
                break;
            }
            if(isTied(board)){
                printf("Tied\n");
                break;
            }
            playerTurn=true;
            aiTurn=false;
            continue;
        }
    }

    return;
}



//Creating a job object
typedef struct Job{
int id;
char* jobStatus;
char* shellCommand;
int pid;
bool inBg;

} Job;

Job* jobTable;
int exitFlag;
int isScripted;

void sigIntHandler(int signum){
printf("\nInterrupt signal %d received\n",signum);
}

void sigStopHandler(int signum){
printf("\nStop signal %d received\n",signum);
}

void addJob(int pid, char* command,int bgFlag){
    for(int i=0;i<50;i++){
        if(jobTable[i].id==-99){
            jobTable[i].id=i;
            jobTable[i].pid=pid;
            jobTable[i].jobStatus="Running";
            jobTable[i].shellCommand=strdup(command);
            jobTable[i].inBg=false;
            if(bgFlag==1){
                jobTable[i].inBg=true;
                printf("[%d] %d\n",jobTable[i].id,jobTable[i].pid);
            }
            break;
        }
    }
}

void removeJob(int childPid){
    for(int i=0;i<50;i++){
        if(jobTable[i].pid==childPid){
            jobTable[i].id=-99;
            jobTable[i].pid=-99;
            jobTable[i].jobStatus="";
            jobTable[i].shellCommand="";
            jobTable[i].inBg=false;
        }
    }
}

void ChildHandler(int sig,siginfo_t *sip,void *notused){
int status=0;
if(sip->si_pid==waitpid(sip->si_pid,&status,WNOHANG)){
    if(WIFEXITED(status)||WTERMSIG(status)){
        //printf("The child is deleted\n");
        for(int i=0;i<50;i++){
            if(jobTable[i].pid==sip->si_pid && jobTable[i].inBg==true){
                printf("[%d] Done    %s\n",jobTable[i].id,jobTable[i].shellCommand);
            }
        }
        removeJob(sip->si_pid);
    }
   }
}

int checkIfBackground(char* input){
    char *ret;
    ret=strstr(input," &");
    if(ret){
        return 1;
    }
    else{
        return 0;
    }
}

//Function to make argument list from string
char **makeArgument(char *input){
    char **arguments = calloc(300,sizeof(char *));
    int idx=0;

    char *argument = strtok(input," \t");
    while(argument != NULL){
        arguments[idx]=argument;
        argument=strtok(NULL," \t");
        idx++;
    }
    arguments[idx]=NULL;
    return arguments;
}

//Function to wait for fork and handles returned status
int handleForkTermination(int pid){

        int status;
        setpgid(pid,pid);
        tcsetpgrp(0,pid);

        //WUNTRACED checks if signal is stopped
        //WNOHANG make sure to keep waiting for the child until SIGCONT reaches
        //No need for sleep(1)
        int returnStatus=waitpid(pid,&status,WUNTRACED | WNOHANG);
        while(returnStatus==0){
            returnStatus=waitpid(pid,&status,WUNTRACED | WNOHANG);
        }

        if(WIFSTOPPED(status)){
            kill(pid,SIGTSTP);
            //printf("Stopped\n");

            //Do not remove
            //Mark as stopped
            for(int i=0;i<50;i++){
                if(jobTable[i].pid==pid){
                    jobTable[i].jobStatus="Stopped";
                    jobTable[i].inBg=true;
                    printf("[%d] %s    %s\n",jobTable[i].id,jobTable[i].jobStatus,jobTable[i].shellCommand);
                }
            }
            //printf("Fork terminated\n");
            tcsetpgrp(0,getpid());
        }
        else if(WIFEXITED(status)){
            //printf("Exited\n");

            removeJob(pid);
            //printf("Fork terminated\n");
            tcsetpgrp(0,getpid());
            int statusCode=WEXITSTATUS(status);

            //Return whatever exit code is given from the fork
            return statusCode;
        }
        else if(WIFSIGNALED(status)){
            // Exited by SIGINT
            //printf("FORCED by SIGINT\n");
            removeJob(pid);
            //printf("Fork terminated\n");
            tcsetpgrp(0,getpid());
            return 3;
        }
        else{
            // Nothing happened, exits normally
            removeJob(pid);
            //printf("Fork terminated\n");
            tcsetpgrp(0,getpid());
            return 0;
        }
        return 0;

}

//Function to fork and run processes
//Process external commands
int runProcess(char* command, char** argument,int ioOutFlag,int ioInFlag,char* fname,int bgStatus){
        //printf("Forking...\n");
        pid_t pid = fork();

        //Error case
        if(pid<0){
            printf("Fork error\n");
            //Error 40 fork error
            exit(40);
        }
        //CHILD
        //Child case background
        else if(pid == 0 && bgStatus == 1){
            //Handle signals back to default
            struct sigaction childsig;
            childsig.sa_handler = SIG_DFL;
            sigaction(SIGTSTP,&childsig,NULL);
            sigaction(SIGINT,&childsig,NULL);


            int bgPID = getpid();
            //printf("Background PID: %d\n",bgPID);


            int execReturnCode=execvp(argument[0],argument);
            if(execReturnCode<=0){
                printf("bad command\n");
                //Error 4 bad command
                exit(4);
            }
            else{
                exit(0);
            }
        }
        //CHILD
        //Child case foreground
        else if(pid == 0 && bgStatus != 1){
            //Handles IO redirection
            if(ioOutFlag){
                int fileOutput = open(fname, O_CREAT | O_WRONLY,0777);
                dup2(fileOutput,STDOUT_FILENO);
                close(fileOutput);
            }
            else if(ioInFlag){
                int fileInput = open(fname, O_RDONLY);
                if(fileInput<0){
                    printf("Input file does not exist\n");
                    //Error 30 no input file
                    exit(30);
                }
                dup2(fileInput, STDIN_FILENO);
                close(fileInput);
            }

            //Set to foreground
            int childPid=getpid();
            int execReturnCode;
            setpgid(childPid,childPid);
            tcsetpgrp(0,childPid);

            //Handle signal
            struct sigaction childsig;
            childsig.sa_handler = SIG_DFL;
            sigaction(SIGTSTP,&childsig,NULL);
            sigaction(SIGINT,&childsig,NULL);

            execReturnCode=execvp(argument[0],argument);

            if(execReturnCode<=0){
                printf("bad command\n");
                exit(4);
            }
            else{
                exit(0);
            }

        }
        //PARENTS
        else {
        // Parent case not background, parent waits
            if(bgStatus!=1){
                //Add to job table as running
                addJob(pid,command,0);
                //Wait for job
                return handleForkTermination(pid);
            }
            //Parent case background, don't wait
            else{
                //Add to Job table as running
                addJob(pid,command,1);
                return 0;
            }
        }
}

//Function to parse user input and execute commands
int parseUserInput(char* input, char* prevCommand,int exitCode){
    if(strlen(input)==0){
        return 0;
    }
    //COMMAND: ttt
    //play tic tac toe
    else if(input[0]=='t' && input[0]=='t' && input[0]=='t'){
        playGame();
        return 0;
    }
    //COMMAND: fg
    //Brings background job to foreground
    else if(input[0]=='f' && input[1]=='g' && input[2]==' ' && input[3]=='%' && strlen(input)>=5){
        // Extract Job ID
       int jobIntIndex=0;
       char* jobInt = malloc(sizeof(char)*50);
       for(int i=4;i<strlen(input);i++){
            jobInt[jobIntIndex]=input[i];
            jobIntIndex++;
       }
       int jobID=atoi(jobInt);
       free(jobInt);
       //printf("Job ID is: %d\n",jobID);

       //Handle switching to foreground
       int found=0;
        for(int i=0;i<50;i++){
            if(jobTable[i].id==jobID){
                    found=1;
                    int currentPID = jobTable[i].pid;
                    //If job is stopped bring to foreground
                    //WORKING
                    if(jobTable[i].jobStatus=="Stopped"){
                        kill(currentPID,SIGCONT);
                        jobTable[i].inBg=false;
                        //sleep(1);
                        handleForkTermination(currentPID);
                        break;
                    }
                    //If job is running bring to foreground
                    //WORKING
                    else{
                        jobTable[i].inBg=false;
                        handleForkTermination(currentPID);
                        break;
                    }
            }
        }

        if(found==1){
            return 0;
        }
        printf("No such job\n");
        //Error 20 no jobs found
        return 20;
    }
    //COMMAND: bg
    //continues stopped background job
    else if(input[0]=='b' && input[1]=='g' && input[2]==' ' && input[3]=='%' && strlen(input)>=5){
        // Extract Job ID
        int jobIntIndex=0;
       char* jobInt = malloc(sizeof(char)*50);
       for(int i=4;i<strlen(input);i++){
            jobInt[jobIntIndex]=input[i];
            jobIntIndex++;
       }
       int jobID=atoi(jobInt);
       free(jobInt);


       //Search for job
       int found=0;
        for(int i=0;i<50;i++){
            if(jobTable[i].id==jobID){
                    found=1;
                    int currentPID = jobTable[i].pid;
                    if(jobTable[i].jobStatus=="Stopped"){
                        //If job is stopped, continue it in the background
                        kill(currentPID,SIGCONT);
                        jobTable[i].jobStatus="Running";
                        printf("[%d] %s &\n",jobTable[i].id,jobTable[i].shellCommand);
                        break;
                    }

            }
        }

        if(found==1){
            return 0;
        }
        //Error 20 no jobs found
        printf("No such job\n");
        return 20;
    }
    //COMMAND: jobs
    //Show all jobs
    else if(input[0]=='j' && input[1]=='o' && input[2]=='b' && input[3]=='s'){
            for(int i=0;i<50;i++){
                if(jobTable[i].id != -99){
                    printf("[%d]  %d  %s        %s\n",jobTable[i].id,jobTable[i].pid,jobTable[i].jobStatus,jobTable[i].shellCommand);
                }
            }
        return 0;
    }
    //COMMAND: echo $?
    //Prints previous exit code
    else if(input[0]=='e' && input[1]=='c' && input[2]=='h' && input[3]=='o' && input[4]==' ' && input[5]=='$' && input[6]=='?'){
        //Echo exit code
        if(exitCode!=-99){
            printf("%d\n",exitCode);
        }
        else{
            printf("No exit code\n");
        }
        return 0;
    }
    //COMMAND: echo
    //prints text
    else if(input[0]=='e' && input[1]=='c' && input[2]=='h' && input[3]=='o' && input[4]==' '){
        int i=5;
        while(i<strlen(input)){
            printf("%c",input[i]);
            i++;
        }
        printf("\n");

    // Copy to previous command
    if(prevCommand!=NULL){
        memset(prevCommand,0,300);
            int i2=0;
            while(i2<strlen(input)){
                prevCommand[i2]=input[i2];
                i2++;
            }
        }
        return 0;
    }
    //COMMAND: !!
    //Repeats previous command
    else if(input[0]=='!' && input[1]=='!'){
        int i=0;
        while(i<strlen(prevCommand)){
            printf("%c",prevCommand[i]);
            i++;
        }
        printf("\n");
        //Recursively execute previous command
        parseUserInput(prevCommand,NULL,exitCode);

        return 0;
    }
    //COMMAND: exit
    //exits the shell
    else if(input[0]=='e' && input[1]=='x' && input[2]=='i' && input[3]=='t' && input[4]==' ' && strlen(input)>=6){
        char* exitCode = malloc(sizeof(char)*50);
        int i=5;
        int exitIndex=0;
        //Flag to exit
        exitFlag=1;
        while(i<strlen(input)){
            exitCode[exitIndex]=input[i];
            i++;
            exitIndex++;
        }

        // convert to 8bit
        int ec=(unsigned char)atoi(exitCode);
        free(exitCode);
        return ec;
    }
    //EXTERNAL/UNKNOWN COMMAND
    else{
        //Create deep copies of input because makeArgument will change the string

        char inputCopy[200];
        strcpy(inputCopy,input);
        char commandCopy[200];
        strcpy(commandCopy,input);

        //------------IO Redirection area------------
        //Checks for > or <
        int ioInFlag=0;
        int ioOutFlag=0;
        int pipeIndex=-99;
        char* fname = calloc(300,sizeof(char));
        int index2=0;

        for(int i=0;i<strlen(input);i++){
            if(input[i]=='<'){
                ioInFlag=1;
                pipeIndex=i;
            }
            else if(input[i]=='>'){
                ioOutFlag=1;
                pipeIndex=i;
            }
            else if(ioInFlag==1 || ioOutFlag==1){
                if(i==pipeIndex+1){
                    continue;
                }
                else{
                    fname[index2]=input[i];
                    index2++;
                }
            }
        }

        // Strip the input for arguments
        char* strippedInput = calloc(200,sizeof(char));
        if(ioOutFlag==1 || ioInFlag==1){
            for(int i=0;i<strlen(input);i++){
                if(i!=pipeIndex-1){
                    strippedInput[i]=input[i];
                }
                else{
                    break;
                }
            }

        }


        //Save as previous command
        if(prevCommand!=NULL){
        memset(prevCommand,0,300);
            int i2=0;
            while(i2<strlen(input)){
                prevCommand[i2]=input[i2];
                i2++;
            }
        }

        // Strip arguments again if IO is redirected
        char** strippedArg;
        if(ioInFlag==1 || ioOutFlag==1){
            strippedArg = makeArgument(strippedInput);
        }
        char** args = makeArgument(input);

        //--------------------------------------------------


        //Checks if process is run as background
        char** bgArgs;
        int bgStatus=checkIfBackground(inputCopy);
        //If background, strip input for argument and pass down the background flag
        if(bgStatus==1){
            int bgCharacterIndex;
            for(int i=0;i<strlen(inputCopy);i++){
                if(inputCopy[i]=='&'){
                    bgCharacterIndex=i;
                }
            }
            char* strippedBackgroundInput = malloc(sizeof(char)*200);
            for(int i=0;i<strlen(inputCopy);i++){
                if(i!=bgCharacterIndex-1){
                    strippedBackgroundInput[i]=inputCopy[i];
                }
                else{
                    break;
                }
            }


            //Make argument from stripped background input
            bgArgs = makeArgument(strippedBackgroundInput);
            //Run background process
            int returnCode=runProcess(commandCopy,bgArgs,0,0,fname,1);
            free(strippedInput);
            free(bgArgs);
            return returnCode;

        }
        // IO symbols detected, Fork and run the program normally with IO flag
        else if(ioInFlag==1 || ioOutFlag==1)
            {
                int returnCode = runProcess(commandCopy,strippedArg,ioOutFlag,ioInFlag,fname,0);
                 free(strippedInput);
                 free(strippedArg);
                 free(fname);

                return returnCode;
            }
        // Fork and run program normally
        else
            {
                int returnCode = runProcess(commandCopy,args,ioOutFlag,ioInFlag,fname,0);
                 free(strippedInput);
                 free(args);
                return returnCode;
            }
        return 0;

    }
}


int main(int argc, char *argv[])
{
    //Initialize variables
    exitFlag=0;
    isScripted=0;
    int activeStatus = 1;
    char shellInput[300];
    size_t size=0;
    char prevCommand[300];
    printf("Starting IC shell\n");
    char commandLineArg[300];
    int exitCode = 0;
    int realExitCode;
    int i=0;

    //Job Table initialization
    jobTable= malloc(sizeof(Job)*50);
    for(int i=0;i<50;i++){
        jobTable[i].id=-99;
        jobTable[i].pid=-99;
        jobTable[i].jobStatus="";
        jobTable[i].shellCommand="";
        jobTable[i].inBg=false;
    }

    //-------Signal handlers---------
    //Disable stop from pausing shell
    struct sigaction action_stop;
    sigemptyset(&action_stop.sa_mask);
    action_stop.sa_handler=sigStopHandler;
    action_stop.sa_flags=0;
    sigaction(SIGTSTP, &action_stop, NULL);

    //Disable interrupt from closing shell
    struct sigaction action_intrpt;
    sigemptyset(&action_intrpt.sa_mask);
    action_intrpt.sa_handler=sigIntHandler;
    action_intrpt.sa_flags=0;
    sigaction(SIGINT, &action_intrpt, NULL);

    signal(SIGTTOU,SIG_IGN);

    struct sigaction action;
    action.sa_sigaction = ChildHandler;
    sigfillset(&action.sa_mask);
    action.sa_flags=SA_SIGINFO;
    sigaction(SIGCHLD,&action,NULL);

    //------------------------------


    //---------------Script Mode-----------------
    //Read command line argument to identify if files is an argument
    //Script mode
    if(argc>=2){
        // Is in script mode
        isScripted=1;

        FILE *fp;
        char buff[255];

        fp = fopen(argv[1], "r");
        char line[256];

        while(fgets(line,sizeof(line),fp)){
            line[strcspn(line,"\n")]=0;
            int temp = parseUserInput(line,prevCommand,exitCode);
            if(temp>-1){
                exitCode=temp;
            }
        }
        fclose(fp);
    }
    //----------------------------------


    //------------Main loop---------------
    while(activeStatus){
            if(isScripted==1){
                // If in script mode, nullify exit flag for main shell
                isScripted=0;
            }
            if(isScripted==0){
                exitFlag=0;
            }
            printf("icsh $ ");
            //Get user input
            fgets(shellInput,300,stdin);
            shellInput[strlen(shellInput)-1]='\0';

            //Process input
            exitCode = parseUserInput(shellInput,prevCommand,exitCode);

            if(exitFlag==1){
                break;
            }
            memset(shellInput,0,300);
    }
    //----------------------------------

    printf("Exiting \n");
    free(jobTable);
    return exitCode;
}





