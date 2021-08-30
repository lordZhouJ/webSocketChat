#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4321

#define BACKLOG 1
#define MAXRECVLEN 1024

int main(int argc, char *argv[])
{
    char buf[MAXRECVLEN];
    int listenfd, connectfd;   /* socket descriptors */
    struct sockaddr_in server; /* server's address information */
    struct sockaddr_in client; /* client's address information */
    socklen_t addrlen;
    /* Create TCP socket */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        /* handle exception */
        perror("socket() error. Failed to initiate a socket");
        exit(1);
    }
 
    /* set socket option */
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
	
    /*******************************************************
    **这里提供了两种写法，对应着两种不一样的结果。
    **第一种只接受客户端目的IP是127.0.0.1的连接
	**第二种,无论客户端是不是连接到本server，都可做出回应
	*******************************************************/
	#if 1
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
	#else
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	#endif

	/*bind的是本地IP以及port*/
    if(bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        /* handle exception */
        perror("Bind() error.");
        exit(1);
    }
    
    if(listen(listenfd, BACKLOG) == -1)
    {
        perror("listen() error. \n");
        exit(1);
    }

    addrlen = sizeof(client);
    while(1)
	{
		if((connectfd=accept(listenfd,(struct sockaddr *)&client, &addrlen))==-1)
		{
			perror("accept() error. \n");
			exit(1);
		}

        struct timeval tv;
        gettimeofday(&tv, NULL);
        printf("You got a connection from client's ip %s, port %d at time %ld.%ld\n",inet_ntoa(client.sin_addr),htons(client.sin_port), tv.tv_sec,tv.tv_usec);
        
        int iret = -1;
        while(1)
        {
            iret = recv(connectfd, buf, MAXRECVLEN, 0);
            if(iret>0)
            {
                printf("%s\n", buf);
            }
			else
            {
                close(connectfd);
                break;
            }
            /* print client's ip and port */
            send(connectfd, buf, iret, 0); /* send to the client welcome message */
        }
    }
    close(listenfd); /* close listenfd */
    return 0;
}
