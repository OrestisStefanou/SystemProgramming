#include"pipe.h"
#include"worker_functions.h"
#include<ctype.h>
#include"Parent_Data_Structures.h"
#include"parent_functions.h"

int main(int argc, char const *argv[])
{
    int server_fifo_fd,client_fifo_fd;  //File descriptors of server and client pipes
    File_Stats stats_data;
    int read_res;           //For error checking
    char client_fifo[256];  //To save the client fifo name
    char server_fifo[256];  //To save the server fifo name
    pid_t pid;              //For fork()
    char *args[]={"./worker",NULL}; //For exec()
    int ret;                //For error checking
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
    DIR *dr = opendir("./Countries");   //Here it is going to be the folder name given in cmdline arguments
    if(dr ==NULL){
        printf("Could not open the directory\n");
    }
    while((de = readdir(dr))!=NULL){
        if(de->d_name[0]!='.'){
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
    FileStatsTreePtr StatsTree = NULL;  //Pointer to the tree where we keep all the stats the workers send us
    char request[100];
    dir_counter=0;
    while(dir_counter<hashtable_size){  //Loop until we send all directories
        for(int i=0;i<3;i++){
            sprintf(client_fifo,CLIENT_FIFO_NAME,pids[i]);
            client_fifo_fd = open(client_fifo,O_WRONLY);
            if (client_fifo_fd!=-1)
            {
                strcpy(request,"Send me the stats\n");
                write(client_fifo_fd,request,sizeof(request));//Send the request
                //strcpy(dir_name,Hashtable[dir_counter].country);
                sprintf(dir_name,"%s/%s","./Countries",Hashtable[dir_counter].country);//Create the directory to send the worker
                write(client_fifo_fd,dir_name,sizeof(request));//Send the directory name
                close(client_fifo_fd);
                sprintf(server_fifo,SERVER_FIFO_NAME,pids[i]);
                fds[i] = open(server_fifo,O_RDONLY);    //Open the pipe to read from worker
                while(read_res = read(fds[i],&stats_data,sizeof(stats_data))>0){//Get the stats from the worker
                    StatsTree = add_FileStatsTree_node(StatsTree,stats_data);   //Insert them in the tree
                }
                close(fds[i]);
                Hashtable[dir_counter].worker_pid=pids[i];  //Save the worker's pid in the hashtable
            }
            else    //Something went wrong
            {
                printf("Something went wrong with open(server)\n");
                for(int i=0;i<3;i++){
                    sprintf(server_fifo,SERVER_FIFO_NAME,pids[i]);
                    unlink(server_fifo);
                    kill(pids[i],SIGINT);
                }
                Hashtable_Free();
                exit(EXIT_SUCCESS);
            }
            memset(request,0,100);
            memset(dir_name,0,100);
            dir_counter++;
            if (dir_counter>=hashtable_size)
            {
                break;
            }
            
        }
    }
    printf("The stats are:\n");
    FileStatsTreePrint(StatsTree);
    //Hashtable testing
    //for(int i=0;i<hashtable_size;i++){
    //    printf("Country:%s and pid of worker:%d\n",Hashtable[i].country,Hashtable[i].worker_pid);
    //}

    //Get user's input
    char user_request[100];
    int request_code=0; //Each request will have a code
    while(1){
        fgets(user_request,100,stdin);
        request_code = get_request_code(user_request);
        if(request_code==7){
            break;
        }
        if(request_code==-1){
            printf("Invalid request\n");
            continue;
        }
    }

    //Close file descriptors and delete the pipes,send interrupt signal to workers
    for(int i=0;i<3;i++){
        close(fds[i]);
        sprintf(server_fifo,SERVER_FIFO_NAME,pids[i]);
        unlink(server_fifo);
        kill(pids[i],SIGINT);
    }
    Hashtable_Free();
    freeFileStatsTree(StatsTree);
    exit(EXIT_SUCCESS);
    return 0;
}

