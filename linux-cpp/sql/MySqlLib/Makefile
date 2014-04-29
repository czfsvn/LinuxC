CXX = /usr/bin/g++
#CXXFLAGS = -Wall -I../Helper -D_GNU_SOURCE -D_REENTRANT `mysql_config --cflags`
CXXFLAGS = -Wall -I../Helper  `mysql_config --cflags`
CXXLFLAGS = -Wall -L../Helper -lhelper `mysql_config --libs_r`

OBJS = MysqlCon.o Mysqlres.o

LIB_TAR = libmysqlczf.a
DYN_LIB_TAR = libmysqlczf.so
SRCS = $(OBJS:%.o=%.cpp)
DEPS = $(OBJS:%.o=.%.d)

all : $(LIB_TAR)

include $(DEPS)
$(DEPS): .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; rm -f $@.$$$$

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS)  $< -o $@

$(LIB_TAR): $(OBJS)
	@rm -rf $(LIB_TAR)
	@ar cru $(LIB_TAR) $(OBJS)
	@ranlib $(LIB_TAR)
	@if [ -n "$(PIC_FLAG)" ]; then \
		$(CXX) -shared -o $(DYN_LIB_TAR) $(OBJS); \
		fi

clean:
	@rm -rf $(OBJS) $(LIB_TAR) core *~
