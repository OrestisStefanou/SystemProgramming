#include"network.h"

#define BACKLOG 10
#define SERVER_NAME_LEN_MAX 255

//Get sockaddr,IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//Add a new file descriptor to the set
void add_to_pfds(struct pollfd *pfds[],int newfd,int *fd_count,int *fd_size){
    //If we don't have room,add more space in the pfds array
    if(*fd_count == *fd_size){
        *fd_size *=2;    //Double it
        *pfds = realloc(*pfds,sizeof(**pfds) * (*fd_size));
    }
    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; //Check ready_to_read

    (*fd_count)++;
}

//Remove an index from the set
void del_from_pfds(struct pollfd pfds[],int i,int *fd_count){
    //Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];
    (*fd_count)--;
}

int listening_socket;
struct pollfd *pfds;

int main(int argc, char const *argv[])
{   
    //Connection variables 
    int new_socket,err,port;
    struct sockaddr_in address;
    struct sockaddr_storage client_address;
    socklen_t client_address_len;

    char remoteIP[INET6_ADDRSTRLEN];

    int fd_count = 0;
    int fd_size = 5;
    pfds = malloc(sizeof *pfds * fd_size);

    //Initialize IPv4 address
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = 0;
    address.sin_addr.s_addr = INADDR_ANY;

    //Create listening socket
    if((listening_socket = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //Bind address to socket
    if(bind(listening_socket,(struct sockaddr *)&address,sizeof(address))==-1){
        perror("bind");
        exit(EXIT_FAILURE);
    }

    //Listen on socket
    if(listen(listening_socket,BACKLOG)==-1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    //Get the port number the worker is listening
    struct sockaddr_in sin;
    socklen_t len=sizeof(sin);
    if (getsockname(listening_socket, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else{
        port = ntohs(sin.sin_port);
        printf("Worker listening on port number %d\n",port);
    }


    //Tuta logika ena ta valo stin sinartisi pu tha diavazi pu to pipe
    int server_port,socket_fd;
    char server_name[SERVER_NAME_LEN_MAX + 1] = { 0 };
    struct hostent *server_host;
    struct sockaddr_in server_address;

    //Get server host from server name
    strncpy(server_name,"127.0.0.1",SERVER_NAME_LEN_MAX);   //This will be sent by the master
    server_host = gethostbyname(server_name);   
    server_port = 27015;    //Same for this

    /* Initialise IPv4 server address with server host. */
    memset(&server_address, 0, sizeof server_address);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    memcpy(&server_address.sin_addr.s_addr, server_host->h_addr, server_host->h_length);

    /* Create TCP socket. */
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    /* Connect to socket with server address. */
    if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof server_address) == -1) {
		perror("connect");
        exit(1);
	}

    //Send Testing message
    Message test_msg;
    char buffer[100];
    strcpy(buffer,"port");
    write(socket_fd,buffer,sizeof(buffer));
    write(socket_fd,&port,sizeof(int));
    close(socket_fd);
////////////////////////////////////////////////

    //Add the listening socket to set
    pfds[0].fd = listening_socket;
    pfds[0].events = POLLIN;     //Report ready to read on incoming connection

    fd_count=1;     //Because of the listening socket
    close(listening_socket);
    return 0;
}
