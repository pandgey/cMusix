#include "cMusix.h"

// Handle keyboard input
void userInput() {
    char ch = getchar();
    if (ch == EOF) return;
    
    switch (ch) {
        case ' ':
            if (!player.is_playing && player.count > 0) {
                playSong();
            } else {
                pauseResume();
            }
            break;
        case 'n':
            nextSong();
            break;
        case 'p':
            previousSong();
            break;
        case '+':
        case '=':
            set_volume(player.volume + 0.1f);
            break;
        case '-':
            set_volume(player.volume - 0.1f);
            break;
        case 's':
            shuffleFunction();
            break;
        case 'r':
            repeatFunction();
            break;
        case 'q':
            exit(0);
            break;
        case 'j':
            if (player.list_offset + DISPLAY_SONGS < player.count) {
                player.list_offset++;
            }
            break;
        case 'k':
            if (player.list_offset > 0) {
                player.list_offset--;
            }
            break;
    }
}