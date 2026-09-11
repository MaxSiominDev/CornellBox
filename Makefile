CC      ?= cc
CFLAGS  += -std=c99 -Wall -Wextra -O2 -MMD -MP -I$(SRCDIR) -DGL_SILENCE_DEPRECATION $(EXTRA_CFLAGS)
LDFLAGS += -framework GLUT -framework OpenGL
LDLIBS  += -lm

SRCDIR  := src
BUILD   := build
SRCS    := $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/props/*.c)
OBJS    := $(patsubst $(SRCDIR)/%.c,$(BUILD)/%.o,$(SRCS))
TARGET  := cornellbox

MINGW     := x86_64-w64-mingw32-gcc
FREEGLUT  := third_party/freeglut-win32
WINBUILD  := build-win
WINTARGET := cornellbox.exe
WINOBJS   := $(patsubst $(SRCDIR)/%.c,$(WINBUILD)/%.o,$(SRCS))
WINCFLAGS := -std=c99 -Wall -Wextra -O2 -MMD -MP -I$(SRCDIR) -DFREEGLUT_STATIC -I$(FREEGLUT)/include $(EXTRA_CFLAGS)
WINLIBS   := -L$(FREEGLUT)/lib -lfreeglut_static -lopengl32 -lglu32 -lgdi32 -lwinmm -lm

TESTSRCS := $(filter-out $(SRCDIR)/main.c,$(SRCS)) $(wildcard tests/*.c)
TESTBIN  := $(BUILD)/run-tests

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

windows: $(WINTARGET)

$(WINTARGET): $(WINOBJS)
	$(MINGW) $(WINOBJS) -o $@ -mwindows $(WINLIBS)

$(WINBUILD)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(MINGW) $(WINCFLAGS) -c $< -o $@

test: $(TESTBIN)
	@$(TESTBIN)

$(TESTBIN): $(TESTSRCS) $(wildcard $(SRCDIR)/*.h $(SRCDIR)/props/*.h tests/*.h)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $(TESTSRCS) -o $@ $(LDFLAGS) $(LDLIBS)

dist: $(TARGET) $(WINTARGET)
	tools/make-dist.sh

clean:
	rm -rf $(BUILD) $(WINBUILD) $(TARGET) $(WINTARGET) dist

-include $(OBJS:.o=.d) $(WINOBJS:.o=.d)

.PHONY: all windows test dist clean
