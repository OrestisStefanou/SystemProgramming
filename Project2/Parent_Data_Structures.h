#ifndef PARENT_DATA_STRUCTURES_H_
#define PARENT_DATA_STRUCTURES_H_
#include"pipe.h"

//StatsTree
struct FileStatsTreeNode{
    struct File_Statistics fileStats;
    struct FileStatsTreeNode *right;
    struct FileStatsTreeNode *left;
};
typedef struct FileStatsTreeNode *FileStatsTreePtr;

//Insert FileStats in the tree(Sorted by date)
FileStatsTreePtr add_FileStatsTree_node(FileStatsTreePtr p,struct File_Statistics fStats){
    if(p==NULL){//Tree is empty
        p = malloc(sizeof(struct FileStatsTreeNode));
        p->fileStats.file_date=fStats.file_date;
        strcpy(p->fileStats.Country,fStats.Country);
        strcpy(p->fileStats.Disease,fStats.Disease);
        for(int i=0;i<4;i++){
            p->fileStats.Age_counter[i]=fStats.Age_counter[i];
        }
        p->left=NULL;
        p->right=NULL;
    }
    else if(check_dates(fStats.file_date,p->fileStats.file_date)){
        p->right=add_FileStatsTree_node(p->right,fStats);
    }
    else if(check_dates(fStats.file_date,p->fileStats.file_date)==0){
        p->left=add_FileStatsTree_node(p->left,fStats);
    }
    return p;
}

void FileStatsTreePrint(FileStatsTreePtr p){
    if(p!=NULL){
        File_Stats_Print(&p->fileStats);
        FileStatsTreePrint(p->left);
        FileStatsTreePrint(p->right);
    }
}

void freeFileStatsTree(FileStatsTreePtr p){
    if(p==NULL){
        return;
    }
    freeFileStatsTree(p->left);
    freeFileStatsTree(p->right);
    free(p);
}
//////////////////////////////////
//Hash table data structure
struct Hashtable_entry
{
    char country[30];
    pid_t worker_pid;
    //Pointer se root dentrou me tha exi ta statistics tis xoras
};
typedef struct Hashtable_entry htable_entry;    //Each entry of the hashtable is a country with
                                                //the pid of the worker that is handling it and 
                                                //a ptr to a root of a tree with the statistics 
                                                //for every date of the country

htable_entry *Hashtable;
int hashtable_size;

//Initialize hashtable.Size must be the number of the countries we have
void Hashtable_init(int table_size){
    hashtable_size=table_size;
    Hashtable = (htable_entry *)malloc(hashtable_size * sizeof(htable_entry));
    for(int i=0;i<hashtable_size;i++){
        Hashtable[i].country[0]=0;  //To know if is "empty"
        Hashtable[i].worker_pid = 0; //No worker on it yet
        //Hashtable[i].root = NULL
    } 
}

int hashfunction(char *c){
    int i=0;
    int sum=0;
    while (c[i]!='\0')
    {
        sum = sum + c[i];
        i++;
    }
    return sum % hashtable_size;
}

//Insert a country to the hashtable
void Hashtable_insert(char *c,pid_t pid){
    int index = hashfunction(c);
    int first_index = index;
    while(Hashtable[index].country[0]!=0){
        index++;
        if(index==hashtable_size){
            index=0;
        }
        if(index==first_index){
            printf("Hashtable is full\n");
            return ;
        }
    }
    strcpy(Hashtable[index].country,c);
}

//Get the index where country c is
int getHashtable_index(char *c){
    int index = hashfunction(c);
    int first_index = index;
    while (strcmp(Hashtable[index].country,c)!=0)
    {
        index++;
        if(index==hashtable_size){
            index = 0;
        }
        if(index==first_index){ //Country not found
            return -1;
        }
    }
    return index;
}

void Hashtable_Free(){
    //for(int i=0;i<hashtable_size;i++)
        //free the trees root
    free(Hashtable);
}

#endif /* PARENT_DATA_STRUCTURES_H_ */