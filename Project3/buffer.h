#ifndef BUFFER_H_
#define BUFFER_H_
#include<stdio.h>

int *fd_buffer;          //File descriptor buffer
int buffer_size;

//Initialize buffer
void buffer_init(){
    for(int i=0;i<buffer_size;i++){
        fd_buffer[i]=0;
    }
}

//Check if buffer is empty.Returns 1 if buffer is empty else returns 0
int buffer_is_empty(){
    for(int i=0;i<buffer_size;i++){
        if (fd_buffer[i]!=0)
        {
            return 0;
        }
    }
    return 1;
}

//Check if buffer is full.Returns 1 if buffer is full else returns 0
int buffer_is_full(){
    for(int i=0;i<buffer_size;i++){
        if(fd_buffer[i]==0){
            return 0;
        }
    }
    return 1;
}

//Insert a file descriptor in the buffer
void buffer_insert(int fd){
    for(int i=0;i<buffer_size;i++){
        if(fd_buffer[i]==0){
            fd_buffer[i] = fd;
            return;
        }
    }
}

//Get a file descriptor from the buffer
int buffer_get(){
    int fd;
    for (int i = 0; i < buffer_size; i++)
    {
        if(fd_buffer[i]!=0){
            fd = fd_buffer[i];
            fd_buffer[i] = 0;
            return fd;
        }
    }
    
}

#endif /* BUFFER_H_ */