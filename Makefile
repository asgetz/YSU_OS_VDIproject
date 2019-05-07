



################################################################################
# ### C++ source files
# SRC = read.cpp ext2.cpp Virtual.cpp
# SRC_HEADERS = read.h ext2.hpp Virtual.hpp
#
#
# ### Object files (*.o) for all C++ source files
# OBJS = $(SRC:.cpp=.o)
#
# ### Includes
# INCLUDES = -I.
#
# ### Compilers
# CXX = g++
# CC = gcc
#
# ### Compiler flags
# COMMON_FLAGS = -Wall -Werror
# SPECIAL_FLAGS += $(COMMON_FLAGS) -Wpadded -fpack-struct=1
# CFLAGS += $(SPECIAL_FLAGS) -g $(INCLUDES)
# CXX_FLAGS = $(SPECIAL_FLAGS) -g -std=c++11 $(INCLUDES)
#
# ### Link flags
# # LINK_FLAGS
#
# ### Make rule statements
# TARGET = read.exe
# LINKLINE = $(CXX) $(CXX_FLAGS) -o $(TARGET) $^
# # all: clean $(TARGET)
#
# $(TARGET): $(OBJS)
# 	$(LINKLINE)
#
# $(OBJS): $(SRC)
# 	$(CXX) $(CXX_FLAGS) -c $< -o $@
#
# clean:
# 	rm -f *.o *.exe







##################################################
CXX = g++
CXX_FLAGS = -Wall -Wpadded -fpack-struct=1 -g -std=c++11
TARGET = read

all: clean $(TARGET)
$(TARGET): $(TARGET).cpp
	$(CXX) $(CXX_FLAGS) -o $(TARGET) $(TARGET).cpp

clean:
	$(RM) $(TARGET)

run:
	./$(TARGET)
