# This is a variable
CC=g++ -std=c++11
ZMQ=/home/utp/Documentos/ClientServer/zmq/
ZMQ_HDRS=$(ZMQ)/include
ZMQ_LIBS=$(ZMQ)/lib 

all: worker  

worker: worker.cc
  $(CC) -I$(ZMQ_HDRS) -c worker.cc
	$(CC) -L$(ZMQ_LIBS) -o worker worker.o -lzmq -lczmq


clean:
	rm -f worker.o client.o server.o client server worker *~ 







