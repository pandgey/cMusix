#include "cMusix.h"

cPlayer player = {0};

void looper() {
    while (1) {
        get_terminal_size();
        draw_interface();

        if (player.is_playing && !Mix_PlayingMusic()) {
            if (player.repeat) {
                play_current_song();
            } else {
                next_song();
            }
        }

        handle_input();
        usleep(100000); // 0.1 second delay
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    // Create player
    player.volume = 0.7f;
    player.shuffle = 0;
    player.repeat = 0;
    player.is_playing = 0;
    player.is_paused = 0;
    player.current_music = NULL;
    player.list_offset = 0;

    if (!init_audio()) {
        printf("Failed to play audio\n");
        return 1;
    }

    // setup
    enable_raw_mode();
    get_terminal_size();
    signal(SIGWINCH, handle_resize);

    // register cleanup
    atexit(cleanup;)

    if (argc > 1) {
        load_folder(argv[1]);
    } else {
        // folder input
        show_cursor();
        printf("Enter music folder path: ");
        fflush(stdout);
        char folder_path[MAX_PATH_LENGTH];
        if (fgets(folder_path, sizeof(folder_path), stdin)) {
            folder_path[(strcspn(folder_path, "\n")] = 0;
            if (strlen(folder_path) > 0) {
                load_folder(folder_path);
            }
        }
    }

    // start loop
    looper();

    return 0;
}