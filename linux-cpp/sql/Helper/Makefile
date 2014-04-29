CXX = /usr/bin/g++
CXXFLAGS =
CXXLFLAGS =

OBJS = Config.o Log.o TypeDef.o Common.o

LIB_TAR = libhelper.a
DYN_LIB_TAR = libhelper.so
SRCS = $(OBJS:%.o=%.cpp)
DEPS = $(OBJS:%.o=.%.d)

all : $(LIB_TAR)

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

#%.d: %.cpp
include $(DEPS)
$(DEPS): .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$


$(LIB_TAR): $(OBJS)
	@rm -rf $(LIB_TAR)
	@ar cru $(LIB_TAR) $(OBJS)
	@ranlib $(LIB_TAR)
	@if [ -n "$(PIC_FLAG)" ]; then \
		$(CXX) -shared -o $(DYN_LIB_TAR) $(OBJS); \
	fi

clean:
	@rm -rf $(OBJS) $(LIB_TAR) core *~
