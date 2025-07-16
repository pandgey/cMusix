#include "cMusix.h"

void createLine(int width, char c) {
    for (int i = 0; i < width; i++) {
        printf("%c", c);
    }
}

void progressBar(int width, float progress) {
    int filled = (int)(progress * width);
    printf("[");
    for (int i = 0; i < width - 2; i++) {
        if (i < filled) {
            set_color(COLOR_GREEN, COLOR_BG_BLACK);
            printf("█");
        } else {
            set_color(COLOR_WHITE, COLOR_BG_BLACK);
            printf("░");
        }
    }
    reset_color();
    printf("]");
}

void volumeBar(int width) {
    int filled = (int)(player.volume * width);
    printf("Vol: ");
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            set_color(COLOR_CYAN, COLOR_BG_BLACK);
            printf("█");
        } else {
            set_color(COLOR_WHITE, COLOR_BG_BLACK);
            printf("░");
        }
    }
    reset_color();
    printf(" %d%%", (int)(player.volume * 100));
}

void truncate_string(char* dest, const char* src, int max_width) {
    if ((int)strlen(src) <= max_width) {
        strcpy(dest, src);
    } else {
        strncpy(dest, src, max_width - 3);
        dest[max_width - 3] = '\0';
        strcat(dest, "...");
    }
}

void createInterface() {
    clear_screen();
    hide_cursor();
    
    int width = player.terminal_width;
    int height = player.terminal_height;
    
    // Header
    move_cursor(1, 1);
    set_color(COLOR_BOLD, COLOR_BG_BLUE);
    printf("  cMusix  ");
    for (int i = 10; i < width; i++) printf(" ");
    reset_color();
    
    // Current song info
    move_cursor(3, 1);
    if (player.count > 0) {
        char truncated_name[256];
        truncate_string(truncated_name, player.songs[player.current_index].name, width - 10);
        
        set_color(COLOR_BOLD, COLOR_BG_BLACK);
        printf("♪ Now Playing: ");
        set_color(COLOR_YELLOW, COLOR_BG_BLACK);
        printf("%s", truncated_name);
        reset_color();
        
        move_cursor(4, 1);
        printf("Track %d of %d", player.current_index + 1, player.count);
    } else {
        set_color(COLOR_RED, COLOR_BG_BLACK);
        printf("No songs loaded");
        reset_color();
    }
    
    // Status line
    move_cursor(6, 1);
    const char* status = "⏹ Stopped";
    if (player.is_playing) {
        status = player.is_paused ? "⏸ Paused" : "▶ Playing";
    }
    
    set_color(COLOR_GREEN, COLOR_BG_BLACK);
    printf("Status: %s", status);
    reset_color();
    
    // Progress bar (placeholder - SDL_mixer doesn't provide position info easily)
    move_cursor(7, 1);
    time_t current_time = time(NULL);
    float fake_progress = 0.0f;
    if (player.is_playing && !player.is_paused) {
        fake_progress = fmod((current_time - player.song_start_time) / 30.0f, 1.0f);
    }
    progressBar(width - 20, fake_progress);
    
    // Volume bar
    move_cursor(8, 1);
    volumeBar(20);
    
    // Mode indicators
    move_cursor(8, width - 30);
    if (player.shuffle) {
        set_color(COLOR_MAGENTA, COLOR_BG_BLACK);
        printf("SHUFFLE");
        reset_color();
    }
    if (player.repeat) {
        set_color(COLOR_CYAN, COLOR_BG_BLACK);
        printf("REPEAT");
        reset_color();
    }
    
    // Separator
    move_cursor(10, 1);
    set_color(COLOR_WHITE, COLOR_BG_BLACK);
    createLine(width, '-');
    reset_color();
    
    // Playlist
    move_cursor(11, 1);
    set_color(COLOR_BOLD, COLOR_BG_BLACK);
    printf("PLAYLIST:");
    reset_color();
    
    int list_height = height - 15;
    int start_row = 12;
    
    for (int i = 0; i < list_height && i < player.count; i++) {
        int song_index = player.list_offset + i;
        if (song_index >= player.count) break;
        
        move_cursor(start_row + i, 1);
        
        char truncated_name[256];
        truncate_string(truncated_name, player.songs[song_index].name, width - 10);
        
        // Highlight current song
        if (song_index == player.current_index) {
            set_color(COLOR_BLACK, COLOR_BG_GREEN);
            printf("▶ %3d. %s", song_index + 1, truncated_name);
            for (int j = (int)strlen(truncated_name) + 7; j < width; j++) printf(" ");
            reset_color();
        } else {
            printf("  %3d. %s", song_index + 1, truncated_name);
        }
    }
    
    // Controls help
    move_cursor(height - 3, 1);
    set_color(COLOR_WHITE, COLOR_BG_BLACK);
    createLine(width, '-');
    reset_color();
    
    move_cursor(height - 2, 1);
    set_color(COLOR_CYAN, COLOR_BG_BLACK);
    printf("CONTROLS: [SPACE]Play/Pause [n]Next [p]Previous [+/-]Volume [s]Shuffle [r]Repeat [q]Quit");
    reset_color();
    
    fflush(stdout);
}