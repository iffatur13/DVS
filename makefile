# generic makefile June 2016
CC = g++
CC_FLAGS = -std=c++11 -pedantic -Wall -Wextra -O2 -D_POSIX_C_SOURCE=1 -D_BSD_SOURCE=1 `pkg-config --cflags opencv`
LN_FLAGS = -lcaer `pkg-config --libs opencv`


#clang++-3.6 -std=c++11 -pedantic -Wall -Wextra -O2 -o visual visual.cc -D_POSIX_C_SOURCE=1 -D_BSD_SOURCE=1 -lcaer
# file names
EXEC = main
SOURCES = $(wildcard *.cc)
OBJECTS = $(SOURCES:.cc=.o)

# main target
all: $(OBJECTS)
	$(CC) $(OBJECTS) $(CC_FLAGS) $(LN_FLAGS) -o $(EXEC)

-include $(OBJECTS:.o:.d)

# obtain object files
%.o: %.cc
	$(CC) -c $(CC_FLAGS) $< -o $@
	$(CC) -M $(CC_FLAGS) $*.cc > $*.d

# note: automatic variables
# $< contains the first dependency file name
# $@ contains the target file name

.PHONY: clean
# clean out the directory but keep the executables with command clean
clean: 
	rm -f $(EXEC) *~ *% *# .#* *.o *.d
