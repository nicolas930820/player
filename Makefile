CC=g++ -std=c++11
ZMQ_PATH=/home/nicolas/zmq
ZMQ_INIC=$(ZMQ_PATH)/include
ZMQ_LIB=$(ZMQ_PATH)/lib
LIBS=-lzmq -lzmqpp

all: client server

client:client.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INIC) client.cpp -o client $(LIBS) -lpthread -lsfml-audio

server:server.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INIC) server.cpp -o server $(LIBS)
