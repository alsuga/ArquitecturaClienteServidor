CC= g++ -std=c++11 -lzmq -lczmq -pthread
#ZMQ=/home/utp/Documentos/cs/zmq/
#ZMQ_HDRS=$(ZMQ)/include
#ZMQ_LIBS=$(ZMQ)/lib

all: node tracker

node: node.cpp
	$(CC) node.cpp -o node

tracker: tracker.cpp
	$(CC) tracker.cpp -o tracker

clean:
	rm -f node tracker
