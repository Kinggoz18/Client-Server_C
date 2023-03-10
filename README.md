# Client-Server_C

This repository contains the source code for my COIS3380 Assignment 4. A client-server file sharing program written in C. The repository includes the following files:

clientcode.c: Contains the code for the client application.
servercode.c: Contains the code for the server application.
testfile.txt: A sample file that can be used for testing the file sharing functionality.

## Getting Started
To get started with the application, follow these steps:

1. Compile the server code using a c complier. Then start the server before starting the client.
It will start the server and it will start listening for incoming connections.

2. Compile the client code a C complier. Then Run the client code with the path to the file you want to share as a command line argument:


## Dependencies
The application uses the following standard C libraries:

fcntl.h
unistd.h
stdio.h
stdlib.h
sys/socket.h
netinet/in.h
string.h
ctype.h
signal.h

## Notes
This project was completed as part of a course assignment. Feel free to use this code as a reference, but please do not submit it as your own work.
