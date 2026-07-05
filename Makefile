CPP = g++
CPPFLAGS = -std=c++20 -Wall -lftxui-dom -lftxui-component -lftxui-screen
TARGET = main
SRC = main.cpp

all: build run
build:
	$(CPP) $(SRC) $(CPPFLAGS) -o  $(TARGET)
run:
	./$(TARGET)
clean:
	rm -rf $(TARGET)
