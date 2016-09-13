Just my chess project in C.

# How it started

I initially wanted to just mess around with the Linux system calls to learn Linux a bit better.

After making a really basic client and server program that used the Linux system calls I felt inspired to make it into something more.

Mac OSX has the same socket system calls as Linux so the client and server are able to run on Mac OSX. Running it on OSX seems a bit buggy at times, but I am not sure if these bugs are exclusive to running it on OSX.
 
# Design

Knowing that I would be working on this project for a couple of months I tried to design the client and server to make them maintanble. I could have a way better job designing this project, but I feel like I have learned a lot about design through trial and error.

I tried to design the client and server in such a way that most of work is done by the server so it would make it hard for someone to modify the client to cheat the server.

# Execution Instructions

* 'make client' will compile the client and its depenencies
* 'make server' will compile the server and its depenencies
* The client is run with the hostname and the port number as arguments.
* The server is run with the port number as an argument.
 
# To Do
* clean up code
* exit game when one person wins
* more ui changes

