CC = g++
RM = /bin/rm -rf
OBJS = Main.o
LIB = DBSimple.a
TARGET = Main

$(TARGET) : $(LIB) $(OBJS)
	$(CC) -o $@ $^

..cpp.o:
	$(CC) -c $<


clean:
	$(RM) *.o
