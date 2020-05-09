#include"pipe.h"
#include"worker_functions.h"
#include<ctype.h>

char client_fifo[256];  //Client fifo pipe name
char server_fifo[256];  //Server fifo pipe name
queuenode *request_queue = NULL;    //queue of requests from the server

void terminate(int sig){
    printf("Got the interrupt signal and exiting\n");
    unlink(client_fifo);
    (void) signal(SIGINT, SIG_DFL);
    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{   
    (void) signal(SIGINT,terminate);//Handle interrupt signal from parent
    printf("Worker %d running\n",getpid());
    int server_fifo_fd,client_fifo_fd;
    char request[100];

    sprintf(server_fifo,SERVER_FIFO_NAME,getpid()); //Create server pipe name
    sprintf(client_fifo,CLIENT_FIFO_NAME,getpid());//Create pipe to read from the server
    if (mkfifo(client_fifo,0777)==-1)
    {
        fprintf(stderr,"Error during mkfifo from client\n");
        exit(EXIT_FAILURE);
    }
    
    while(1){
        client_fifo_fd = open(client_fifo, O_RDONLY);//Wait for server to open it and to send a request
        if (client_fifo_fd!=-1)
        {   //Read request from the server
            while(read(client_fifo_fd,request,sizeof(request))>0){
                //printf("Server request is %s",request);
                add_item(&request_queue,request);
                memset(request,0,100);  //Empty the buffer to read next request
            }
            close(client_fifo_fd);
        }
        else
        {
            printf("Something went wrong with open(client)\n");
            unlink(client_fifo);
            exit(EXIT_FAILURE);
        }
        get_item(&request_queue,request);
        if(strcmp(request,"Send me the stats\n")==0){
            send_file_stats(server_fifo,request_queue);//request_queue has the name of the folders to handle
        }
    }
    unlink(client_fifo);
    exit(EXIT_SUCCESS);
}
