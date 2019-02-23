TARGET = cmc
WORKING_DIRECTORY = build
CC = gcc
CXX = g++
SCL = scl enable devtoolset-7
RECENT_GCC_AVAILABLE :=$([ `gcc -dumpversion | cut -c 1` -ge "7" ] && echo "YES" || echo "NO")
SCL_AVAILABLE :=$(which scl)

.PHONY: $(WORKING_DIRECTORY) clean

ifeq ($(RECENT_GCC_AVAILABLE),YES)
$(TARGET): $(WORKING_DIRECTORY) $(WORKING_DIRECTORY)/Makefile
	$(MAKE) -C $(WORKING_DIRECTORY) $(TARGET)
	cp $(WORKING_DIRECTORY)/$(TARGET) $(TARGET)

$(WORKING_DIRECTORY)/Makefile: $(WORKING_DIRECTORY) CMakeLists.txt
	cd $(WORKING_DIRECTORY) && cmake -D CMAKE_C_COMPILER=$(CC) -D CMAKE_CXX_COMPILER=$(CXX) -D TARGET=$(TARGET) ..
else ifeq ($(SCL_AVAILABLE),)
$(TARGET): $(WORKING_DIRECTORY) $(WORKING_DIRECTORY)/Makefile
	@echo SCL or a recent version of GCC are not available. Please upgrade your GCC version to 7 or greater.

$(WORKING_DIRECTORY)/Makefile: $(WORKING_DIRECTORY) CMakeLists.txt
	@echo SCL or a recent version of GCC are not available. Please upgrade your GCC version to 7 or greater.
else
$(TARGET): $(WORKING_DIRECTORY) $(WORKING_DIRECTORY)/Makefile
	$(SCL) "$(MAKE) -C $(WORKING_DIRECTORY) $(TARGET)"
	cp $(WORKING_DIRECTORY)/$(TARGET) $(TARGET)

$(WORKING_DIRECTORY)/Makefile: $(WORKING_DIRECTORY) CMakeLists.txt
	cd $(WORKING_DIRECTORY) && $(SCL) "cmake -D CMAKE_C_COMPILER=$(CC) -D CMAKE_CXX_COMPILER=$(CXX) -D TARGET=$(TARGET) ..
endif

$(WORKING_DIRECTORY):
	mkdir -p $(WORKING_DIRECTORY)

clean:
	rm -rf $(WORKING_DIRECTORY)
	rm -f $(TARGET)
