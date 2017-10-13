run: WebServer.o
	gcc -o run WebServer.o

WebServer.o: WebServer.c
	gcc -c WebServer.c