CXX = /usr/bin/g++
CXXFLAGS = $(COMPILE_OPT)-I -Wall -I../MySqlLib -I../Helper  `mysql_config  --cflags`
CXXLFLAGS = $(COMPILE_OPT)-Wall -lrt  -L../Helper -L../MySqlLib -lhelper -lmysqlczf `mysql_config --libs_r` -static

OBJS = Main.o

LIB_TAR = ../Helper/libhelper.a \
		  ../MySqlLib/libmysqlczf.a

SRCS = $(OBJS:%.o=%.cpp)
DEPS = $(OBJS:%.o=.%.d)
TARGET = Main


all : $(TARGET)

include $(DEPS)
$(DEPS): .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(TARGET): $(OBJS) $(LIB_TAR)
	$(CXX) $(OBJS) -o $@ $(CXXLFLAGS)


distclean:
	@find . -iname .\*.d -exec rm \{\} \;
	@find . -iname .\*.d.* -exec rm \{\} \;


clean:
	@rm -rf $(OBJS) $(LIB_TAR) core *~
