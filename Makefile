.PHONY: all clean

CXX = g++

##Note: $^ all the dependency , $< the first dependency
#CFLAGS = -O3 -std=c++0x
CFLAGS = -g -Wall -std=c++0x
CSRCS     = $(wildcard *.cpp)
COBJS  = $(addsuffix .o, $(basename $(CSRCS)))
EXEC = sta

all: $(EXEC)
	@echo "Finish!"

sta: main.o cirMgr.o
	@echo "> Building $@"
	@$(CXX) $(CFLAGS) $^ -o $@

%.o: %.cpp
	@echo "> compiling: $<"
	@$(CXX) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(COBJS)
	$(RM) $(EXEC)
