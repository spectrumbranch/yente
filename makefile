CC=g++
CFLAGS=-c -Wall -g
LDFLAGS=

LIBDIRS=-L$(LD_LIBRARY_PATH)
LIBS=-lmysqlcppconn

SOURCEDIR=src

SOURCES= $(SOURCEDIR)/DataSource.cpp 
SOURCES+= $(SOURCEDIR)/MySqlDataSource.cpp 
SOURCES+= $(SOURCEDIR)/RandomDataSource.cpp 
SOURCES+= $(SOURCEDIR)/Player.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=yente

all: $(EXECUTABLE)

#$(SOURCES) $(SOURCEDIR)/Matchmaking.cpp $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) -g $(LIBDIRS) $(LDFLAGS) $(OBJECTS) $(SOURCEDIR)/Matchmaking.cpp -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(SOURCEDIR)/*o $(EXECUTABLE) 
