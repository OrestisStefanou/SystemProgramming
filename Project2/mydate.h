#ifndef MYDATE_H_
#define MYDATE_H_
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct myDate{
    int day;
    int month;
    int year;
};

typedef struct myDate Date;

void get_date(Date *date_in_int,char *date_in_char){//Converts date to myDate struct 
    //maybe useless
}

void set_date(Date *date_to_set,int d,int m,int y){//FILL Date structure
    date_to_set->day=d;
    date_to_set->month=m;
    date_to_set->year=y;
}

int check_dates(Date date1,Date date2){//Date11->Entry date,Date2->exit date
    //if date1 is smaller than date 2 return 0,else return 1
    if(date1.year>date2.year){
        return 1;
    }
    if((date1.year == date2.year) && (date1.month>date2.month)){
        return 1;
    }
    if((date1.year == date2.year) && (date1.month == date2.month) && (date1.day>date2.day)){
        return 1;
    }
    return 0;
}

void print_date(Date *date_to_print){
    if(date_to_print->day==100){
        printf("Exit date not given\n");
        return;
    }
    if(date_to_print->day==0){
        printf("Entry date not given\n");
        return;
    }
    printf("%d-%d-%d\n",date_to_print->day,date_to_print->month,date_to_print->year);
}

#endif /* MYDATE_H_ */