Just my chess project in C

I used Linux system calls to open sockets between the client and server programs. Mac OSX has the same socket system calls as
Linux so the client and server are able to run on Mac OSX.

I tried to design the client and server in such a way that most of work is done by the server so it would make it hard for
someone to modify the client to cheat the server.

the client is run with the hostname and the port number as arguments
the server is run with the port number as an argument
