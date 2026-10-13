CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
STATIC_FLAGS = -static -static-libgcc -static-libstdc++

TARGET = main.exe
TEST_TARGET = tests.exe
RELEASE_EXE = TerminalGame.exe

DIST_DIR = dist/TerminalGame-Windows-x64
ZIP_NAME = dist/TerminalGame-Windows-x64.zip

HEADERS = IConsole.hpp WindowsConsole.hpp PosixConsole.hpp Map.hpp Snake.hpp Food.hpp Game.hpp PathUtils.hpp

ifeq ($(OS),Windows_NT)
    ifeq ($(shell where sh.exe 2>nul),)
        CLEAN_CMD = cmd /C "if exist $(TARGET) del /Q /F $(TARGET) & if exist $(TEST_TARGET) del /Q /F $(TEST_TARGET) & if exist $(RELEASE_EXE) del /Q /F $(RELEASE_EXE)"
    else
        CLEAN_CMD = rm -rf $(TARGET) $(TEST_TARGET) $(RELEASE_EXE)
    endif
else
    CLEAN_CMD = rm -rf $(TARGET) $(TEST_TARGET) $(RELEASE_EXE)
endif

.PHONY: all test release run clean rebuild

all: $(TARGET)

$(TARGET): main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

$(TEST_TARGET): tests.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) tests.cpp -o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

release: main.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -O2 $(STATIC_FLAGS) main.cpp -o $(RELEASE_EXE)

run: $(TARGET)
	./$(TARGET)

clean:
	-@$(CLEAN_CMD)

rebuild: clean all