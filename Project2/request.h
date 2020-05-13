#ifndef REQUEST_H_
#define REQUEST_H_
#include"mydate.h"

//DiseaseFrequency data
struct dfData{
    char virusName[25];
    Date entry_date;
    Date exit_date;
    char country[25];
};

//Get the input of the user and fill the dfData structure
//returns -1 in case of error
int fill_dfData(char *buf,struct dfData *data){
    int i=0,j=0;
    char temp_date[5];
    //Skip request command
    while(buf[i]!=' ' && buf[i]!='\n'){
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    i++;

    //Get virus name
    while(buf[i]!=' ' && buf[i]!='\n'){
        data->virusName[j] = buf[i];
        j++;
        i++;
    }
    if(buf[i]=='\n'){
        return -1;
    }
    i++;
    data->virusName[j]='\0';

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
    data->entry_date.day=atoi(temp_date);
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
    data->entry_date.month=atoi(temp_date);
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
    data->entry_date.year=atoi(temp_date);
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
    data->exit_date.day=atoi(temp_date);
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
    data->exit_date.month=atoi(temp_date);
    i++;
    j=0;
    //get year
    while(buf[i]!=' ' && buf[i]!='\n'){
        temp_date[j]=buf[i];
        j++;
        i++;
    }
    temp_date[j]='\0';
    data->exit_date.year=atoi(temp_date);
    if(buf[i]=='\n'){   //No country given
        data->country[0]=0;
        return 0;
    }
    //Get country
    i++;
    j=0;
    while (buf[i]!='\n')
    {
        data->country[j]=buf[i];
        i++;
        j++;
    }
    data->country[j]='\0';
    return 0;
}
#endif /* REQUEST_H_ */