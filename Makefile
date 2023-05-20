CPP=g++
CPPFLAGS=-I./ -std=c++11 -g

EXE = nn
SRCDIR=src

SRCS = $(SRCDIR)/main.cpp $(SRCDIR)/Network.cpp $(SRCDIR)/NetworkLayer.cpp $(SRCDIR)/NetworkNode.cpp
INCS = $(SRCDIR)/Network.h $(SRCDIR)/NetworkLayer.h $(SRCDIR)/NetworkNode.h $(SRCDIR)/json.hpp

default: $(EXE)

$(EXE): $(SRCS) $(INCS)
	$(CPP) $(CPPFLAGS) $(SRCS) -o $(EXE)

clean:
	rm -f $(EXE)