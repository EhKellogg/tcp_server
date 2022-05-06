#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <vector>

//removes trailing characters from a char array
void chomp(char *s)
{
	for (char *p = s + strlen(s)-1; *p == '\r' || *p == '\n'; p--)
		*p = '\0';
}

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		std::cout << "not enough arguments" << std::endl;
		exit(EXIT_FAILURE);
	}
	char buffer[256];
	int sock, newSock;
	socklen_t serverlen, clientlen;
	ssize_t received;
	struct sockaddr_in echoserver;
	struct sockaddr_in echoclient;
	std::string temp;
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
	std::vector<std::string> files; //vector to hold the names of files in the folder

	//loop that runs the server and waits for a client to connect 
	// when connected it accepts the commands from the client
	// creates a fork and closes the socket to continue waiting for more clients
	// the child process looks for an argument and opens the directory given by the server
	// checks to see if the file specified by client exists and if it does it should send the file (wasn't able to get that figured out)
	// if only GET / is sent then just prints out the names of the files
	while((newSock = accept(sock, (struct sockaddr *) &echoclient, &clientlen)) != -1)
	{
		if(fork())
		{	
			close(newSock);
		}
		else
		{
			std::cerr << "Client connected: " << inet_ntoa(echoclient.sin_addr) << "\n";
			if((received = read(newSock, buffer, 256)) == -1)
			{
				perror("read failed");
				exit(2);
			}

			DIR *dirp;
				struct dirent *dirEntry;
				
				dirp = opendir(argv[2]);
				if(dirp == 0)
				{
					perror(argv[2]);
					exit(EXIT_FAILURE);
				}
			while((dirEntry = readdir(dirp)) != NULL)
				{
					if(dirEntry->d_name[0] != '.')
					{
						files.push_back(dirEntry->d_name);
					}	
				}
			std::string get = "GET /";
			chomp(buffer);
			int compare = strcmp(get.c_str(), buffer);
			if(compare == 0)
			{
			
				
				for(auto x : files)
				{
					write(newSock, x.c_str(), x.length());
					write(newSock, " ", 1);	
				}
				write(newSock, "\n", 1);
			}
			for(int i = 0; i < files.size(); ++i)
			{	
				if(strncmp(files[i].c_str(), buffer, 0) == 0)
				{
					write(newSock, files[i].c_str(), files[i].length());
				}
			}
			std::cout << files[0] << std::endl;
			write(newSock, "\n", 1);
		
			closedir(dirp);
			close(newSock);
			exit(0);
		}
		
	}
	
	return 0;
}
