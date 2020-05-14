#ifndef PARENT_FUNCTIONS_H_
#define PARENT_FUNCTIONS_H_
#include<stdio.h>
#include<string.h>
#include"Parent_Data_Structures.h"

//Get the request code of a user request
int get_request_code(char *buf){
    char request[30];
    int i=0;
    while(buf[i]!=' ' && buf[i]!='\n'){
        request[i] = buf[i];
        i++;
    }
    request[i]='\0';
    if (strcmp(request,"/exit")==0){
        return 7;
    }
    if (strcmp(request,"/listCountries")==0){
        return 1;
    }
    if (strcmp(request,"/diseaseFrequency")==0){
        return 2;
    }
    if (strcmp(request,"/topk-AgeRanges")==0){
        return 3;
    }
    if (strcmp(request,"/searchPatientRecord")==0){
        return 4;
    }
    if (strcmp(request,"/numPatientAdmissions")==0){
        return 5;
    }
    if (strcmp(request,"/numPatientDischarges")==0){
        return 6;
    }
    return -1;  //In case of invalid request
}

void topkRanges(char *buf){
    int k=0;
    char country[25];
    char disease[25];
    Date entryDate;
    Date exitDate;
    int i=0,j=0;
    
}

#endif /* PARENT_FUNCTIONS_H_ */