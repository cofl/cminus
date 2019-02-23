TARGET = cmc
WORKING_DIRECTORY = build
CC = gcc
CXX = g++

.SILENT:
.PHONY: $(WORKING_DIRECTORY) clean docs

$(TARGET): $(WORKING_DIRECTORY) $(WORKING_DIRECTORY)/Makefile
	$(MAKE) -C $(WORKING_DIRECTORY) $(TARGET)

$(WORKING_DIRECTORY)/Makefile: $(WORKING_DIRECTORY) CMakeLists.txt
	cd $(WORKING_DIRECTORY) && cmake -D CMAKE_C_COMPILER=$(CC) -D CMAKE_CXX_COMPILER=$(CXX) -D TARGET=$(TARGET) ..

$(WORKING_DIRECTORY):
	mkdir -p $(WORKING_DIRECTORY)

clean:
	rm -rf $(WORKING_DIRECTORY)
