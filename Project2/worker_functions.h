#ifndef WORKER_FUNCTIONS_H_
#define WORKER_FUNCTIONS_H_
#include"pipe.h"
#include"Worker_Data_Structures.h"


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

//Get country name from the directory name
void getCountryFromDir(char *dir,char *c){
    int i=0;
    int j=0;
    while(dir[i]!='/'){
        i++;
    }
    i++;
    while(dir[i]!='/'){
        i++;
    }
    i++;
    while(dir[i]!='\0'){
        c[j] = dir[i];
        i++;
        j++;
    }
    c[j] = '\0';
}

//Send file statistics to the server
void send_file_stats(char *server_fifo,queuenode *requests,struct WorkersDataStructs *myData){
    File_Stats stats_data;
    char directory[100];
    char path[100];
    char country_name[25];
    get_item(&requests,directory);  //Get the directory to handle
    //printf("Directory to handle is %s\n",directory);
    DirListInsert(&myData->directories,directory);  //Insert the directory in the worker's Dir list
    getCountryFromDir(directory,country_name);
    //printf("Country name is %s\n",country_name);
    //Read the directory and insert the files(dates) in the dateslist to sort them
    struct dirent *de;//Pointer to directory entry
    dateListptr dateList=NULL;
    DIR *dr = opendir(directory);
    if(dr==NULL){   //In case of error update the parent and abort
        printf("Could not open the directory\n");
        int server_fifo_fd = open(server_fifo,O_WRONLY);//Open server pipe
        if (server_fifo_fd==-1)
        {
            fprintf(stderr,"No server\n");
            exit(EXIT_FAILURE);
        }
        printf("Error during opening directory\n");
        //Send the stats(empty stats)
        write(server_fifo_fd, &stats_data, sizeof(stats_data));
        close(server_fifo_fd);
        return;
    }
    //READING THE DIRECTORY AND SORTING THE DATES
    while((de=readdir(dr))!=NULL){
        if(de->d_name[0]!='.'){
            date_list_insert(&dateList,de->d_name);     //Insert the filename(date) in the list
        }
    }
    //Open the server pipe to send the stats
    int server_fifo_fd = open(server_fifo,O_WRONLY);//Open server pipe
    if (server_fifo_fd==-1)
    {
        fprintf(stderr,"No server\n");
        exit(EXIT_FAILURE);
    }

    //OPEN THE FILES OF THE DIRECTORY FILL THE DATA STRUCTURES AND CREATE THE STATS TO SEND
    FILE *fp;   //for fopen
    struct patient_record *record;  //To read the records from the files
    char buf[100];  //Buffer to read the records from the file
    struct date_list_node dates;    //Struct to get numeric and string value of Dates(filenames)
    statsListPtr filestats; //Pointer to stats List
    while(dateListPop(&dateList,&dates)){
        //filestats=NULL; //Initialize the pointer to the filestats list
        memset(path,0,100);
        strcpy(path,directory);
        strcat(path,"/");
        strcat(path,dates.stringDate); //Create the path to open the file
        myData->Filenames=add_filetree_node(myData->Filenames,path);    //Add the path to visited Filenames
        //printf("File to open is %s\n",path);
        fp = fopen(path,"r");
        if(fp==NULL){
            printf("Error during opening file\n");
            continue;
        }
        while(fgets(buf,100,fp)!=NULL){ //Read the file line by line
            record=malloc(sizeof(struct patient_record));
            read_record(record,buf);    //Fill the patient record structure
            record->filedate=dates.numericDate; //Patient Record date is the name of the file
            strcpy(record->country,country_name);   //Patient Country is the name of the folder
            if(strcmp(record->status,"EXIT")==0){
                //Check for error
                if(RecordTreesearch(myData->InPatients,record)==0){//Record not found in the "ENTER" patients tree
                    printf("Patient not in 'ENTER' records\n");
                    continue;
                }
                //If not found insert it in the "EXIT" patients tree
                myData->OutPatients=add_Recordtree_node(myData->OutPatients,record);
                //THINK IF YOU HAVE TO INSERT THEM IN THE HASHTABLE HERE
                //DiseaseHashTableInsert(myData->DiseaseHashTable,record,myData->hashtablesize);  //Update the hashtable
                //statsListUpdate(&filestats,record,country_name);    //Update the stats
            }else   //Patient has "ENTER" status
            {
                myData->InPatients=add_Recordtree_node(myData->InPatients,record);//Insert the record in the "ENTER" patients tree
                DiseaseHashTableInsert(myData->DiseaseHashTable,record,myData->hashtablesize);  //Update the hashtable
                statsListUpdate(&filestats,record,country_name);    //Update the stats
            }
        }
        fclose(fp);
        //Get the stats and send them to the server
        File_Stats stats_to_send;
        while(statsListPop(&filestats,&stats_to_send)){
            write(server_fifo_fd, &stats_to_send, sizeof(stats_to_send));    
        }
    }
    closedir(dr);
    close(server_fifo_fd);
    return;
}

#endif /* WORKER_FUNCTIONS_H_ */