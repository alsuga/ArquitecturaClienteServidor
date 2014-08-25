CC=g++

all: operation

operation:
	$(CC) -o oclient oclient.cc -lzmq
	$(CC) -o osever oserver.cc -lzmq
	
clean:
	rm -f oclient oserver 
