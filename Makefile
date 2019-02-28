TARGET = cmc
WORKING_DIRECTORY = build
TEST_DIRECTORY = input
CXX = g++

RECENT_GCC_AVAILABLE :=$(shell [ `gcc -dumpversion | cut -c 1` -ge "7" ] && echo "YES" || echo "NO")
EVAL := eval

# We use "EVAL" to choose eval or SCL, which acts like EVAL
# SCL is used if we don't have a recent version of GCC that supports C++17
# https://michigantechit.teamdynamix.com/TDClient/KB/ArticleDet?ID=53139
ifeq "$(RECENT_GCC_AVAILABLE)" "YES"
else ifeq "$(SCL_AVAILABLE)" ""
$(error SCL or a recent version of GCC are not available. Please upgrade your GCC version to 7 or greater.)
else
EVAL := scl enable devtoolset-7
endif

.SILENT: all $(TARGET) $(WORKING_DIRECTORY) $(WORKING_DIRECTORY)/Makefile test
.PHONY: all clean clean_all clean_test test

all: $(TARGET)

$(TARGET): $(WORKING_DIRECTORY) $(WORKING_DIRECTORY)/Makefile
	$(EVAL) "$(MAKE) -C $(WORKING_DIRECTORY) $(TARGET)"
	cp $(WORKING_DIRECTORY)/$(TARGET) $(TARGET)

$(WORKING_DIRECTORY)/Makefile: $(WORKING_DIRECTORY) CMakeLists.txt
	cd $(WORKING_DIRECTORY) && $(EVAL) "cmake -D CMAKE_CXX_COMPILER=$(CXX) -D TARGET=$(TARGET) .."

$(WORKING_DIRECTORY):
	mkdir -p $(WORKING_DIRECTORY)

clean_all: clean clean_test

clean:
	rm -rf $(WORKING_DIRECTORY)
	rm -f $(TARGET)

test: $(TARGET)
	for item in `ls $(TEST_DIRECTORY)/*.cm`; do \
		echo Testing $$item ; \
		./$(TARGET) $$item ; \
	done

clean_test:
	rm -f $(TEST_DIRECTORY)/*.s
