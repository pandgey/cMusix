#include "cMusix.h"

int audio_file(const char* filename) {
    if (!filename) return 0;
    
    const char* ext = strrchr(filename, '.');
    if (!ext) return 0;
    
    // Convert extension to lowercase for comparison
    char lower_ext[10];
    int i = 0;
    while (ext[i] && i < 9) {
        lower_ext[i] = tolower(ext[i]);
        i++;
    }
    lower_ext[i] = '\0';
    
    // Check for supported audio file extensions
    return (strcmp(lower_ext, ".mp3") == 0 || 
            strcmp(lower_ext, ".wav") == 0 || 
            strcmp(lower_ext, ".flac") == 0 || 
            strcmp(lower_ext, ".ogg") == 0 ||
            strcmp(lower_ext, ".m4a") == 0 ||
            strcmp(lower_ext, ".aac") == 0);
}

int init_audio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        return 0;
    }

    return 1;
}

void cleanup() {
    // Stop and free music
    if (player.current_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(player.current_music);
        player.current_music = NULL;
    }
    
    // Close audio systems
    Mix_CloseAudio();
    SDL_Quit();
    
    // Restore terminal state
    clear_screen();
    move_cursor(1, 1);
    show_cursor();
    reset_color();
    
    // Restore normal terminal mode
    rawModeOff();
    
    // Print a nice goodbye message
    printf("You are now exiting cMusix...\n");
    fflush(stdout);
}

void playSong() {
    if (player.count == 0) return;

    if (player.current_music) {
        Mix_FreeMusic(player.current_music);
    }

    player.current_music = Mix_LoadMUS(player.songs[player.current_index].path);
    if (!player.current_music) return;

    Mix_VolumeMusic((int)(player.volume * 128));

    if (Mix_PlayMusic(player.current_music, 0) == -1) return;

    player.is_playing = 1;
    player.is_paused = 0;
    player.song_start_time = time(NULL);
}

void pauseResume() {
    if (!player.is_playing) return;

    if (player.is_paused) {
        Mix_ResumeMusic();
        player.is_paused = 0;
    } else {
        Mix_PauseMusic();
        player.is_paused = 1;
    }
}

void stopPlayback() {
    Mix_HaltMusic();
    player.is_playing = 0;
    player.is_paused = 0;
}

void nextSong() {
    if (player.count == 0) return;

    if (player.shuffle) {
        player.current_index = rand() % player.count;
    } else {
        player.current_index = (player.current_index + 1) % player.count;
    }

    playSong();
}

void previousSong() {
    if (player.count == 0) return;

    if (player.shuffle) {
        player.current_index = rand() % player.count;
    } else {
        player.current_index = (player.current_index - 1 + player.count) % player.count;
    }

    playSong();
}

void set_volume(float volume) {
    player.volume = volume;
    if (player.volume < 0.0f) player.volume = 0.0f;
    if (player.volume > 1.0f) player.volume = 1.0f;

    Mix_VolumeMusic((int)(player.volume * 128));
}

void shuffleFunction() {
    player.shuffle = !player.shuffle;
}

void repeatFunction() {
    player.repeat = !player.repeat;
}