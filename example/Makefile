############################################################
# epoll client server example makefile
############################################################

######################################
# Set variable
######################################	
CC	= gcc
INCLUDE = -I../src -I../lib/libutil
CFLAGS = $(INCLUDE) -Wall -fPIC

SERVER_TARGET = epoll-server
SERVER_SRC = epoll-server-example.c

SERVER_OBJ = $(SERVER_SRC:.c=.o)

CLIENT_TARGET = epoll-client
CLIENT_SRC = epoll-client-example.c

CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
	
all: $(SERVER_TARGET) $(CLIENT_TARGET)
######################################
# Compile & Link
# 	Must use \tab key after new line
######################################
$(SERVER_TARGET): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -L../src -L../lib/libutil $(SERVER_OBJ) -o $(SERVER_TARGET) \
	-static -lepoll -lutil -lpthread

$(CLIENT_TARGET): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -L../lib/libutil $(CLIENT_OBJ) -o $(CLIENT_TARGET) \
	-static -lutil
	
######################################
# Clean 
######################################
clean:
	rm -f *.o $(SERVER_TARGET) $(CLIENT_TARGET)
