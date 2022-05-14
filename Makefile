# cc and flags
CC = gcc
CFLAGS = #-g -Wall -I include/

# specific files
CLIENT_FILE = client
SERVER_FILE = server

SRCEXT := c

all: client server

client:
	$(CC) $(CFLAGS) -o $(CLIENT_FILE) -c $(CLIENT_FILE).$(SRCEXT)

server:
	$(CC) $(CFLAGS) -o $(SERVER_FILE) -c $(SERVER_FILE).$(SRCEXT)

clean:
	@rm -rf $(CLIENT_FILE) $(SERVER_FILE)
