#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


using namespace std;
int main(int argc, char *argv[])
{
    //using input redirection we grab the size of the ring and store it in procNum to be passed to the client server upoon request
    int procNum;
    cin >> procNum;
    vector<int> procAccess;
    int procType;
    int emptyVal;
    //here we read in the rest of the process access numbers and store it into a vector
    while(cin >> procType)
    {
        procAccess.push_back(procType);
    }
    //creating a socket to allow for clients to connect to the server. Edge cases cover any fails in the connection. This is an implementation of the example code given to us in on BB.
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        cout << "ERROR, no port provided" << '\n';
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        cout << "ERROR opening socket" << '\n';
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if ((::bind(sockfd, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr))) < 0)
        cout << "ERROR on binding" << '\n';
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
    if (newsockfd < 0)
        cout << "ERROR on accept" << '\n';
    n = read(newsockfd,&emptyVal,sizeof(int));
    if (n < 0) cout<< "ERROR reading from socket" <<'\n';
    n = write(newsockfd, &procNum, sizeof(procNum));
    close(newsockfd);
    int vecRef = 0;
    while(true)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
        if (newsockfd < 0)
            cout << "ERROR on accept" << '\n';
        bzero(buffer,256);
        n = read(newsockfd,&emptyVal,sizeof(int));
        if (n < 0) cout<< "ERROR reading from socket" <<'\n';
        cout << buffer << '\n';
        n = write(newsockfd, &procAccess[vecRef], sizeof(int));
        close(newsockfd);
        if (procAccess[vecRef] == -1)
            break;
        vecRef++;
      }
    close(sockfd);
    return 0;

    
}
