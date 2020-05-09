#ifndef WORKER_FUNCTIONS_H_
#define WORKER_FUNCTIONS_H_
#include"pipe.h"

//A queue with the requests from the server
struct request_queue_node
{
    char request[100];
    struct request_queue_node *next;
};
typedef struct request_queue_node queuenode;

//Add item to the end of the queue
void add_item(queuenode **qnode,char *r){
    while (*qnode!=NULL)
    {
        qnode = &((*qnode)->next);
    }
    *qnode = (queuenode *)malloc(sizeof(queuenode));
    strcpy((*qnode)->request,r);
    (*qnode)->next = NULL;
}

//Get item from the queue
int get_item(queuenode **qnode,char *r){
    if((*qnode)==NULL){
        return 0;
    }
    queuenode *temp = *qnode;//Save the address of the node
    strcpy(r,(*qnode)->request);   //get the request to return
    *qnode = (*qnode)->next;        //get new head of the queue
    free(temp);                 //free the old head of the queue
    return 1;
}

void print_queue(queuenode *qnode){
    queuenode *temp = qnode;
    while (temp!=NULL)
    {
        printf("Request is %s\n",temp->request);
        temp = temp->next;
    }
    
}

//Send file statistics to the server
void send_file_stats(char *server_fifo,queuenode *requests){
    File_Stats stats_data;
    char request[100];
    while (get_item(&requests,request))  //Get the directories to handle
    {
        printf("Directory to handle is %s\n",request);
        memset(request,0,100);
    }
    
    //TESTING DATA
    set_date(&stats_data.file_date,1,1,2005);
    strcpy(stats_data.Country,"Cyprus");
    strcpy(stats_data.Disease,"Malazavragka");
    for(int i=0;i<4;i++)
        stats_data.Age_counter[i]=getpid();
    //////////////////////
    int server_fifo_fd = open(server_fifo,O_WRONLY);//Open server pipe
    if (server_fifo_fd==-1)
    {
        fprintf(stderr,"No server\n");
        exit(EXIT_FAILURE);
    }
    write(server_fifo_fd, &stats_data, sizeof(stats_data));
    strcpy(stats_data.Country,"Greece");
    write(server_fifo_fd, &stats_data, sizeof(stats_data));
    close(server_fifo_fd);
    printf("Finished with this\n");
    return;
}

#endif /* WORKER_FUNCTIONS_H_ */