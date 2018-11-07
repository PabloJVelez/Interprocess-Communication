#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;
int main(int argc, char *argv[])
{
    /* The following lines of code (18-53) are an implementation of the sample code given to us on BB.
       They allow a connection to be made between the client and the server to retrieve the size of the
       ring */
    
    int sockfd, portno, n,ringSize, procAcess;
    int emptyVal =0;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int childRing;

    char buffer[256];
    if (argc < 3) {
        cout << "ERROR: Missing connection details" << '\n';
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        cout << "ERROR opening socket" << '\n';
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        cout << "ERROR, no such host provided" << '\n';
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        cout << "ERROR connecting" << '\n';
    n = write(sockfd,&emptyVal,sizeof(int));
    if (n < 0)
        cout << "ERROR writing to socket";
    n = read(sockfd,&ringSize,sizeof(ringSize));
    if (n < 0)
        cout << "ERROR reading from socket";
    close(sockfd);
    ringSize = ringSize -1;
    // from here we are creating the pipes and using the child processes after calling fork to run n-1 processes where n is the ring size allowing the parent process to run the first process
    int pipefd[ringSize+ 1][2];
    int pid;
    int i;
    int token;
    
    for (i = 0; i <= ringSize; i++){
        if (pipe(pipefd[i]) < 0)
            {
                cout << "ERROR: Pipe Creation failed" << '\n';
                exit(1);
            }
    }

    for (i = 0; i < ringSize ; i++){
        
        if ((pid=fork())==0)
        {
            while(true){
 
                read(pipefd[i][0], &token, sizeof(token) );
                if (token == -1)
                {
                    token = -1;
                    write(pipefd[i+1][1],&token,sizeof(token));
                    _exit(0);
                }
                sockfd = socket(AF_INET, SOCK_STREAM, 0);
                if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
                    cout << "ERROR connecting" << '\n';
                n = write(sockfd,&emptyVal,sizeof(int));
                if (n < 0)
                    cout << "ERROR writing to socket";
                n = read(sockfd,&procAcess,sizeof(int));
                close(sockfd);
                if (n < 0)
                    cout << "ERROR reading from socket";
                if (token >= 0 && procAcess == 1){
                    cout << "Process " << i + 1 << " is using the network" <<'\n';
                }
                if (procAcess==-1)
                {
                    token = -1;
                    write(pipefd[i+1][1],&token,sizeof(token));
                    _exit(0);
                }
                token=token+1;
                write(pipefd[i+1][1],&token,sizeof(token));
                }
            }
        close (pipefd[i][0]);
        close(pipefd[i + 1][1]);
        }
    //parent runs the first process in the ring
    token=0;
    while(1)
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
            cout << "ERROR connecting" << '\n';
        n = write(sockfd,&emptyVal,sizeof(int));
        if (n < 0)
            cout << "ERROR writing to socket";
        n = read(sockfd,&procAcess,sizeof(int));
        close(sockfd);
        if (n < 0)
            cout << "ERROR reading from socket";
        if (procAcess == 1){
            cout << "Process 0 is using the network" <<'\n';
        }
        if (procAcess==-1)
        {
            token = -1;
            write(pipefd[0][1],&token,sizeof(token));
            break;
        }
        token=token+1;
        write(pipefd[0][1],&token, sizeof(int));
        read(pipefd[ringSize][0], &token, sizeof(int));
        if (token == -1)
        {
            token = -1;
            write(pipefd[0][1],&token, sizeof(int));
            break;
        }
    }
    
    for(i=0;i<ringSize;i++)
        wait(NULL);
        
}

