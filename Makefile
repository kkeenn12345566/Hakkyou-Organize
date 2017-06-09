CC			= g++
CFLAGS		= -std=c++11
LIBS		= -L. -lsqlite3 -lboost_filesystem -lboost_system
INCLUDES	= -IC:\MinGW\include
DBG			= -g
SOURCES		= 
OBJECTS		= $(SOURCES:.cpp=.o)

all: main $(SOURCES)

main: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) main.cpp $(LIBS) $(INCLUDES) -o main.exe $(DBG)

sqlite3:
	gcc -c sqlite3.c
	ar crf libsqlite3.a sqlite3.o
	
.cpp.o:
	$(CC) $(CFLAGS) $< $(LIBS) $(INCLUDES) -c -o $@ $(DBG)
