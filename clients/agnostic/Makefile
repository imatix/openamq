APR_CONFIG	=/home/mato/work/imatix/apr-1.0.1/apr-1-config
APR_CFLAGS	=$(shell $(APR_CONFIG) --cflags)
APR_CPPFLAGS	=$(shell $(APR_CONFIG) --cppflags --includes)
APR_LDFLAGS	=$(shell $(APR_CONFIG) --ldflags)
APR_LDLIBS	=$(shell $(APR_CONFIG) --link-ld --libs)

DEBUG		=-g -O0

CC		=gcc
CFLAGS		=$(DEBUG) #$(APR_CFLAGS)
CPPFLAGS	=$(APR_CPPFLAGS)
LDLIBS		=$(APR_LDLIBS)
LDFLAGS	 	=$(DEBUG) $(APR_LDFLAGS)

all: client client.o amqp_client_api.o

clean:
	$(RM) -f client client.o amqp_client_api.o amqp_client_api.c amqp_client_api.h

client: amqp_client_api.o client.o
	$(CC) $(LDFLAGS) -o $@ $+ $(LDLIBS)

client.o: client.c 
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

amqp_client_api.o: amqp_client_api.c amqp_client_api.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

amqp_client_api.c amqp_client_api.h: amqp_client_api.gsl amq_frames.xml
	gsl -script:amqp_client_api.gsl amq_frames.xml
