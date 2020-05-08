#include"pipe.h"
#include<ctype.h>

char client_fifo[256];  //Client fifo pipe name
char server_fifo[256];  //Server fifo pipe name

void terminate(int sig){
    printf("Got the kill signal\n");
    unlink(client_fifo);
    exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[])
{   
    printf("Worker %d running\n",getpid());
    int server_fifo_fd,client_fifo_fd;
    File_Stats stats_data;
    char request[100];

    //TESTING DATA
    set_date(&stats_data.file_date,1,1,2005);
    strcpy(stats_data.Country,"Cyprus");
    strcpy(stats_data.Disease,"Malazavragka");
    for(int i=0;i<4;i++)
        stats_data.Age_counter[i]=getpid();
    //

    sprintf(server_fifo,SERVER_FIFO_NAME,getpid()); //Create server pipe name

    sprintf(client_fifo,CLIENT_FIFO_NAME,getpid());//Create pipe to read from the server
    if (mkfifo(client_fifo,0777)==-1)
    {
        fprintf(stderr,"Error during mkfifo from client\n");
        exit(EXIT_FAILURE);
    }
    
    client_fifo_fd = open(client_fifo, O_RDONLY);//Wait for server to open it and to send a request
    if (client_fifo_fd!=-1)
    {   //Read request from the server
        while(read(client_fifo_fd,request,sizeof(request))>0){
            printf("Server request is %s",request);
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
    
    server_fifo_fd = open(server_fifo,O_WRONLY);//Open server pipe
    if (server_fifo_fd==-1)
    {
        fprintf(stderr,"No server\n");
        exit(EXIT_FAILURE);
    }
    write(server_fifo_fd, &stats_data, sizeof(stats_data));
    strcpy(stats_data.Country,"Greece");
    write(server_fifo_fd, &stats_data, sizeof(stats_data));
    close(server_fifo_fd);
    unlink(client_fifo);
    exit(EXIT_SUCCESS);
}
