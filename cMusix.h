#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <ctype.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define MAX_SONGS 1000
#define MAX_PATH_LENGTH 512
#define MAX_FILENAME_LENGTH 256
#define DISPLAY_SONGS 10

// ANSI color codes
#define COLOR_RESET     0
#define COLOR_BOLD      1
#define COLOR_RED       31
#define COLOR_GREEN     32
#define COLOR_YELLOW    33
#define COLOR_BLUE      34
#define COLOR_MAGENTA   35
#define COLOR_CYAN      36
#define COLOR_WHITE     37
#define COLOR_BG_BLACK  40
#define COLOR_BG_RED    41
#define COLOR_BG_GREEN  42
#define COLOR_BG_YELLOW 43
#define COLOR_BG_BLUE   44

typedef struct {
    char path[MAX_PATH_LENGTH];
    char name[MAX_FILENAME_LENGTH];
} Song;

typedef struct {
    Song songs[MAX_SONGS];
    int count;
    int current_index;
    int is_playing;
    int is_paused;
    float volume;
    int shuffle;
    int repeat;
    Mix_Music* current_music;
    int list_offset;
    int selected_index;
    time_t song_start_time;
    int terminal_width;
    int terminal_height;
} MusicPlayer;

// Global player instance
extern MusicPlayer player;

// Terminal handling
extern struct termios original_termios;

// Function declarations
// terminal.c
void disable_raw_mode();
void enable_raw_mode();
void get_terminal_size();
void handle_resize(int sig);
void clear_screen();
void move_cursor(int row, int col);
void hide_cursor();
void show_cursor();
void set_color(int fg, int bg);
void reset_color();

// audio.c
int audio_file(const char* filename);
int init_audio();
void cleanup();
void play_current_song();
void pause_resume();
void stop_playback();
void next_song();
void previous_song();
void set_volume(float volume);
void toggle_shuffle();
void toggle_repeat();

// playlist.c
void add_song(const char* filepath);
void scan_directory(const char* dir_path);
void load_folder(const char* folder_path);

// interface.c
void draw_line(int width, char c);
void draw_progress_bar(int width, float progress);
void draw_volume_bar(int width);
void truncate_string(char* dest, const char* src, int max_width);
void draw_interface();

// input.c
void handle_input();

// main.c
void main_loop();

#endif