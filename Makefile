EXE = nn
SRCDIR=./src
INCDIR=./src

CPP=g++
CPPFLAGS=-I$(INCDIR) -std=c++11 -g

SRCS = $(SRCDIR)/main.cpp \
	   $(SRCDIR)/Network.cpp \
	   $(SRCDIR)/NetworkLayer.cpp \
	   $(SRCDIR)/NetworkNode.cpp \
	   $(SRCDIR)/TrainingData.cpp \

INCS = $(INCDIR)/Network.h \
       $(INCDIR)/NetworkLayer.h \
	   $(INCDIR)/NetworkNode.h \
	   $(INCDIR)/TrainingData.h \
	   $(INCDIR)/json.hpp \

LIBS = -lstb

default: $(EXE)

$(EXE): $(SRCS) $(INCS)
	$(CPP) $(CPPFLAGS) $(SRCS) $(LIBS) -o $(EXE)

clean:
	rm -f $(EXE)