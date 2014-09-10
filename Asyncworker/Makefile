# This is a variable
CC=g++ -std=c++11
ZMQ=/home/utp/Documentos/ClientServer/zmq/
ZMQ_HDRS=$(ZMQ)/include
ZMQ_LIBS=$(ZMQ)/lib 

all: client server

client: client.c
	$(CC) -I$(ZMQ_HDRS) -c client.c
	$(CC) -L$(ZMQ_LIBS) -o client client.o -lzmq -lczmq
	

server:  server.c
	$(CC) -I$(ZMQ_HDRS) -c server.c
	$(CC) -L$(ZMQ_LIBS) -o server server.o -lzmq -lczmq

clean:
	rm -f client.o server.o client server *~







