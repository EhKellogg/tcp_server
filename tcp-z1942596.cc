#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

int main(int argc, char *argv[])
{
	if(argc < 2)
		std::cout << "not enogh arguments" << std::endl;
	char buffer[257];
	int sock, newSock;
	socklen_t serverlen, clientlen;
	ssize_t received;
	struct sockaddr_in echoserver;
	struct sockaddr_in echoclient;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}
	
	memset(&echoserver, 0, sizeof(echoserver));
	echoserver.sin_family = AF_INET;
	echoserver.sin_addr.s_addr = INADDR_ANY;
	echoserver.sin_port = htons(atoi(argv[1]));

	serverlen = sizeof(echoserver);
	if(bind(sock, (struct sockaddr *) &echoserver, serverlen) < 0)
	{
		perror("Failed to bind server socket");
		exit(EXIT_FAILURE);
	}
	
	if(listen(sock, 64) < 0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
	
	
	clientlen = sizeof(echoclient);
	while((newSock = accept(sock, (struct sockaddr *) &echoclient, &clientlen)) != -1)
	{
		if(fork())
		{	
			close(newSock);
		}
		else
		{
			//close(0);
			//dup(newSock);
			//close(1);
			//dup(newSock);	
		
			std::cerr << "Client connected: " << inet_ntoa(echoclient.sin_addr) << "\n";
			if((received = read(newSock, buffer, 256)) == -1)
			{
				perror("read failed");
				exit(2);
			}
			if((write(newSock, buffer, received)) != received)
			{
				perror("write mismatch");
				exit(3);
			}
			close(newSock);
			exit(0);
		}
		
	}
	

	return 0;
}
