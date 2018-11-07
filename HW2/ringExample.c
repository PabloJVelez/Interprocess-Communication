#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#define NO_OF_CHILDREN 3


int main()
{
    int pipefd[NO_OF_CHILDREN + 1][2];
    //int input[30]={0,1,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1};
    int input[4]={1,1,1,1};
    int pid;
    int i;
    int token;
    
    for(i = 0; i <= NO_OF_CHILDREN; i++)
    {
        if (pipe(pipefd[i]) < 0)
        {
            printf("Pipe Creation failed\n");
            exit(1);
        }
    }
    
    printf("Pipes created\n");
    
    for(i = 0; i < NO_OF_CHILDREN; i++)
    {
        
        if ((pid=fork())==0)
        {
            while(1)
            {
                read(pipefd[i][0], &token, sizeof(token) );
                
                if (token >= 0 && input[token] == 1)
                    printf("Process %d is sending information\n",i+1);
                if (token == -1 || input[token]==-1)
                {
                    printf("Ending\n");
                    token = -1;
                    write(pipefd[i+1][1],&token,sizeof(token));
                    _exit(0);
                }
                token=token+1;
                write(pipefd[i+1][1],&token,sizeof(token));
            }
            
        }
        //These ends are no longer needed in parent.
        close (pipefd[i][0]);
        close(pipefd[i + 1][1]);
    }
    
    token=0;
    while(1)
    {
        
        //Write to the pipe which is connected to the first child.
        
        token=token+1;
        write(pipefd[0][1],&token, sizeof(int));
        //Read from the pipe, which is attached to the last child created
        read(pipefd[NO_OF_CHILDREN][0], &token, sizeof(int));
        if (token == -1 || input[token] == -1)
        {
            token = -1;
            printf("Parent breaking\n");
            write(pipefd[0][1],&token, sizeof(int));
            break;
        }
        if (input[token]==1)
            printf("Process 0 is sending information\n");
    }
    
    //By this time all other children, would have exited
    //Let's wait to see this
    
    for(i=0;i<NO_OF_CHILDREN;i++)
        wait(NULL);
    
    printf("My %d children terminated! Now I am exiting\n", NO_OF_CHILDREN);
    exit(0);
}
