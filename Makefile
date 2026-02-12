CXX      = g++
CXXFLAGS = -Wall -std=c++17
INCLUDES = -ITetrisConsole/source/Tetris \
           -ITetrisConsole/source/Konsole \
           -ITetrisConsole/include
LDFLAGS  =
LDLIBS   = -lole32 -lwinmm

SRCDIR   = TetrisConsole/source
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/Konsole/*.cpp) \
       $(wildcard $(SRCDIR)/Tetris/*.cpp)

OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

TARGET = TetrisConsole.exe

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	@if exist "$(subst /,\,$(BUILDDIR))" rmdir /s /q "$(subst /,\,$(BUILDDIR))"
	@if exist $(TARGET) del $(TARGET)
