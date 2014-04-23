CC = g++
OBJS = main.o interest.o name.o
LIB = libtest.a

main : $(LIB)
	$(CC) -o $@ $^

main.o : main.c interest.h name.h
	$(CC) -c $<

interest.o : interest.c interest.h
	$(CC) -c $<

name.o : name.c name.h
	$(CC) -c $<

$(LIB) : $(OBJS)
	rm -rf $(LIB)
	ar rcv $(LIB) $(OBJS)
	ranlib $(LIB)

clean:
	rm -rf $(OBJS) main

