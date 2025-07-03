#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <fcntl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define MAX_SONGS 1000
#define MAX_PATH_LENGTH 512
#define MAX_FILENAME_LENGTH 256

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
} MusicPlayer;

// Global player instance
MusicPlayer player = {0};

// Terminal handling for non-blocking input
struct termios original_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disable_raw_mode);
    
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int kbhit() {
    int ch = getchar();
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

// Check if file has supported audio extension
int is_audio_file(const char* filename) {
    const char* ext = strrchr(filename, '.');
    if (!ext) return 0;
    
    return (strcasecmp(ext, ".mp3") == 0 ||
            strcasecmp(ext, ".wav") == 0 ||
            strcasecmp(ext, ".flac") == 0 ||
            strcasecmp(ext, ".ogg") == 0);
}

// Add song to playlist
void add_song(const char* filepath) {
    if (player.count >= MAX_SONGS) {
        printf("Warning: Maximum songs limit reached (%d)\n", MAX_SONGS);
        return;
    }
    
    strncpy(player.songs[player.count].path, filepath, MAX_PATH_LENGTH - 1);
    
    // Extract filename from path
    const char* filename = strrchr(filepath, '/');
    if (filename) {
        filename++; // Skip the '/'
    } else {
        filename = filepath;
    }
    
    strncpy(player.songs[player.count].name, filename, MAX_FILENAME_LENGTH - 1);
    player.count++;
}

// Recursively scan directory for music files
void scan_directory(const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        printf("Error: Cannot open directory '%s'\n", dir_path);
        return;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue; // Skip hidden files
        
        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        struct stat file_stat;
        if (stat(full_path, &file_stat) == 0) {
            if (S_ISDIR(file_stat.st_mode)) {
                // Recursively scan subdirectory
                scan_directory(full_path);
            } else if (S_ISREG(file_stat.st_mode) && is_audio_file(entry->d_name)) {
                add_song(full_path);
            }
        }
    }
    
    closedir(dir);
}

// Initialize SDL and audio
int init_audio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Error: SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Error: SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 0;
    }
    
    return 1;
}

// Clean up resources
void cleanup() {
    if (player.current_music) {
        Mix_FreeMusic(player.current_music);
        player.current_music = NULL;
    }
    Mix_CloseAudio();
    SDL_Quit();
}

// Play current song
void play_current_song() {
    if (player.count == 0) {
        printf("No songs in playlist\n");
        return;
    }
    
    if (player.current_music) {
        Mix_FreeMusic(player.current_music);
    }
    
    player.current_music = Mix_LoadMUS(player.songs[player.current_index].path);
    if (!player.current_music) {
        printf("Error loading music: %s\n", Mix_GetError());
        return;
    }
    
    Mix_VolumeMusic((int)(player.volume * 128));
    
    if (Mix_PlayMusic(player.current_music, 0) == -1) {
        printf("Error playing music: %s\n", Mix_GetError());
        return;
    }
    
    player.is_playing = 1;
    player.is_paused = 0;
    
    printf("\n♪ Now playing: %s\n", player.songs[player.current_index].name);
    printf("   [%d/%d]\n", player.current_index + 1, player.count);
}

// Pause or resume playback
void pause_resume() {
    if (!player.is_playing) {
        printf("No song is playing\n");
        return;
    }
    
    if (player.is_paused) {
        Mix_ResumeMusic();
        player.is_paused = 0;
        printf("▶ Resumed\n");
    } else {
        Mix_PauseMusic();
        player.is_paused = 1;
        printf("⏸ Paused\n");
    }
}

// Stop playback
void stop_playback() {
    Mix_HaltMusic();
    player.is_playing = 0;
    player.is_paused = 0;
    printf("⏹ Stopped\n");
}

// Next song
void next_song() {
    if (player.count == 0) return;
    
    if (player.shuffle) {
        player.current_index = rand() % player.count;
    } else {
        player.current_index = (player.current_index + 1) % player.count;
    }
    
    play_current_song();
}

// Previous song
void previous_song() {
    if (player.count == 0) return;
    
    if (player.shuffle) {
        player.current_index = rand() % player.count;
    } else {
        player.current_index = (player.current_index - 1 + player.count) % player.count;
    }
    
    play_current_song();
}

// Set volume
void set_volume(float volume) {
    player.volume = volume;
    if (player.volume < 0.0f) player.volume = 0.0f;
    if (player.volume > 1.0f) player.volume = 1.0f;
    
    Mix_VolumeMusic((int)(player.volume * 128));
    printf("Volume: %d%%\n", (int)(player.volume * 100));
}

// Toggle shuffle
void toggle_shuffle() {
    player.shuffle = !player.shuffle;
    printf("Shuffle: %s\n", player.shuffle ? "ON" : "OFF");
}

// Toggle repeat
void toggle_repeat() {
    player.repeat = !player.repeat;
    printf("Repeat: %s\n", player.repeat ? "ON" : "OFF");
}

// List all songs
void list_songs() {
    if (player.count == 0) {
        printf("No songs in playlist\n");
        return;
    }
    
    printf("\nPlaylist (%d songs):\n", player.count);
    for (int i = 0; i < player.count; i++) {
        const char* marker = (i == player.current_index) ? "▶" : " ";
        printf("%s %3d. %s\n", marker, i + 1, player.songs[i].name);
    }
}

// Jump to specific song
void jump_to_song(int index) {
    if (player.count == 0) {
        printf("No songs in playlist\n");
        return;
    }
    
    if (index >= 0 && index < player.count) {
        player.current_index = index;
        play_current_song();
    } else {
        printf("Invalid song number. Choose 1-%d\n", player.count);
    }
}

// Show help
void show_help() {
    printf("\nCLI Music Player Commands:\n");
    printf("─────────────────────────────\n");
    printf("  SPACE/p    - Play/Pause\n");
    printf("  n          - Next song\n");
    printf("  b          - Previous song\n");
    printf("  s          - Stop\n");
    printf("  +          - Volume up\n");
    printf("  -          - Volume down\n");
    printf("  z          - Toggle shuffle\n");
    printf("  r          - Toggle repeat\n");
    printf("  l          - List all songs\n");
    printf("  f          - Load new folder\n");
    printf("  1-9        - Jump to song number\n");
    printf("  h/?        - Show this help\n");
    printf("  q          - Quit\n");
    printf("─────────────────────────────\n");
}

// Load folder
void load_folder(const char* folder_path) {
    // Clear current playlist
    player.count = 0;
    player.current_index = 0;
    
    printf("Scanning folder: %s\n", folder_path);
    scan_directory(folder_path);
    
    if (player.count == 0) {
        printf("No music files found in '%s'\n", folder_path);
    } else {
        printf("Loaded %d songs\n", player.count);
    }
}

// Main loop
void main_loop() {
    char input[256];
    int running = 1;
    
    printf("🎵 CLI Music Player\n");
    printf("Type 'h' for help, 'q' to quit\n");
    
    while (running) {
        printf("\n> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin)) {
            // Remove newline
            input[strcspn(input, "\n")] = 0;
            
            // Convert to lowercase
            for (int i = 0; input[i]; i++) {
                input[i] = tolower(input[i]);
            }
            
            if (strcmp(input, "q") == 0 || strcmp(input, "quit") == 0) {
                running = 0;
            } else if (strcmp(input, "h") == 0 || strcmp(input, "?") == 0) {
                show_help();
            } else if (strcmp(input, " ") == 0 || strcmp(input, "p") == 0) {
                if (!player.is_playing && player.count > 0) {
                    play_current_song();
                } else {
                    pause_resume();
                }
            } else if (strcmp(input, "n") == 0) {
                next_song();
            } else if (strcmp(input, "b") == 0) {
                previous_song();
            } else if (strcmp(input, "s") == 0) {
                stop_playback();
            } else if (strcmp(input, "+") == 0) {
                set_volume(player.volume + 0.1f);
            } else if (strcmp(input, "-") == 0) {
                set_volume(player.volume - 0.1f);
            } else if (strcmp(input, "z") == 0) {
                toggle_shuffle();
            } else if (strcmp(input, "r") == 0) {
                toggle_repeat();
            } else if (strcmp(input, "l") == 0) {
                list_songs();
            } else if (strcmp(input, "f") == 0) {
                printf("Enter folder path: ");
                fflush(stdout);
                char folder_path[MAX_PATH_LENGTH];
                if (fgets(folder_path, sizeof(folder_path), stdin)) {
                    folder_path[strcspn(folder_path, "\n")] = 0;
                    stop_playback();
                    load_folder(folder_path);
                }
            } else if (strlen(input) == 1 && input[0] >= '1' && input[0] <= '9') {
                jump_to_song(input[0] - '1');
            } else if (strlen(input) > 0) {
                printf("Unknown command. Type 'h' for help.\n");
            }
        }
        
        // Check if song finished and handle repeat/next
        if (player.is_playing && !Mix_PlayingMusic()) {
            if (player.repeat) {
                play_current_song();
            } else {
                next_song();
            }
        }
    }
    
    printf("Goodbye! 👋\n");
}

int main(int argc, char* argv[]) {
    // Initialize random seed
    srand(time(NULL));
    
    // Initialize player
    player.volume = 0.7f;
    player.shuffle = 0;
    player.repeat = 0;
    player.is_playing = 0;
    player.is_paused = 0;
    player.current_music = NULL;
    
    // Initialize audio
    if (!init_audio()) {
        return 1;
    }
    
    // Register cleanup function
    atexit(cleanup);
    
    // Load folder from command line or ask user
    if (argc > 1) {
        load_folder(argv[1]);
    } else {
        printf("Enter music folder path: ");
        fflush(stdout);
        char folder_path[MAX_PATH_LENGTH];
        if (fgets(folder_path, sizeof(folder_path), stdin)) {
            folder_path[strcspn(folder_path, "\n")] = 0;
            if (strlen(folder_path) > 0) {
                load_folder(folder_path);
            }
        }
    }
    
    // Start main loop
    main_loop();
    
    return 0;
}