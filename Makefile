Q:=@
CPP=g++
LD=g++
CPPFLAGS =--std=c++11 -g -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"
LDFLAGS=-L/usr/lib/x86_64-linux-gnu
OUTDIR=bin
LIBS=-lstb

EXE = $(OUTDIR)/nn
SRCDIR=src

SRCS = $(SRCDIR)/main.cpp \
	   $(SRCDIR)/Network.cpp \
	   $(SRCDIR)/NetworkLayer.cpp \
	   $(SRCDIR)/NetworkNode.cpp \
	   $(SRCDIR)/TrainingData.cpp \

CPPFLAGS+=-I./ -I/usr/include/

default: $(EXE)

OBJS += $(patsubst %.cpp, $(OUTDIR)/%.o, $(notdir $(SRCS)))
DEPS += $(patsubst %.cpp, $(OUTDIR)/%.d, $(notdir $(SRCS)))

$(OUTDIR)/%.o: $(SRCDIR)/%.cpp
	@echo 'Building file $<'
	@mkdir -p $(OUTDIR)
	$(Q)$(CPP) $(CPPFLAGS) -c -o $@ $<

$(EXE): $(OBJS)
	@echo 'Linking $@'
	$(Q)$(LD) $(LDFLAGS) $^ $(LIBS) -o $@

-include $(DEPS)

clean:
	rm -f $(EXE) $(OBJS) $(DEPS)