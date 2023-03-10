//SERVER CODE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#define PORTNUMB 50716 
#define LENGTH 1024

//Receives a file name
int receiveFileName(int socketid);
//Different from get message in client code, this one removes the lenght from the received string
const char* getMessage(char* string, int start);
//For receiving data from the client into File.data array, returns the lenght of data received
void receiveFileContent(int socketid, int file_d);
//Returns the disconnect message sent from the client
const char* disconnect(int socketid);
//Returns the index where the message starts
int startIndex(char* string);

int main(int argc, char* argv[])
{
	int socketid;	//Our socket descriptor, would be used to terminate the connection
	int newfd;
	struct sockaddr_in srv;
	struct sockaddr_in cli;
	int cli_len = sizeof(cli);
	int not_done = 1;
	char buffer[1024] = { 0 };
	char c;
	//Create the server socket
	if ((socketid = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Socket call failed");
		exit(0);
	}
	kill(socketid, SIGINT);			//Setup a signal to end the connection

	//Bind the newly created socket to a port
	srv.sin_family = AF_INET;
	srv.sin_port = htons(PORTNUMB);
	srv.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(socketid, (struct sockaddr*)&srv, sizeof(srv)) < 0)
	{
		perror("Failed to find socket.");
		exit(1);
	}
	//Start listening for only 1 client
	if (listen(socketid, 1) < 0)
	{
		perror("Listen call failed");
	}
	//Wait and accept call from client
	newfd = accept(socketid, (struct sockaddr*)&cli, &cli_len);
	if (newfd < 0)
	{
		perror("Accept call failed");
	}
	//Run the server until we get terminate it using SIGINT
	printf("Conected to client\n");
	//receive the file name and create it
	int newFile = receiveFileName(newfd);
	receiveFileContent(newfd, newFile);
	printf("Successuly received contents of file and copied into File\n");
	//Close the file, end connection
	close(newFile);
	close(newfd);
	return 0;
}
int receiveFileName(int socketid)
{
	char recvBuffer[LENGTH];
	char fName[LENGTH];
	int i = 0, not_done=1;
	char clone[20] = "_local_clone";
	int NameLen;
	while (not_done)
	{
		NameLen = strlen(fName);
		if (recv(socketid, &recvBuffer, LENGTH, 0) > 0)
		{
			int recvLen = recvBuffer[0] - '0';
			int start = startIndex(recvBuffer);
			//Get the first index of the recvBuffer, since it contains the length of the string
			if (NameLen == 0)
			{
				strcpy(fName, getMessage(recvBuffer, start));
			}
			else {
				strcat(fName, getMessage(recvBuffer, start));
			}
			//If the length of the receievd data was less than 1024 (LENGTH) end the loop
			if (recvLen < LENGTH)
			{
				not_done = 0;
			}
		}
		else {
			perror("Failed to receive");
			not_done = 0;
		}
	}
	for (i = strlen(recvBuffer)-4; i < strlen(recvBuffer); i++)
	{
		strncat(clone, &recvBuffer[i], 1);
	}
	strcat(fName, clone);
	//Create the file and start receiving its contents from the client
	int newFile = open(fName, O_WRONLY | O_CREAT, 0777);
	if (newFile < 0)
	{
		perror("Cannot create file\n");
	}
	return newFile;
}
void receiveFileContent(int socketid, int file_d)
{
	char recvBuffer[LENGTH];
	char FinalBuffer[LENGTH];
	char temp[10];
	int fileLen = 0, textLen = 0;
	int not_done = 1, recvLen;
	while (not_done)
	{
		recvLen = read(socketid, &recvBuffer, 1024);
		if (recvLen > 0)
		{
			int i = 0, size, x = 0;;
			//Gets the lenght of the text from packet sent
			int start = startIndex(recvBuffer);
			for (i = 0; i < start; i++)
			{
				temp[i] = recvBuffer[i];
			}
			textLen = atoi(temp);
			//Get the first index of the recvBuffer, since it contains the length of the string
			if (fileLen == 0)
			{
				//Copy the data in the buffer one at a time
				for (i = start; i < textLen; i++)
				{
					FinalBuffer[x] = recvBuffer[i];
					x += 1;
					size = x;
				}
				FinalBuffer[x] = '\0';
				fileLen += recvLen;
				//copy the contents from file.data into the created file
				int copy = write(file_d, FinalBuffer, size);
				if (copy < 0)
				{
					perror("An error occured");
				}
			}
			else {
				//Copy the data in the buffer one at a time
				for (i = 0; i < LENGTH; i++)
				{
					if (FinalBuffer[i] = '\0')
						break;
					else
						x += 1;
				}
				for (i = start; i < textLen; i++)
				{
					FinalBuffer[x] = recvBuffer[i];
					x += 1;
					size = x;
				}
				fileLen += recvLen;
				FinalBuffer[x] = '\0';
				//copy the contents into the created file
				int size = strlen(FinalBuffer);
				int copy = write(file_d, FinalBuffer, size);
				if (copy < 0)
				{
					perror("An error occured");
				}
			}
			//Clear the recvBuffer array
			bzero(recvBuffer, LENGTH);
		}
		else if(recvLen < 0) {
			perror("Failed to receive");
		}
		if (recvLen < LENGTH)
		{
			not_done = 0;
		}
	}
}
const char* disconnect(int socketid)
{
	char* msg = (char*)malloc(sizeof(char) * 20);
	char recvBuffer[20];
	int recLen = recv(socketid, &recvBuffer, 20, 0);
	printf("%d\n", recLen);
	if (recLen > 0)
	{
		int i = 0, length = strlen(recvBuffer);
		printf("disconnect() %s\n", recvBuffer);
		for (i = 0; i < length; i++)
		{
			msg[i] = recvBuffer[i];
		}
	}
	else if (recLen < 0)
	{
		perror("Failed to receive");
		exit(1);
	}
	msg[strlen(msg)] = '\0';
	return msg;
}
const char* getMessage(char* string, int start) 
{
	int length = strlen(string);
	char* msg = (char*)malloc(sizeof(char) * length);
	int x = 0, i;
	for (i = start; i < length; i++)
	{
		msg[x] = string[i];
		x += 1;
	}
	msg[x] = '\0';
	return msg;
} 
int startIndex(char* string)
{
	int test = 0, i;
	for (i = 0; i < strlen(string); i++)
	{
		if (isdigit(string[i]))
		{
			test += 1;
			continue;
		}
		break;
	}
	return test;
}