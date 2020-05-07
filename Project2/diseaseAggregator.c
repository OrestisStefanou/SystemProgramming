#include"client.h"
#include<ctype.h>

int main(int argc, char const *argv[])
{
    int server_fifo_fd,client_fifo_fd;
    struct data_to_pass_st my_data;
    int read_res;
    char client_fifo[256];
    char server_fifo[256];
    char *tmp_char_ptr;
    pid_t pid;
    char *args[]={"./client",NULL};
    int ret;
    int fds[5];


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
            ret = execvp(args[0],args);
            if(ret==-1){
                perror("execvp");
                exit(1);
            }
        //this is the parent
        default:
            printf("I created a worker with pid:%d\n",pid);
            sprintf(server_fifo,SERVER_FIFO_NAME,pid);
            mkfifo(server_fifo,0777);
            fds[i] = open(server_fifo,O_RDONLY);
            break;
        }
    }
    printf("Coming here\n");
    for(int i=0;i<5;i++)
    {
        read_res = read(fds[i],&my_data,sizeof(my_data));
        if(read_res>0){
            tmp_char_ptr = my_data.some_data;
            printf("Data is:%s\n",tmp_char_ptr);
            strcpy(tmp_char_ptr,"Hello");
            sprintf(client_fifo,CLIENT_FIFO_NAME,my_data.client_pid);
            client_fifo_fd = open(client_fifo,O_WRONLY);
            if (client_fifo_fd!=-1)
            {
                write(client_fifo_fd,&my_data,sizeof(my_data));
                close(client_fifo_fd);
            }
        }
    }
    close(server_fifo_fd);
    unlink(SERVER_FIFO_NAME);
    exit(EXIT_SUCCESS);
    return 0;
}

