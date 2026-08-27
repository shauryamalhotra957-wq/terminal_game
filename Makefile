CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = main.exe
SOURCE = main.cpp

.PHONY: all run clean rebuild

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

rebuild: clean all