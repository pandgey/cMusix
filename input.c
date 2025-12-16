#include "cMusix.h"

// Handle keyboard input
void userInput() {
    fd_set fds;
    struct timeval tv;
    
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    // Check if input is available
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) <= 0) {
        return;
    }
    
    char ch;
    if (read(STDIN_FILENO, &ch, 1) != 1) {
        return;
    }
    
    // Handle escape sequences (arrow keys, etc.)
    if (ch == 27) {  // ESC
        // Check if this is part of an escape sequence
        char seq[3];
        
        // Set a very short timeout to check for sequence
        tv.tv_sec = 0;
        tv.tv_usec = 10000;  // 10ms
        
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
            // This is an escape sequence, read and ignore it
            if (read(STDIN_FILENO, &seq[0], 1) == 1) {
                if (seq[0] == '[') {
                    read(STDIN_FILENO, &seq[1], 1);
                    // Ignore the escape sequence
                    return;
                }
            }
            return;
        }
        // If no sequence follows, treat as actual ESC key press
        // Fall through to quit
    }
    
    switch (ch) {
        case ' ':
            if (!player.is_playing && player.count > 0) {
                playSong();
            } else {
                pauseResume();
            }
            break;
        case 'n':
        case 'N':
            nextSong();
            break;
        case 'p':
        case 'P':
            previousSong();
            break;
        case '+':
        case '=':
            set_volume(player.volume + 0.1f);
            break;
        case '-':
        case '_':
            set_volume(player.volume - 0.1f);
            break;
        case 's':
        case 'S':
            shuffleFunction();
            break;
        case 'r':
        case 'R':
            repeatFunction();
            break;
        case 'q':
        case 'Q':
            cleanup();
            exit(0);
            break;
        case 27: // ESC key (only reaches here if not part of sequence)
            cleanup();
            exit(0);
            break;
        case 'j':
        case 'J':
            if (player.list_offset + DISPLAY_SONGS < player.count) {
                player.list_offset++;
            }
            break;
        case 'k':
        case 'K':
            if (player.list_offset > 0) {
                player.list_offset--;
            }
            break;
        default:
            // Ignore unrecognized keys instead of crashing
            break;
    }
}