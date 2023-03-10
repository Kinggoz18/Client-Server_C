# Client-Server_C

This repository contains the source code for my COIS3380 Assignment 4. A client-server file sharing program written in C. The repository includes the following files:

clientcode.c: Contains the code for the client application.
servercode.c: Contains the code for the server application.
testfile.txt: A sample file that can be used for testing the file sharing functionality.
Getting Started
To get started with the application, follow these steps:

Compile the server code using the following command:

Copy code
gcc servercode.c -o server
Start the server by running the following command:

bash
Copy code
./server
This will start the server and it will start listening for incoming connections.

Compile the client code using the following command:

Copy code
gcc clientcode.c -o client
Run the client code with the path to the file you want to share as a command line argument:

bash
Copy code
./client <path_to_file>
The client will connect to the server and send the specified file to the server.

Dependencies
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
Notes
This project was completed as part of a course assignment. Feel free to use this code as a reference, but please do not submit it as your own work.
