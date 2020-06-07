#ifndef NETWORK_H_
#define NETWORK_H_

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include<netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include<arpa/inet.h>
#include<poll.h>

//Message to send through the sockets
struct message{ 
    char type[25];  //Message type
    void *data;     //Data of the message
};
typedef struct message Message;

#endif /* BUFFER_H_ */