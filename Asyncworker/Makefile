CC= g++ -std=c++11
#ZMQ=/home/utp/Documentos/cs/zmq/
#ZMQ_HDRS=$(ZMQ)/include
#ZMQ_LIBS=$(ZMQ)/lib

all: worker client server

worker: worker.cc
	#$(CC) -I$(ZMQ_HDRS) -c worker.cc
	#$(CC) -L$(ZMQ_LIBS) -o worker worker.o -lzmq -lczmq
	$(CC) worker.cc -o worker -lzmq -lczmq

#client server

client: client.cc
#	$(CC) -I$(ZMQ_HDRS) -c client.c
#	$(CC) -L$(ZMQ_LIBS) -o client client.o -lzmq -lczmq
	$(CC) client.cc -o client -lzmq -lczmq

server:  diserver.cc
#	$(CC) -I$(ZMQ_HDRS) -c server.c
#	$(CC) -L$(ZMQ_LIBS) -o server server.o -lzmq -lczmq
	$(CC) diserver.cc -o server -lzmq -lczmq

clean:
	rm -f client.o server.o worker.o client server worker *~
