#include "cMusix.h"

MusicPlayer player = {0};

void looper() {
    while (1) {
        get_terminal_size();
        createInterface();

        if (player.is_playing && !Mix_PlayingMusic()) {
            if (player.repeat) {
                playSong();
            } else {
                nextSong();
            }
        }

        userInput();
        usleep(100000); // 0.1 second delay
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    // Initialize player
    player.volume = 0.7f;
    player.shuffle = 0;
    player.repeat = 0;
    player.is_playing = 0;
    player.is_paused = 0;
    player.current_music = NULL;
    player.list_offset = 0;

    if (!init_audio()) {
        printf("Failed to initialize audio\n");
        return 1;
    }

    // Setup terminal
    rawModeOn();
    get_terminal_size();
    signal(SIGWINCH, resize);

    // Register cleanup
    atexit(cleanup);

    if (argc > 1) {
        // Use command line argument
        load_folder(argv[1]);
    } else {
        // Try multiple common music directory names
        const char* music_dirs[] = {
            "./Music/",
            "./Music",
            "Music/",
            "Music",
            "~/Music/",
            "~/Music",
            NULL
        };
        
        int found_music_dir = 0;
        for (int i = 0; music_dirs[i] != NULL; i++) {
            struct stat dir_stat;
            if (stat(music_dirs[i], &dir_stat) == 0 && S_ISDIR(dir_stat.st_mode)) {
                printf("Found %s directory, loading...\n", music_dirs[i]);
                load_folder(music_dirs[i]);
                found_music_dir = 1;
                break;
            }
        }
        
        if (!found_music_dir) {
            // Ask for folder input
            show_cursor();
            printf("No Music directory found.\n");
            printf("Enter music folder path (or press Enter for current directory): ");
            fflush(stdout);
            char folder_path[MAX_PATH_LENGTH];
            if (fgets(folder_path, sizeof(folder_path), stdin)) {
                folder_path[strcspn(folder_path, "\n")] = 0;
                if (strlen(folder_path) == 0) {
                    strcpy(folder_path, ".");
                }
                load_folder(folder_path);
            }
        }
    }

    if (player.count == 0) {
        printf("No audio files found! Press any key to continue...\n");
        getchar();
    } else {
        printf("Press any key to start cMusix...\n");
        getchar();
    }

    // Start main loop
    looper();

    return 0;
}