PROJECT = tpush

CC = g++
RM = rm -rf
ECHO = echo
MKDIR = mkdir -p

EV_INC = -I/usr/local/include -I.libev/include
EV_LIB = -L/usr/local/lib -I.libev/lib

CCFLAGS = -O2 -Wall $(EV_INC)

LDFLAGS = $(EV_LIB) -lev -lpthread -lrt

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

