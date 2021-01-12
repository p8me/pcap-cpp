# General purpose C/C++ Makefile
# Created by Pooya Merat on 2018-04-10.

CXX      = g++
CC       = gcc

# name of the executable
PROG_NAME := exec_name.run

SRC_DIRS = .
OBJDIR = obj

SRCS := $(shell find $(SRC_DIRS) -maxdepth 1 -type f -regex ".*\.cpp" -o -regex ".*\.c")
_OBJS := $(addsuffix .o,$(basename $(SRCS)))
OBJS := $(patsubst ./%,$(OBJDIR)/%,$(_OBJS))

# Libraries: -L and -l have to be in order for ubuntu
LDFLAGS += -L/usr/local/lib -lpcap
LDFLAGS += -L/usr/local/lib -lpthread
LDFLAGS += -lzmqpp -lzmq

CXXFLAGS 	+= -Wall -std=c++11
CCFLAGS 	+= -Wall -Wextra
CCFLAGS 	+= -std=gnu99 # Disable if not needed
CCFLAGS 	+= -O3

.PHONY: clean

$(PROG_NAME): directories $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

directories:
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	@- $(RM) $(PROG_NAME)
	@- $(RM) -rf $(OBJDIR)

# Print for debugging
print-%  : ; @echo $* = $($*)
