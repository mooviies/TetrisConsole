CXX      = g++
CXXFLAGS = -Wall -std=c++17
INCLUDES = -ITetrisConsole/source/Tetris \
           -ITetrisConsole/source/Konsole \
           -ITetrisConsole/include \
           -Ibuild
LDFLAGS  =

SRCDIR   = TetrisConsole/source
BUILDDIR = build

# OS detection
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)

ifeq ($(UNAME_S),Linux)
    LDLIBS   = -lpthread -ldl -lm
    TARGET   = tetris
    PLATFORM_EXCLUDE = %Win32.cpp
    MKDIR_P  = mkdir -p
    RM_RF    = rm -rf
    RM_F     = rm -f
else ifeq ($(UNAME_S),Darwin)
    LDLIBS   = -lpthread -ldl -lm -framework CoreAudio -framework AudioToolbox -framework CoreFoundation
    TARGET   = tetris
    PLATFORM_EXCLUDE = %Win32.cpp
    MKDIR_P  = mkdir -p
    RM_RF    = rm -rf
    RM_F     = rm -f
else
    LDLIBS   = -lole32 -lwinmm
    TARGET   = TetrisConsole.exe
    PLATFORM_EXCLUDE = %Linux.cpp
    MKDIR_P  = @if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
    RM_RF    = @if exist "$(subst /,\,$(1))" rmdir /s /q "$(subst /,\,$(1))"
    RM_F     = @if exist $(1) del $(1)
endif

ALL_SRCS = $(wildcard $(SRCDIR)/Konsole/*.cpp) \
           $(wildcard $(SRCDIR)/Tetris/*.cpp)

SRCS = $(filter-out $(PLATFORM_EXCLUDE),$(ALL_SRCS))

OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))

MEDIA_SRC = $(wildcard TetrisConsole/media/*)
MEDIA_CPP = $(BUILDDIR)/media_data.cpp
MEDIA_H   = $(BUILDDIR)/media_data.h
MEDIA_OBJ = $(BUILDDIR)/media_data.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) $(MEDIA_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(MEDIA_H)
ifneq ($(UNAME_S),Windows)
	@$(MKDIR_P) $(dir $@)
else
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
endif
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(MEDIA_CPP) $(MEDIA_H): $(MEDIA_SRC) scripts/embed_media.py
	@$(MKDIR_P) $(BUILDDIR)
	python3 scripts/embed_media.py

$(MEDIA_OBJ): $(MEDIA_CPP) $(MEDIA_H)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $(MEDIA_CPP)

clean:
ifneq ($(UNAME_S),Windows)
	$(RM_RF) $(BUILDDIR)
	$(RM_F) $(TARGET)
	$(RM_F) media
else
	@if exist "$(subst /,\,$(BUILDDIR))" rmdir /s /q "$(subst /,\,$(BUILDDIR))"
	@if exist $(TARGET) del $(TARGET)
endif
