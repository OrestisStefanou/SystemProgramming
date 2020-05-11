#include<stdio.h>
#include<string.h>
#include"worker_functions.h"
#include"Parent_Data_Structures.h"
#include"mydate.h"
#include"dirent.h"
#include"patient.h" 
#include"Worker_Data_Structures.h"

int main(int argc, char const *argv[])
{
    FILE *fp = fopen("./Countries/China/01-01-2020","r");
    if(fp==NULL){
        printf("Error during opening the file\n");
        exit(1);
    }
    char buf[100];
    DiseaseHT table = create_DiseaseHashtable(10);
    struct patient_record *record;
    while(fgets(buf,100,fp)!=NULL){
        record=malloc(sizeof(struct patient_record));
        read_record(record,buf);
        DiseaseHashTableInsert(table,record,10);
    }
    fclose(fp);
    fp = fopen("./Countries/England/10-01-2020","r");
    if(fp==NULL){
        printf("Error during opening the file\n");
        exit(1);
    }
    while(fgets(buf,100,fp)!=NULL){
        record=malloc(sizeof(struct patient_record));
        read_record(record,buf);
        DiseaseHashTableInsert(table,record,10);
    }
    fclose(fp);
    RecordTreeptr root = getDiseaseHTvalue(table,"H1N1",10);
    RecordTreenode_print(root);

    DIR *dr = opendir("./Countries/China");
    struct dirent *de;//Pointer to directory entry
    dateListptr dateList=NULL;
    if(dr==NULL){
        exit(EXIT_FAILURE);
    }
    while((de=readdir(dr))!=NULL){
        if(de->d_name[0]!='.'){
            date_list_insert(&dateList,de->d_name);     //Insert the filename(date) in the list
        }
    }
    datelistPrint(dateList);
    closedir(dr);
    return 0;
}
