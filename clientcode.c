//Client Code
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <arpa/inet.h>

#define PORTNUMB 50716 
#define LENGTH 1024

struct DataSent {
	int numOfBytes;
};

int sockID;		//Global variable to disconnect through signal
//Function to send the name of the file once connected
void sendFileName(int socketid, char* fileName);
//Signal handler function to end connection and close the client 
void end_connection();
//Returns a string with the length of the message to be sent and the message istelf, to be stored in the packet we're sending 
const char* getMessage(char* string, int length);
//Sends the contents of the file to the serever
void sendFileContents(int sourceFile, int socketid);
//Sends a disconnect message to the server
void sendDisconnect(int socketid);
//Removes the directory from the file name 
const char* getFileName(char* fileName);

int main(int argc, char* argv[])
{
	signal(SIGINT, end_connection);

	int socketid;
	struct sockaddr_in srv;
	char c, rc;
	int more_data = 1;
	char fileName[60];
	int source_file;
	//If not argument was passed
	if (argv[1] == NULL)
	{
		perror("Please enter the file name you wish to send as a command line argument.\n");
		exit(1);
	}
	strcpy(fileName, argv[1]);
	//Create the Client socket
	if ((socketid = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Socket call failed");
		exit(0);
	}
	sockID = socketid;
	//Connect it to the server's socket
	srv.sin_family = AF_INET;
	srv.sin_port = htons(PORTNUMB);
	srv.sin_addr.s_addr = htonl(INADDR_ANY);

	if (connect(socketid, (struct sockaddr*)&srv, sizeof(srv)) < 0)
	{
		perror("connect call failed");
		exit(0);
	}
	printf("Conected to Server, now opening and sending file name\n");
	//Open the file that was passed as an argument
	source_file = open(fileName, O_RDWR);
	if(source_file < 0)
	{
		printf("Failed to open file.\n");
		exit(1);
	}
	//Remove the path from the file name
	char newName[20];
	strcpy(newName, getFileName(fileName));
	sendFileName(socketid, newName);
	sleep(4);
	//Start sending the contents of the file to the server
	sendFileContents(source_file, socketid);
	//Close the file, end connection
	close(source_file);
	close(socketid);
	return 0;
}
void sendFileName(int socketid, char* fileName)
{
	int length = strlen(fileName);
	int i = 0;
	char packet[LENGTH];
	while (length > 0)
	{
		//Copy the message containing the message length and the message itself into packet
		strcpy(packet, getMessage(fileName, length));
		send(socketid, &packet, LENGTH, 0);
		length -= LENGTH;
		i += 1;
	}
	printf("File name sent: %s\n", fileName);
}
void sendFileContents(int sourceFile, int socketid)
{
	char temp[LENGTH];
	char packet[LENGTH];
	int read_length = 0;
	char current = '\0';
	//Read exactly LENGTH to be sent
	while ((read_length = read(sourceFile, temp, LENGTH)) > 0)
	{
		strcpy(packet, getMessage(temp, read_length + 1));
		int length = strlen(packet);
		send(socketid, &packet, read_length + sizeof(length), 0);
		printf("Message sent....\n");
		//Clear the temp array
		bzero(temp, LENGTH);
	}
}
const char* getFileName(char* fileName)
{
	int i, last=0;
	int length = strlen(fileName);
	char* msg = (char*)malloc(sizeof(char) * 30);
	for (i = 0; i < length; i++)
	{
		if (fileName[i] == '/')
		{
			last = i;
		}
	}
	if (last != 0)
	{
		last += 1;
	}
	for (i = 0; i < length; i++)
	{
		msg[i] = fileName[last];
		last += 1;
	}
	msg[strlen(msg)] = '\0';
	printf("File name in msg %s....\n", msg);
	return msg;
}
void sendDisconnect(int socketid)
{
	char dis[20] = "disconnecting";
	send(socketid, &dis, strlen(dis), 0);
}
void end_connection()
{
	signal(SIGINT, end_connection);
	printf("Ending connection....");
	close(sockID);
	exit(0);
}
const char* getMessage(char* string, int length)
{
	int x, i;
	char* numb = malloc(sizeof(length));
	char* msg = (char*)malloc(sizeof(char) * length + sizeof(numb));
	//Add the length of the string to the msg
	sprintf(numb, "%d", length);
	for (i = 0; i <= strlen(numb); i++)
	{
		msg[i] = numb[i];
		x = i;
	}
	//Then add the string to the packet(msg) to be sent
	for (i = 0; i < length; i++)
	{
		msg[x] = string[i];
		x += 1;
	}
	msg[x] = '\0';
	return msg;
}