PROG		= amq_main

###

CC 		= gcc
SLEEPYCATLOC	= /usr/local/libdb43
DEBUG		= -g #-D_DEBUG
INCLUDES	= -I$(SLEEPYCATLOC)/include -I$(IBASE)/include -I.
CFLAGS		= -Wall $(DEBUG) $(INCLUDES) #-static
LDFLAGS		= -L$(SLEEPYCATLOC)/lib -L$(IBASE)/lib -ldb -lpthread

###

all:   $(PROG)

###

OBJS		= $(PROG).o     \
                  amq.o \
                  amq_vhost.o                \
                  amq_client.o               \
                  amq_dest.o                 \
                  amq_mesg.o                 \
                  amq_subs.o                 \
                  amq_outq.o                 \


###

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS) $(LDFLAGS)

strip:
	strip -s $(PROG)

clean:
	rm -f $(OBJS) *~ *.core $(PROG)
