CC			= g++
CFLAGS		= -std=c++11
LIBS		= -lboost_filesystem -lboost_system
INCLUDES	= -IC:\MinGW\include
DBG			= -g
SOURCES		= 
OBJECTS		= $(SOURCES:.cpp=.o)

all: main $(SOURCES)

main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) test.cpp $(LIBS) $(INCLUDES) -o test.exe $(DBG)

.cpp.o:
	$(CC) $(CFLAGS) $< $(LIBS) $(INCLUDES) -c -o $@ $(DBG)
