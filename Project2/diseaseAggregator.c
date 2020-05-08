#include"pipe.h"
#include<ctype.h>

int main(int argc, char const *argv[])
{
    int server_fifo_fd,client_fifo_fd;
    File_Stats stats_data;
    int read_res;
    char client_fifo[256];
    char server_fifo[256];
    pid_t pid;
    char *args[]={"./client",NULL};
    int ret;
    int fds[5];     //Array with the file descriptors of the open pipes(useless??)
    int pids[5];    //Array with the pids of the workers

    //Create 5 workers
    for(int i=0;i<5;i++){
        pid = fork();
        switch (pid)
        {
        case -1:
            perror("fork failed");
            exit(1);
        //This is the child
        case 0:
            ret = execvp(args[0],args);//Start worker process
            if(ret==-1){
                perror("execvp");
                exit(1);
            }
        //this is the parent
        default:
            printf("I created a worker with pid:%d\n",pid);
            pids[i] = pid;
            sprintf(server_fifo,SERVER_FIFO_NAME,pid);
            mkfifo(server_fifo,0777);   //Create the pipe that we read from the worker
            break;
        }
    }

    sleep(2);//This should change(Wait for workers to create their pipes)

    //Send a request to all workers
    char request[100];
    strcpy(request,"Send me the stats\n");
    for(int i=0;i<5;i++){
        sprintf(client_fifo,CLIENT_FIFO_NAME,pids[i]);
        client_fifo_fd = open(client_fifo,O_WRONLY);
        if (client_fifo_fd!=-1)
        {
            write(client_fifo_fd,request,sizeof(request));//Send the request
            strcpy(request,"Send me the stats again\n");
            write(client_fifo_fd,request,sizeof(request));//Send the request
            close(client_fifo_fd);
            sprintf(server_fifo,SERVER_FIFO_NAME,pids[i]);
            fds[i] = open(server_fifo,O_RDONLY);    //Open the pipe to read from worker
            while(read_res = read(fds[i],&stats_data,sizeof(stats_data))>0){//Get the response
                printf("Worker with pid %d sent:\n",pids[i]);
                File_Stats_Print(&stats_data);
            }
        }
        else
        {
            printf("Something went wrong with open(server)\n");
            for(int i=0;i<5;i++){
                close(fds[i]);
                sprintf(server_fifo,SERVER_FIFO_NAME,pids[i]);
                unlink(server_fifo);
            }
            exit(EXIT_SUCCESS);
        }
        
    }

    //Close file descriptors and delete the pipes
    for(int i=0;i<5;i++){
        close(fds[i]);
        sprintf(server_fifo,SERVER_FIFO_NAME,pids[i]);
        unlink(server_fifo);
    }
    exit(EXIT_SUCCESS);
    return 0;
}

