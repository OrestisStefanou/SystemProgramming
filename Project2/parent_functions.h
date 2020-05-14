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

int topkRanges(char *buf){
    int k=0;
    char country[25];
    char disease[25];
    Date entryDate;
    Date exitDate;
    char temp_date[5];
    int i=0,j=0;

    //Skip request command
    while(buf[i]!=' ' && buf[i]!='\n'){
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    i++;

    //Get k
    while(buf[i]!=' ' && buf[i]!='\n'){
        temp_date[j] = buf[i];
        j++;
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    i++;
    temp_date[j]='\0';
    k=atoi(temp_date);

    //Get country
    j=0;
    while(buf[i]!=' ' && buf[i]!='\n'){
        country[j] = buf[i];
        j++;
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    i++;
    country[j]='\0';

    //Get virus
    j=0;
    while(buf[i]!=' ' && buf[i]!='\n'){
        disease[j] = buf[i];
        j++;
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    i++;
    disease[j]='\0';

    //Get enter date
    j=0;
    //get day
    while(buf[i]!='-' && buf[i]!='\n'){
        temp_date[j]=buf[i];
        j++;
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    temp_date[j]='\0';
    entryDate.day=atoi(temp_date);
    i++;
    j=0;
    //get month
    while(buf[i]!='-' && buf[i]!='\n'){
        temp_date[j]=buf[i];
        j++;
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    temp_date[j]='\0';
    entryDate.month=atoi(temp_date);
    i++;
    j=0;
    //get year
    while(buf[i]!=' ' && buf[i]!='\n'){
        temp_date[j]=buf[i];
        j++;
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    temp_date[j]='\0';
    entryDate.year=atoi(temp_date);
    i++;

    //Get exit date
    j=0;
    //get day
    while(buf[i]!='-' && buf[i]!='\n'){
        temp_date[j]=buf[i];
        j++;
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    temp_date[j]='\0';
    exitDate.day=atoi(temp_date);
    i++;
    j=0;
    //get month
    while(buf[i]!='-' && buf[i]!='\n'){
        temp_date[j]=buf[i];
        j++;
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    temp_date[j]='\0';
    exitDate.month=atoi(temp_date);
    i++;
    j=0;
    //get year
    while(buf[i]!='\n'){
        temp_date[j]=buf[i];
        j++;
        i++;
    }
    temp_date[j]='\0';
    exitDate.year=atoi(temp_date);
    printf("K=%d\n",k);
    printf("Country:%s\n",country);
    printf("Disease:%s\n",disease);
    printf("Entry date:");
    print_date(&entryDate);
    printf("Exit date:");
    print_date(&exitDate);
}

#endif /* PARENT_FUNCTIONS_H_ */