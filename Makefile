# Compiler
CXX=g++
# Compiler flags
CXXFLAGS=-Wall -g -I include
# Target executable
TARGET=main
# For deleting the target
TARGET_DEL=main.exe
# Source files
SRCS=$(wildcard */*.cpp) $(wildcard *.cpp)
# Object files
OBJSTMP=$(subst .cpp,.o,$(SRCS))
OBJS=$(subst src/,build/,$(OBJSTMP))
OBJDIR=build
SRCDIR=src
# Default rule to build and run the executable
all: $(TARGET) run
# Rule to link object files into the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
# Rule to compile .cpp files into .o files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -std=c++17
# Rule to run the executable
run: $(TARGET)
	./$(TARGET)
# Clean rule to remove generated files
clean:
	del $(TARGET_DEL) $(OBJS)
