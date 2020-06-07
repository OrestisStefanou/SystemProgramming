#include"network.h"
#include"buffer.h"

#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))
#define BACKLOG 10

struct ClientInfo
{
    int new_socket_fd;
    struct sockaddr_in client_address;
    //Put more arguments if needed here
};
typedef struct ClientInfo ClientInfo;

//Get sockaddr,IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa){
    if(sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//Thread function to handle connection to the client
void *serve_client(void *arg);

//Signal handler to handle SIGTERM and SIGINT signals
void signal_handler(int signal_number);

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cvar;    //Condition variable
pthread_t *pthreads;    //Array with thread ids

int number_of_threads = 4;  //Get this value as a command line argument

int socket_fd1,socket_fd2;
struct pollfd *pfds; 

int main(int argc, char const *argv[])
{
    int port,new_socket_fd,err;
    struct sockaddr_in address;
    struct sockaddr_storage client_address;
    socklen_t client_address_len;

    char remoteIP[INET6_ADDRSTRLEN];

    int fd_count = 0;
    int fd_size = 2;
    pfds = malloc(sizeof *pfds * fd_size); //Monitor the 2 listening sockets

    //Check command line arguments
    if (argc < 2)
    {
        printf("Please give port number and buffer size\n");
        exit(EXIT_FAILURE);
    }

    pthreads = malloc(number_of_threads * sizeof(pthread_t));   //Create the array
    //Create file descriptor buffer
    buffer_size = atoi(argv[2]);
    fd_buffer = malloc(buffer_size * sizeof(int));
    //Initialize condition variable
    pthread_cond_init(&cvar,NULL);

    port = atoi(argv[1]);   //Port number to listen

    //Initialize IPv4 address
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    //Initialize the buffer
    buffer_init();

    //Create the threads
    for(int i=0;i<number_of_threads;i++){
        if(err=pthread_create(&pthreads[i],NULL,serve_client,NULL)){
            perror2("pthread_create",err);
            exit(EXIT_FAILURE);
        }
    }

    //Create the first listening socket
    if((socket_fd1 = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //Bind address to socket
    if(bind(socket_fd1,(struct sockaddr *)&address,sizeof(address))==-1){
        perror("bind");
        exit(EXIT_FAILURE);
    }

    //Listen on socket
    if(listen(socket_fd1,BACKLOG)==-1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    /////////////////////////////////////////
    //Initialize IPv4 address
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port+1);;
    address.sin_addr.s_addr = INADDR_ANY;
    //Create the second listening socket
    if((socket_fd2 = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    //Bind address to socket
    if(bind(socket_fd2,(struct sockaddr *)&address,sizeof(address))==-1){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    //Listen on socket
    if(listen(socket_fd2,BACKLOG)==-1){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in sin;
    socklen_t len=sizeof(sin);
    if (getsockname(socket_fd2, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else
        printf("port number %d\n", ntohs(sin.sin_port));
    /////////////////////////////////////////

    //Add the two listening sockets to set
    pfds[0].fd = socket_fd1;
    pfds[0].events = POLLIN;    //Report ready to read on incoming connection
    pfds[1].fd = socket_fd2;
    pfds[1].events = POLLIN;

    fd_count=2;
    
    /* Assign signal handlers to signals. */
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    if (signal(SIGTERM, signal_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("signal");
        exit(1);
    }
    printf("Waiting for connections\n");
    while (1)
    {
        int poll_count = poll(pfds,fd_count,-1);

        if(poll_count == -1){
            perror("poll");
            exit(EXIT_FAILURE);
        }

        //Run through the existing connections looking for data to read
        for(int i=0;i<fd_count;i++){
            //Check if someone is ready to read
            if(pfds[i].revents & POLLIN){
                if(pfds[i].fd == socket_fd1){
                    //Fist listening socket ready to read,handle new connection
                    //Accept connection to client
                    client_address_len = sizeof(struct sockaddr_in);
                    new_socket_fd = accept(socket_fd1, (struct sockaddr *)&client_address, &client_address_len);
                    if (new_socket_fd == -1) {
                        perror("accept");
                        continue;
                    }
                    //Convert IP address from binary to text form 
                    inet_ntop(client_address.ss_family,get_in_addr((struct sockaddr*)&client_address),remoteIP, INET6_ADDRSTRLEN);
                    printf("New connection from:%s\n",remoteIP);
                    //Lock mutex
                    if(err=pthread_mutex_lock(&mtx)){   //Lock mutex
                        perror2("pthread mutex lock",err);
                        exit(EXIT_FAILURE);
                    }
                    while (buffer_is_full())    //If buffer is full wait
                    {
                        pthread_cond_wait(&cvar,&mtx);  //wait for signal
                    }
                    //Entering new socket from connection in the buffer
                    buffer_insert(new_socket_fd);   //Enter the new socket fd in the buffer
                    pthread_cond_signal(&cvar);     //Wake one thread to handle the connection
                    //Unlock the mutex
                    if (err=pthread_mutex_unlock(&mtx))
                    {
                        perror2("pthread mutex unlock\n",err);
                        exit(EXIT_FAILURE);
                    }
                }else
                {
                    //Second listening socket ready to read,handle new connection
                    //Accept connection to client
                    client_address_len = sizeof(struct sockaddr_in);
                    new_socket_fd = accept(socket_fd2, (struct sockaddr *)&client_address, &client_address_len);
                    if (new_socket_fd == -1) {
                        perror("accept");
                        continue;
                    }
                    //Convert IP address from binary to text form 
                    inet_ntop(client_address.ss_family,get_in_addr((struct sockaddr*)&client_address),remoteIP, INET6_ADDRSTRLEN);
                    printf("New connection from:%s\n",remoteIP);
                    //Lock mutex
                    if(err=pthread_mutex_lock(&mtx)){   //Lock mutex
                        perror2("pthread mutex lock",err);
                        exit(EXIT_FAILURE);
                    }
                    while (buffer_is_full())    //If buffer is full wait
                    {
                        pthread_cond_wait(&cvar,&mtx);  //wait for signal
                    }
                    //Entering new socket from connection in the buffer
                    buffer_insert(new_socket_fd);   //Enter the new socket fd in the buffer
                    pthread_cond_signal(&cvar);     //Wake one thread to handle the connection
                    //Unlock the mutex
                    if (err=pthread_mutex_unlock(&mtx))
                    {
                        perror2("pthread mutex unlock\n",err);
                        exit(EXIT_FAILURE);
                    }
                }
                
            }
        }
    }
}


void *serve_client(void *arg){
    int err;
    printf("Thread started\n");
    //printf("Thread trying to lock mutex\n");
    while(1){
        if(err=pthread_mutex_lock(&mtx)){   //Lock mutex
            perror2("pthread mutex lock",err);
            exit(EXIT_FAILURE);
        }
        //printf("Serve client thread locked the mutex\n");
        while (buffer_is_empty())
        {
            pthread_cond_wait(&cvar,&mtx);  //If buffer is empty wait for signal
        }
        //Find the socket in the buffer
        printf("Thread handling the connection\n");
        int new_socket_fd = buffer_get();   //Get the file descriptor of the new connection
        //Unlock the mutex
        if (err=pthread_mutex_unlock(&mtx))
        {
            perror2("pthread mutex unlock\n",err);
            exit(EXIT_FAILURE);
        }
        /* TODO: Put client interaction code here. For example, use
        * write(new_socket_fd,,) and read(new_socket_fd,,) to send and receive
        * messages with the client.
        */
        //char buffer[100];
        //strcpy(buffer,"Hello from server\n");
        //write(new_socket_fd,buffer,sizeof(buffer)); //Sent something to client
        //memset(buffer,0,100);
        //read(new_socket_fd,buffer,sizeof(buffer));  //Receive something from client
        //printf("%s\n",buffer);
        char buffer[100];
        read(new_socket_fd,buffer,sizeof(buffer));
        if(strcmp(buffer,"port")==0){
            int worker_port;
            read(new_socket_fd,&worker_port,sizeof(worker_port));
            printf("Worker is listening on port %d\n",worker_port);
        }
        close(new_socket_fd);
    }
}

void signal_handler(int signal_number) {
    int err;
    printf("Caught the signal\n");
    //Terminate all threads
    for(int i=0;i<number_of_threads;i++)
        pthread_kill(pthreads[i],SIGKILL);
    free(pthreads);
    //Destroy condition variable
    if(err = pthread_cond_destroy(&cvar)){
        perror2("pthread_cond_destroy",err);
        exit(EXIT_FAILURE);
    }
    free(pfds);
    //printf("Coming here\n");
    close(socket_fd1);
    close(socket_fd2);
    exit(0);
}