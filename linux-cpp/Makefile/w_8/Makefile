
#include des/Makefile

#CXX = /usr/bin/g++
CXXFLAGS = $(ZEBRA_COMPILE_OPT) -I. -I../ZT2Share/Code/Command -Wall _D_GNU_SOURCE -D_REENTRANT 'mysql_config --cflags' 'xml2-config --cflags' 'freetype-config --cflags'

CXXLFLAGS = $(ZEBRA_COMPILE_OPT) -Wall -lencdec -lpthread -lrt -lz 'mysql-config --libs_r' 'xml2-config --libs' -llog4cxx -lgd -ldl -LHProtect'freetype-config --libs'

LIB_ZEBRA = libzebra.a
DYN_LIB_ZEBRA = libzebra.so

OBJS =
SRCS = $(OBJS:%.o=%.cpp)
DEPS = $(OBJS:%.o=%.o)

all : $(LIB_ZEBRA)

include $(DEPS)
$(DEPS): .%.d: %.cpp
	@$(CXX) -M $(CXXFLAGS) $< > $@.$$$$; sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@,$$$$ > $@; rm -f $@.$$$$

$(OBJS): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(LIB_ZEBRA): $(OBJS)
	@rm -rf $(LIB_ZEBRA)
	@ar cru $(LIB_ZEBRA) $(OBJS)
	@ranlib $(LIB_ZEBRA)
	@if [ -n "$(PIC_FLAG)"]; then \
		$(CXX) -shared -o $(DYN_LIB_ZEBRA) $(OBJS); \
		mv lib*.so ../Robot; \
	fi

clean:
	@rm -rf *.o $(ALL_TARGET) cor *~
