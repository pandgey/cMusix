CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lSDL2 -lSDL2_mixer
TARGET = cMusix

# OS Detection
UNAME_S := $(shell uname -s)
DISTRO := $(shell if [ -f /etc/os-release ]; then . /etc/os-release; echo $ID; fi)

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

SRCDIR = .
OBJDIR = obj
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Additional help target
help:
	@echo "Music Player Makefile"
	@echo "====================="
	@echo "Targets:"
	@echo "  all       - Build the music player (default)"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build optimized release version"
	@echo "  install   - Install SDL2 dependencies for your OS"
	@echo "  clean     - Remove build files"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Detected OS: $(UNAME_S)"
	@echo "Detected Distribution: $(DISTRO)"
	@echo ""
	@echo "Usage:"
	@echo "  make install  # Install dependencies"
	@echo "  make          # Build the project"
	@echo "  ./music_player /path/to/music"

.PHONY: all clean install help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

# Package manager detection
install:
	@echo "Detected OS: $(UNAME_S)"
	@echo "Detected Distribution: $(DISTRO)"
	@echo "Installing SDL2 dependencies..."
ifeq ($(UNAME_S),Linux)
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "Using apt-get (Debian/Ubuntu)..."; \
		sudo apt-get update && sudo apt-get install -y libsdl2-dev libsdl2-mixer-dev; \
	elif command -v yum >/dev/null 2>&1; then \
		echo "Using yum (RHEL/CentOS)..."; \
		sudo yum install -y SDL2-devel SDL2_mixer-devel; \
	elif command -v dnf >/dev/null 2>&1; then \
		echo "Using dnf (Fedora)..."; \
		sudo dnf install -y SDL2-devel SDL2_mixer-devel; \
	elif command -v pacman >/dev/null 2>&1; then \
		echo "Using pacman (Arch Linux)..."; \
		sudo pacman -S --needed sdl2 sdl2_mixer; \
	elif command -v zypper >/dev/null 2>&1; then \
		echo "Using zypper (openSUSE)..."; \
		sudo zypper install -y libSDL2-devel libSDL2_mixer-devel; \
	elif command -v emerge >/dev/null 2>&1; then \
		echo "Using emerge (Gentoo)..."; \
		sudo emerge -av media-libs/libsdl2 media-libs/sdl2-mixer; \
	elif command -v apk >/dev/null 2>&1; then \
		echo "Using apk (Alpine Linux)..."; \
		sudo apk add --no-cache sdl2-dev sdl2_mixer-dev; \
	elif command -v xbps-install >/dev/null 2>&1; then \
		echo "Using xbps (Void Linux)..."; \
		sudo xbps-install -S SDL2-devel SDL2_mixer-devel; \
	else \
		echo "Unknown package manager. Please install SDL2 and SDL2_mixer manually."; \
		echo "Required packages: SDL2 development libraries, SDL2_mixer development libraries"; \
		exit 1; \
	fi
else ifeq ($(UNAME_S),Darwin)
	@if command -v brew >/dev/null 2>&1; then \
		echo "Using Homebrew (macOS)..."; \
		brew install sdl2 sdl2_mixer; \
	elif command -v port >/dev/null 2>&1; then \
		echo "Using MacPorts (macOS)..."; \
		sudo port install libsdl2 libsdl2_mixer; \
	else \
		echo "Please install Homebrew or MacPorts first, then run 'make install' again."; \
		echo "Homebrew: https://brew.sh/"; \
		echo "MacPorts: https://www.macports.org/"; \
		exit 1; \
	fi
else ifeq ($(UNAME_S),FreeBSD)
	@echo "Using pkg (FreeBSD)..."
	sudo pkg install -y sdl2 sdl2_mixer
else ifeq ($(UNAME_S),OpenBSD)
	@echo "Using pkg_add (OpenBSD)..."
	sudo pkg_add sdl2 sdl2-mixer
else ifeq ($(UNAME_S),NetBSD)
	@echo "Using pkgin (NetBSD)..."
	sudo pkgin install sdl2 SDL2_mixer
else
	@echo "Unsupported OS: $(UNAME_S)"
	@echo "Please install SDL2 and SDL2_mixer manually for your system."
	@exit 1
endif

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)