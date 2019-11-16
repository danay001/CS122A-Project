CC=g++
CC_FLAGS=-std=c++11 -lwiringPi
EXEC=vac.out
SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

%.o: %.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@ -lwiringPi

clean:
	rm -f $(EXEC) $(OBJECTS)
