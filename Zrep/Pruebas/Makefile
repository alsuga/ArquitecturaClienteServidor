CC= g++ -std=c++11
#ZMQ=/home/utp/Documentos/cs/zmq/
#ZMQ_HDRS=$(ZMQ)/include
#ZMQ_LIBS=$(ZMQ)/lib

all: server client

server: simple_server.cpp
	#$(CC) -I$(ZMQ_HDRS) -c worker.cc
	#$(CC) -L$(ZMQ_LIBS) -o worker worker.o -lzmq -lczmq
	$(CC) simple_server.cpp -o server -lzmq -lzmqpp

#client server

client: simple_client.cpp
#	$(CC) -I$(ZMQ_HDRS) -c client.c
#	$(CC) -L$(ZMQ_LIBS) -o client client.o -lzmq -lczmq
	$(CC) simple_client.cpp -o client -lzmq -lzmqpp

clean:
	rm -f client server *~
