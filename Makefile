PROJECT = tpush

CC = g++
RM = rm -rf
ECHO = echo

EV_INC = -I/usr/local/include -I.libev/include
EV_LIB = -L/usr/local/lib -I.libev/lib

CCFLAGS = -O2 -Wall $(EV_INC)

LDFLAGS = $(EV_LIB) -lev

CPP_OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
C_OBJS = $(patsubst %.c, %.o, $(wildcard *.c))

.PHONE: all clean

$(PROJECT) : $(CPP_OBJS) $(C_OBJS)
	$(CC) -o $@ $(CPP_OBJS) $(C_OBJS) $(CCFLAGS) $(LDFLAGS)

%.o : %.cpp
	$(CC) -o $@ -c $< $(CCFLAGS)


%.o : %.c
	$(CC) -o $@ -c $< $(CCFLAGS)

clean:
	$(RM) *.o $(PROJECT) 

