# location of make.config
SWATCONFIGDIR = ./
# defines SWATDBDIR
include $(SWATCONFIGDIR)/make.config

# path to SwatDB library files
LIBDIR = $(SWATDBDIR)lib/

# paths to include directories
INCLUDES = -I. -I$(SWATDBDIR)include/

# compiler
CC = g++

# compiler flags for test code build
CFLAGS =  -g -Wall #-pthread

# lflags for linking
LFLAGS = -L$(LIBDIR)

# swatdb and other libraries to link in 
LIBS = $(LFLAGS) -lswatdb -lm -pthread -lcrypto -lssl -lUnitTest++

SRCS=  blockNLJ.cpp  indexNLJ.cpp operation.cpp  relopsmgr_joins.cpp \
       join.cpp tupleNLJ.cpp relopsmgr.cpp



# suffix replacement rule
OBJS = $(SRCS:.cpp=.o)

# be very careful to not add any spaces to ends of these
TARGET0 = chkpt
TARGET1 = smalltests
TARGET2 = tnljoin_tests
TARGET3 = bnljoin_tests
TARGET4 = indexjoin_tests
TARGET5 = exception_tests

# generic makefile
.PHONY: clean runtests mkfiles

all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET0) mkfiles

mkfiles:
	./getfiles.sh

$(TARGET0): $(OBJS) $(TARGET0).cpp *.h *.sh
	./mktestconf.sh
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET0) $(TARGET0).cpp $(OBJS) $(LIBS)

$(TARGET1): $(OBJS) $(TARGET1).cpp *.h *.sh
	./mktestconf.sh
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET1) $(TARGET1).cpp $(OBJS) $(LIBS)

$(TARGET2): $(OBJS) $(TARGET2).cpp *.h *.sh
	./mktestconf.sh
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET2) $(TARGET2).cpp $(OBJS) $(LIBS)

$(TARGET3): $(OBJS) $(TARGET3).cpp *.h *.sh
	./mktestconf.sh
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET3) $(TARGET3).cpp $(OBJS) $(LIBS)

$(TARGET4): $(OBJS) $(TARGET4).cpp *.h *.sh
	./mktestconf.sh
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET4) $(TARGET4).cpp $(OBJS) $(LIBS)

$(TARGET5): $(OBJS) $(TARGET5).cpp *.h *.sh
	./mktestconf.sh
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET5) $(TARGET5).cpp $(OBJS) $(LIBS)


# suffix replacement rule using autmatic variables:
# automatic variables: $< is the name of the prerequiste of the rule
# (.cpp file),  and $@ is name of target of the rule (.o file)
.cpp.o: $(SRCS) *.h *.sh
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

runtests: 
	./$(TARGET0)
	sleep 2
	./$(TARGET1)
	sleep 2
	./$(TARGET2)
	sleep 2
	./$(TARGET3)
	sleep 2
	./$(TARGET4)
	sleep 2
	./$(TARGET5)

clean:
	./cleanup.sh
	$(RM) *.o $(TARGET1) $(TARGET2)  $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET0) *.rel
