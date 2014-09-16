# This is a variable
CC=g++ -std=c++11
<<<<<<< HEAD
ZMQ=/home/utp/Documentos/ClientServer/zmq/
ZMQ_HDRS=$(ZMQ)/include
ZMQ_LIBS=$(ZMQ)/lib 

all: worker  

worker: worker.cc
  $(CC) -I$(ZMQ_HDRS) -c worker.cc
	$(CC) -L$(ZMQ_LIBS) -o worker worker.o -lzmq -lczmq


clean:
	rm -f worker.o client.o server.o client server worker *~ 
=======
ZMQ=/home/utp/Documentos/cs/zmq/
ZMQ_HDRS=$(ZMQ)/include
ZMQ_LIBS=$(ZMQ)/lib 

all: worker

worker: worker.cc
	$(CC) -I$(ZMQ_HDRS) -c worker.cc
	$(CC) -L$(ZMQ_LIBS) -o worker worker.o -lzmq -lczmq

#client server

#client: client.c
#	$(CC) -I$(ZMQ_HDRS) -c client.c
#	$(CC) -L$(ZMQ_LIBS) -o client client.o -lzmq -lczmq
	

#server:  server.c
#	$(CC) -I$(ZMQ_HDRS) -c server.c
#	$(CC) -L$(ZMQ_LIBS) -o server server.o -lzmq -lczmq

clean:
	rm -f client.o server.o worker.o client server worker *~
>>>>>>> 6106ae2c78a487c0f3cc49d547c6fabbd00f76fd







