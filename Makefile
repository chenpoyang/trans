####################################################################
# For building: ALL_T
# make version:	GNU Make 3.82
# g++ version:	gcc version 4.5.1
# gcc version:	g++ version 4.5.1
####################################################################

#### === CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT === ####

########## Compiler, tools and options

CC			= gcc
CFLAGS		= -O2 -Wall -g $(DEFINES)
CXXFLAGS	= -O2 -Wall -g $(DEFINES)

# all target
ALL_T		= server client

# all object
CORE_OBJ    = global.o
CLIENT_OBJ  = client.o
SERVER_OBJ  = server.o
ALL_OBJ     = $(CORE_OBJ) $(CLIENT_OBJ) $(SERVER_OBJ)

# Targets start here
all:	$(ALL_OBJ) $(ALL_T)

client: $(CORE_OBJ) $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ -lpthread $(CORE_OBJ) $(CLIENT_OBJ)

server: $(CORE_OBJ) $(SERVER_OBCLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ -lpthread $(CORE_OBJ) $(SERVER_OBJ)

clean:
	$(RM) $(ALL_OBJ) $(ALL_T)

# list targets that do not create files
.PHONY: clean all

# DO NOT DELETE
global.o	: global.c global.h
client.o	: client.c global.h
server.o    : server.c global.h
