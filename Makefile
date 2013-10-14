PROJECT = tpush

CC = g++
RM = rm -rf
ECHO = echo
MKDIR = mkdir -p

UNAME_S = $(shell uname -s)

EV_INC = -I/usr/local/include -I.libev/include
EV_LIB = -L/usr/local/lib -L.libev/lib

DEBUG ?= 0

CCFLAGS = -Wall $(EV_INC)

ifeq ($(DEBUG), 1)
	CCFLAGS += -g -ggdb -DDEBUG
else
	CCFLAGS += -O2 -DNDEBUG
endif

LDFLAGS = $(EV_LIB) -lev -lpthread 

ifeq ($(UNAME_S), Linux)
	CCFLAGS += -D LINUX
	LDFLAGS += -lrt
endif

ifeq ($(UNAME_S), Darwin)
	CCFLAGS += -D OSX
endif


OBJ_DIR = obj

CPP_OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(wildcard *.cpp))
C_OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(wildcard *.c))

OBJS = $(CPP_OBJS) $(C_OBJS)

.PHONE: all clean

$(PROJECT) : $(OBJS)
	$(CC) -o $@ $^ $(CCFLAGS) $(LDFLAGS)

$(OBJS) : | $(OBJ_DIR)

$(OBJ_DIR):
	$(MKDIR) $@

$(OBJ_DIR)/%.o : %.cpp
	$(CC) -o $@ -c $< $(CCFLAGS)

$(OBJ_DIR)/%.o : %.c
	$(CC) -o $@ -c $< $(CCFLAGS)

clean:
	$(RM) *.o $(PROJECT) $(PROJECT).exe $(OBJ_DIR)/* 

