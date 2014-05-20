LD_DIR_ROOT = $(PWD)

BUILD_STRING = `date +%Y%m%d`" build"`cat $(BUILD_DIR_ROOT)/.build`
VERSIONSTRING = `if [ $(VERION_STRING) ]; then echo $(VERION_STRING); else echo "0.0.0"; fi`
DEBUG_OPT = -g -Werror -rdynamic $(DEB_OPT)
ZEBRA_DIST_OPT = -g -O2 -Wno-deprecated -fno-strict-aliasing -Werror -rdynamic


SUB_DIRS = Helper MySqlLib SqlTest_1

TEST_SUB_DIRS =

ROLEPARSERS_DIRS =

ZT2SERVER_DIRS =

ZT2SERVER_SUB_DIRS = $(ZT2SERVER_DIRS) $(TEST_SUB_DIRS)

ALL_SUB_DIRS = $(SUB_DIRS) $(TEST_SUB_DIRS)

NJOBS = `if [ "" == "$(NCPUS)" ]; then echo "" ; else echo "-j$(NCPUS)"; fi`
DMUCS_HOST_IP=127.0.0.1
export CXX= /usr/bin/g++

.PHONY : all debug ctags distclean clean

all: debug


debug:
	@for dir in $(SUB_DIRS); \
		do \
		COMPILE_OPT='$(DEBUG_OPT) -DBUILD_STRING=$(BUILD_STRING)' PIC_FLAG='-fPIC' $(MAKE) -C $$dir || exit 1; \
		done


ctags:
	@ctags -R -h ".c.cpp.h" -o tags

distclean:
	@find . -iname .\*.d -exec rm \{\} \;
	@find . -iname .\*.d.* -exec rm \{\} \;

clean:
	@for dir in $(ALL_SUB_DIRS); \
	do \
		$(MAKE) -C $$dir clean; \
		done
