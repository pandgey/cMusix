# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lSDL2 -lSDL2_mixer
TARGET = cMusix

# Colors
YELLOW = \033[1;33m
GREEN = \033[1;32m
RED = \033[1;31m
RESET = \033[0m

# OS Detection
UNAME_S := $(shell uname -s)
DISTRO := $(shell grep '^ID=' /etc/os-release 2>/dev/null | cut -d= -f2 | tr -d '"')

# OS-specific compiler flags
ifeq ($(UNAME_S),Linux)
	CFLAGS += -D_GNU_SOURCE
	LIBS += -lm
endif
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -I/usr/local/include -I/opt/homebrew/include
	LIBS += -L/usr/local/lib -L/opt/homebrew/lib -lm
endif
ifeq ($(UNAME_S),FreeBSD)
	CFLAGS += -I/usr/local/include
	LIBS += -L/usr/local/lib -lm
endif
ifeq ($(UNAME_S),OpenBSD)
	CFLAGS += -I/usr/local/include
	LIBS += -L/usr/local/lib -lm
endif
ifeq ($(UNAME_S),NetBSD)
	CFLAGS += -I/usr/pkg/include
	LIBS += -L/usr/pkg/lib -lm
endif

# Directories
SRCDIR = .
OBJDIR = obj
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Targets
.PHONY: all clean install help debug release

all: $(TARGET)
	@echo "$(GREEN)[✓] Build complete$(RESET)"

$(TARGET): $(OBJECTS)
	@echo "$(YELLOW)[*] Linking...$(RESET)"
	$(CC) $(OBJECTS) -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	@echo "$(YELLOW)[*] Compiling $<$(RESET)"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	@echo "$(RED)[x] Cleaning...$(RESET)"
	@rm -rf $(OBJDIR) $(TARGET)

install:
	@echo "$(YELLOW)[*] Detected OS: $(UNAME_S)$(RESET)"
	@echo "$(YELLOW)[*] Detected Distribution: $(DISTRO)$(RESET)"
	@echo "$(GREEN)[*] Installing SDL2 dependencies...$(RESET)"
ifeq ($(UNAME_S),Linux)
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "$(GREEN)> Using apt-get (Debian/Ubuntu)$(RESET)"; \
		sudo apt-get update && sudo apt-get install -y libsdl2-dev libsdl2-mixer-dev; \
	elif command -v yum >/dev/null 2>&1; then \
		echo "$(GREEN)> Using yum (RHEL/CentOS)$(RESET)"; \
		sudo yum install -y SDL2-devel SDL2_mixer-devel; \
	elif command -v dnf >/dev/null 2>&1; then \
		echo "$(GREEN)> Using dnf (Fedora)$(RESET)"; \
		sudo dnf install -y SDL2-devel SDL2_mixer-devel; \
	elif command -v pacman >/dev/null 2>&1; then \
		echo "$(GREEN)> Using pacman (Arch Linux)$(RESET)"; \
		sudo pacman -S --needed sdl2 sdl2_mixer; \
	elif command -v zypper >/dev/null 2>&1; then \
		echo "$(GREEN)> Using zypper (openSUSE)$(RESET)"; \
		sudo zypper install -y libSDL2-devel libSDL2_mixer-devel; \
	elif command -v emerge >/dev/null 2>&1; then \
		echo "$(GREEN)> Using emerge (Gentoo)$(RESET)"; \
		sudo emerge -av media-libs/libsdl2 media-libs/sdl2-mixer; \
	elif command -v apk >/dev/null 2>&1; then \
		echo "$(GREEN)> Using apk (Alpine Linux)$(RESET)"; \
		sudo apk add --no-cache sdl2-dev sdl2_mixer-dev; \
	elif command -v xbps-install >/dev/null 2>&1; then \
		echo "$(GREEN)> Using xbps (Void Linux)$(RESET)"; \
		sudo xbps-install -Sy SDL2-devel SDL2_mixer-devel; \
	else \
		echo "$(RED)! Unknown package manager. Please install SDL2 and SDL2_mixer manually.$(RESET)"; \
		exit 1; \
	fi
else ifeq ($(UNAME_S),Darwin)
	@if command -v brew >/dev/null 2>&1; then \
		echo "$(GREEN)> Using Homebrew$(RESET)"; \
		brew install sdl2 sdl2_mixer; \
	elif command -v port >/dev/null 2>&1; then \
		echo "$(GREEN)> Using MacPorts$(RESET)"; \
		sudo port install libsdl2 libsdl2_mixer; \
	else \
		echo "$(RED)! Please install Homebrew or MacPorts first.$(RESET)"; \
		exit 1; \
	fi
else ifeq ($(UNAME_S),FreeBSD)
	@sudo pkg install -y sdl2 sdl2_mixer
else ifeq ($(UNAME_S),OpenBSD)
	@sudo pkg_add sdl2 sdl2-mixer
else ifeq ($(UNAME_S),NetBSD)
	@sudo pkgin install sdl2 SDL2_mixer
else
	@echo "$(RED)! Unsupported OS: $(UNAME_S)$(RESET)"
	@exit 1
endif

debug: override CFLAGS += -g -DDEBUG
debug: $(TARGET)

release: override CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

help:
	@echo "$(YELLOW)Music Player Makefile$(RESET)"
	@echo "====================="
	@echo "$(GREEN)Targets:$(RESET)"
	@echo "  all       - Build the music player (default)"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  install   - Install SDL2 dependencies for your OS"
	@echo "  clean     - Remove build files"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "$(GREEN)Detected OS:$(RESET) $(UNAME_S)"
	@echo "$(GREEN)Detected Distribution:$(RESET) $(DISTRO)"
	@echo ""
	@echo "$(GREEN)Usage:$(RESET)"
	@echo "  make install  # Install dependencies"
	@echo "  make          # Build the project"
	@echo "  ./$(TARGET) /path/to/music"
