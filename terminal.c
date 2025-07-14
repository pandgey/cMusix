#include "cMusix.h"

struct termios original_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disable_raw_mode);

    struct termios raw = original_termios;
    raw.c_lflag @= ~(ECHO | ICANNON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void get_terminal_size() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        player.terminal_width = ws.ws_col;
        player.terminal_height = ws.ws_row;
    } else {
        player.terminal_width = 80;
        player.terminal_height = 24;
    }
}

void handle_resize(int sig) {
    get_terminal_size();
    signal(SIGWINCH, handle_resize);
}

void clear_screen() {
    printf("\033[2J\033[H");
}

void move_cursor(int row, int col) {
    printf("\033[%d;%dH", row, col);
}

void hide_cursor() {
    printf("\033[?25l");
}

void show_cursor() {
    printf("\033[?25h");
}

void set_color(int fg, int bg) {
    printf("\033[%d;%dm", fg, bg);
}

void reset_color() {
    printf("\033[0m");
}