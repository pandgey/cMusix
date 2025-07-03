#!/bin/bash

# CLI Music Player Installation Script
# Supports: Ubuntu/Debian, macOS, Fedora/RHEL, Arch Linux

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
INSTALL_DIR="$HOME/.local/bin"
SOURCE_FILE="main.c"
BINARY_NAME="cMusix"
DESKTOP_FILE="$HOME/.local/share/applications/music-player.desktop"

# Detect OS
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get &> /dev/null; then
            OS="ubuntu"
        elif command -v dnf &> /dev/null; then
            OS="fedora"
        elif command -v pacman &> /dev/null; then
            OS="arch"
        else
            OS="linux"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    else
        OS="unknown"
    fi
}

# Install dependencies based on OS
install_dependencies() {
    print_status "Installing dependencies for $OS..."
    
    case $OS in
        ubuntu)
            sudo apt-get update
            sudo apt-get install -y libsdl2-dev libsdl2-mixer-dev gcc make
            ;;
        fedora)
            sudo dnf install -y SDL2-devel SDL2_mixer-devel gcc make
            ;;
        arch)
            sudo pacman -S --needed sdl2 sdl2_mixer gcc make
            ;;
        macos)
            if ! command -v brew &> /dev/null; then
                print_error "Homebrew is required for macOS installation"
                print_status "Installing Homebrew..."
                /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
            fi
            brew install sdl2 sdl2_mixer
            ;;
        *)
            print_error "Unsupported operating system: $OSTYPE"
            print_status "Please install SDL2 and SDL2_mixer manually"
            exit 1
            ;;
    esac
    
    print_success "Dependencies installed successfully"
}

# Check if source file exists
checkSource() {
    if [[ ! -f "$SOURCE_FILE" ]]; then
        print_error "Source file '$SOURCE_FILE' not found"
        print_status "Please make sure music_player.c is in the current directory"
        exit 1
    fi
}

# Create installation directory
create_install_dir() {
    if [[ ! -d "$INSTALL_DIR" ]]; then
        print_status "Creating installation directory: $INSTALL_DIR"
        mkdir -p "$INSTALL_DIR"
    fi
}

# Compile the music player
compile_player() {
    print_status "Compiling music player..."
    
    # Try different compilation methods
    if gcc -o "$BINARY_NAME" "$SOURCE_FILE" -lSDL2 -lSDL2_mixer 2>/dev/null; then
        print_success "Compilation successful"
    elif pkg-config --exists sdl2 SDL2_mixer 2>/dev/null; then
        print_status "Trying with pkg-config..."
        gcc -o "$BINARY_NAME" "$SOURCE_FILE" $(pkg-config --cflags --libs sdl2 SDL2_mixer)
        print_success "Compilation successful with pkg-config"
    else
        print_error "Compilation failed"
        print_status "Please check that SDL2 development libraries are installed"
        exit 1
    fi
}

# Install the binary
install_binary() {
    print_status "Installing binary to $INSTALL_DIR"
    mv "$BINARY_NAME" "$INSTALL_DIR/"
    chmod +x "$INSTALL_DIR/$BINARY_NAME"
    print_success "Binary installed successfully"
}

# Add to PATH if needed
update_path() {
    # Check if directory is already in PATH
    if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
        print_status "Adding $INSTALL_DIR to PATH"
        
        # Determine shell config file
        if [[ -n "$ZSH_VERSION" ]]; then
            SHELL_CONFIG="$HOME/.zshrc"
        elif [[ -n "$BASH_VERSION" ]]; then
            SHELL_CONFIG="$HOME/.bashrc"
        else
            SHELL_CONFIG="$HOME/.profile"
        fi
        
        # Add to PATH
        echo "" >> "$SHELL_CONFIG"
        echo "# Added by music player installer" >> "$SHELL_CONFIG"
        echo "export PATH=\"\$PATH:$INSTALL_DIR\"" >> "$SHELL_CONFIG"
        
        print_success "Added to PATH in $SHELL_CONFIG"
        print_warning "Please restart your terminal or run: source $SHELL_CONFIG"
    else
        print_status "Directory already in PATH"
    fi
}

# Create desktop entry (Linux only)
create_desktop_entry() {
    if [[ "$OS" != "macos" ]]; then
        print_status "Creating desktop entry..."
        
        # Create directories if they don't exist
        mkdir -p "$(dirname "$DESKTOP_FILE")"
        
        cat > "$DESKTOP_FILE" << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=cMusix
Comment=Command-line music player
Exec=$INSTALL_DIR/$BINARY_NAME
Icon=multimedia-audio-player
Terminal=true
Categories=AudioVideo;Audio;Player;
Keywords=music;audio;player;cli;
EOF
        
        chmod +x "$DESKTOP_FILE"
        print_success "Desktop entry created"
    fi
}

# Test installation
test_installation() {
    print_status "Testing installation..."
    
    if [[ -x "$INSTALL_DIR/$BINARY_NAME" ]]; then
        print_success "Installation test passed"
        print_status "You can now run: $BINARY_NAME [music_folder]"
        print_status "Or if PATH is not updated yet: $INSTALL_DIR/$BINARY_NAME [music_folder]"
    else
        print_error "Installation test failed"
        exit 1
    fi
}

# Create uninstall script
create_uninstaller() {
    UNINSTALL_SCRIPT="$INSTALL_DIR/uninstall-music-player.sh"
    
    cat > "$UNINSTALL_SCRIPT" << 'EOF'
#!/bin/bash
# Uninstall script for CLI Music Player

INSTALL_DIR="$HOME/.local/bin"
BINARY_NAME="music_player"
DESKTOP_FILE="$HOME/.local/share/applications/music-player.desktop"

echo "Uninstalling CLI Music Player..."

# Remove binary
if [[ -f "$INSTALL_DIR/$BINARY_NAME" ]]; then
    rm "$INSTALL_DIR/$BINARY_NAME"
    echo "Removed binary"
fi

# Remove desktop entry
if [[ -f "$DESKTOP_FILE" ]]; then
    rm "$DESKTOP_FILE"
    echo "Removed desktop entry"
fi

# Remove uninstaller
rm "$0"

echo "Uninstallation complete"
echo "Note: You may want to remove the PATH entry from your shell config file"
EOF
    
    chmod +x "$UNINSTALL_SCRIPT"
    print_success "Created uninstaller: $UNINSTALL_SCRIPT"
}

# Show usage information
show_usage() {
    cat << EOF

🎵 CLI Music Player Installation Complete!

Usage:
  $BINARY_NAME [music_folder]     # Play music from specified folder
  $BINARY_NAME                    # Will ask for folder path

Commands (once running):
  p/SPACE  - Play/Pause
  n        - Next song
  b        - Previous song
  +/-      - Volume up/down
  z        - Toggle shuffle
  r        - Toggle repeat
  l        - List songs
  h        - Show help
  q        - Quit

Examples:
  $BINARY_NAME ~/Music
  $BINARY_NAME /path/to/music/folder

To uninstall:
  $INSTALL_DIR/uninstall-music-player.sh

EOF
}

# Main installation function
main() {
    echo "CLI Music Player Installer"
    echo "=============================="
    
    # Check if running as root
    if [[ $EUID -eq 0 ]]; then
        print_warning "Running as root. This will install system-wide."
        INSTALL_DIR="/usr/local/bin"
    fi
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --prefix=*)
                INSTALL_DIR="${1#*=}"
                shift
                ;;
            --help|-h)
                echo "Usage: $0 [--prefix=DIR] [--help]"
                echo "  --prefix=DIR  Install to specific directory (default: $HOME/.local/bin)"
                echo "  --help        Show this help"
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    # Run installation steps
    detect_os
    print_status "Detected OS: $OS"
    
    checkSource
    install_dependencies
    create_install_dir
    compile_player
    install_binary
    
    # Only update PATH for user installations
    if [[ "$INSTALL_DIR" == "$HOME/.local/bin" ]]; then
        update_path
    fi
    
    create_desktop_entry
    create_uninstaller
    test_installation
    show_usage
    
    print_success "Installation completed successfully!"
}

# Run main function
main "$@"