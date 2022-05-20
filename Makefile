CC = gcc
CFLAGS  = -g -Wall
INCLUDE_FLAGS = -I include/

default: client server


## COMPILING THE CLIENT
client:  client.o
	$(CC) $(CFLAGS) -o client obj/client.o

client.o:  src/client/client.c
	@mkdir -p obj
	$(CC) $(CFLAGS) -c src/client/client.c -o obj/client.o

## COMPILING THE SERVER
server:  server.o processor.o monitor.o
	$(CC) $(CFLAGS) -o server obj/server.o obj/processor.o obj/monitor.o

server.o:  src/server/server.c include/processor.h include/monitor.h
	@mkdir -p obj
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c src/server/server.c -o obj/server.o

processor.o: src/server/processor/processor.c include/processor.h include/monitor.h
	@mkdir -p obj
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c src/server/processor/processor.c -o obj/processor.o

monitor.o: src/server/monitor/monitor.c include/monitor.h
	@mkdir -p obj
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c src/server/monitor/monitor.c -o obj/monitor.o


## CLEANING THE FILES
clean:
	@rm -rf ./obj/* client server