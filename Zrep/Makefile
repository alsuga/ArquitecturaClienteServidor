CC= g++ -std=c++11 -lzmq -lczmq -pthread
#ZMQ=/home/utp/Documentos/cs/zmq/
#ZMQ_HDRS=$(ZMQ)/include
#ZMQ_LIBS=$(ZMQ)/lib

all: worker client server

worker: mserver.cpp
	$(CC) mserver.cpp -o musicserver

client: mclient.cpp
	$(CC) mclient.cpp -o client 

server:  mbroker.cpp
	$(CC) mbroker.cpp -o broker

clean:
	rm -f client broker musicserver *~ *.mp3
