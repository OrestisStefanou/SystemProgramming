#include<stdio.h>
#include"worker_functions.h"
#include"Parent_Data_Structures.h"
#include"mydate.h"
#include"dirent.h"

int main(int argc, char const *argv[])
{
    Hashtable_init(5);
    Hashtable_insert("Cyprus",10);
    Hashtable_insert("Greece",10);
    Hashtable_insert("France",10);
    Hashtable_insert("Italy",10);
    Hashtable_insert("Spain",10);
    Hashtable_insert("Kokos",10);
    int index;
    index = getHashtable_index("Greece");
    if(index!=-1){
        printf("%s",Hashtable[index].country);
    }
    index = getHashtable_index("Italy");
    if(index!=-1){
        printf("%s",Hashtable[index].country);
    }
    index = getHashtable_index("Cyprus");
    if(index!=-1){
        printf("%s",Hashtable[index].country);
    }
    index = getHashtable_index("Skata");
    if(index!=-1){
        printf("%s",Hashtable[index].country);
    }
    printf("\n\n");
    //TESTING DATE LIST
    dateListptr datelist=NULL;
    struct dirent *de;  //Pointer to directory entry
    DIR *dr = opendir("./Countries/China");
    if(dr ==NULL){
        printf("Could not open the directory\n");
    }
    while((de = readdir(dr))!=NULL){
        if((strcmp(de->d_name,".")!=0) && (strcmp(de->d_name,"..")!=0)){
            printf("Trying to enter %s\n",de->d_name);
            date_list_insert(&datelist,de->d_name);
        }
    }
    datelistPrint(datelist);
    closedir(dr);
    return 0;
}
