EXE = nn
SRCDIR=./src
INCDIR=./src

CPP=g++
CPPFLAGS=-I$(INCDIR) -std=c++11 -g

SRCS = $(SRCDIR)/main.cpp $(SRCDIR)/Network.cpp $(SRCDIR)/NetworkLayer.cpp $(SRCDIR)/NetworkNode.cpp
INCS = $(INCDIR)/Network.h $(INCDIR)/NetworkLayer.h $(INCDIR)/NetworkNode.h $(INCDIR)/json.hpp

default: $(EXE)

$(EXE): $(SRCS) $(INCS)
	$(CPP) $(CPPFLAGS) $(SRCS) -o $(EXE)

clean:
	rm -f $(EXE)