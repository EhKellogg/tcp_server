#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/stat.h>

int main()
{
	while(true)
	{
		connSock = accept(sock, (struct sockaddr *) &echoclient, &clientlen);
		if(connSock < 0)
		{
			perror("accept failed");
			exit(EXIT_FAILURE);
		}
		if(fork())
			close(connSock);
		else
		{
			if((recieved = read(connSock, buffer, 256)) < 0)
			{
				perror("Failed to recieve message");
				exit(EXIT_FAILURE);
			}
			buffer[received] = '\0';
			cerr << "Client(" << inet_ntoa(echoclient.sin_addr) << ") sent: " << buffer << std::endl;
			if(write(connSock, buffer, received) != received)
			{
				perror("Mismatch in number of echo'd bytes");
				exit(EXIT_FAILURE);
			}
			close(connSock);
			exit(EXIT_SUCCESS);
		}
	}
	close(sock);

	return 0;
}
