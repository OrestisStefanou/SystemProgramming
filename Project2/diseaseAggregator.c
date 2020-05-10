#include"pipe.h"
#include"worker_functions.h"
#include<ctype.h>
#include"Parent_Data_Structures.h"

int main(int argc, char const *argv[])
{
    int server_fifo_fd,client_fifo_fd;
    File_Stats stats_data;
    int read_res;
    char client_fifo[256];
    char server_fifo[256];
    pid_t pid;
    char *args[]={"./worker",NULL};
    int ret;
    int fds[5];     //Array with the file descriptors of the open pipes(useless??)
    pid_t pids[5];    //Array with the pids of the workers

    int num_of_workers=atoi(argv[1]);   //Number of workers

    //Create workers
    for(int i=0;i<3;i++){
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
    printf("Waiting for workers to create their pipes\n");
    sleep(2);

    //Get directories of Country folder
    struct dirent *de;  //Pointer to directory entry
    int dir_counter=0;
    queuenode *dir_queue=NULL;
    DIR *dr = opendir("./Countries");
    if(dr ==NULL){
        printf("Could not open the directory\n");
    }
    while((de = readdir(dr))!=NULL){
        if((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0)){
            //add the directory name to a queue and increase the counter of the dirs
            add_item(&dir_queue,de->d_name);
            dir_counter++;
            //to use later at hashtable creation.Insert dirs in the hashtable
            //printf("%s\n",de->d_name);
        }
    }
    closedir(dr);
    //////////////////////////////////////

    //Fill the Hashtable
    Hashtable_init(dir_counter);
    char dir_name[100];
    while(get_item(&dir_queue,dir_name)){   //Get directories from the queue
        Hashtable_insert(dir_name,0);   //Insert directory(country) to the hashtable
    }
    //

    //Hashtable testing
    //for(int i=0;i<hashtable_size;i++){
    //    printf("Country:%s\n",Hashtable[i].country);
    //}

    //Send a request to all workers with the directories to handle
    char request[100];
    for(int i=0;i<3;i++){
        sprintf(client_fifo,CLIENT_FIFO_NAME,pids[i]);
        client_fifo_fd = open(client_fifo,O_WRONLY);
        if (client_fifo_fd!=-1)
        {
            strcpy(request,"Send me the stats\n");
            write(client_fifo_fd,request,sizeof(request));//Send the request
            strcpy(request,"China/15-02-2005\n");
            write(client_fifo_fd,request,sizeof(request));//Send the request
            close(client_fifo_fd);
            sprintf(server_fifo,SERVER_FIFO_NAME,pids[i]);
            fds[i] = open(server_fifo,O_RDONLY);    //Open the pipe to read from worker
            while(read_res = read(fds[i],&stats_data,sizeof(stats_data))>0){//Get the response
                printf("Worker with pid %d sent:\n",pids[i]);
                File_Stats_Print(&stats_data);
            }
            close(fds[i]);
        }
        else
        {
            printf("Something went wrong with open(server)\n");
            for(int i=0;i<5;i++){
                sprintf(server_fifo,SERVER_FIFO_NAME,pids[i]);
                unlink(server_fifo);
            }
            exit(EXIT_SUCCESS);
        }
        
    }

    //Close file descriptors and delete the pipes,send interrupt signal to workers
    for(int i=0;i<5;i++){
        close(fds[i]);
        sprintf(server_fifo,SERVER_FIFO_NAME,pids[i]);
        unlink(server_fifo);
        kill(pids[i],SIGINT);
    }
    Hashtable_Free();
    exit(EXIT_SUCCESS);
    return 0;
}

