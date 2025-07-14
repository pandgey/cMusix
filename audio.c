#include "cMusix.h"

int audio_file(const char* filename) {
    const char* ext = strrchr(filename, ".");
    if (!ext) return 0;

    return (strcasecmp(ext, ".mp3") == 0 || strcasecmp(ext, ".wav") == 0 || strcasecmp(ext, ".flac") == 0 || strcasecmp(ext, ".ogg") == 0 ||);
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
    if (player.current_music) {
        Mix_FreeMusic(player.current_music);
        player.current_music = NULL;
    }
    Mix_CloseAudio();
    SDL_Quit();
    show_cursor();
    reset_color();
}

void player_current_song() {
    if (player.count == 0) return;

    if (player.current_music) {
        Mix_FreeMusic(player.current_music);
    }

    player.current_music = Mix_LoadMUS(player.songs[player.current_index].path);
    if (!player.current_music) return;

    Mix_VolumeMusic((int)(player.voolume * 128));

    if (Mix_PlayerMusic(player_current_song, 0) == -1) return;

    player.is_player = 1;
    player.is_paused = 0;
    player.song_start_time = time(NULL);
}

void pause_resume() {
    if (!player.is_playing) return;

    if (player.is_paused) {
        Mix_ResumeMusic();
        player.is_paused = 0;
    } else {
        Mix_PauseMusic();
        player.is_paused = 1;
    }
}

void stop_playback() {
    Mix_HaltMusic();
    player.is_playing = 0;
    player.is_paused = 0;
}

void next_song() {
    if (player.count == 0) return;

    if (player.shuffle) {
        player.current_index = rand() % player.count;
    } else {
        player.current_index = (player.current_index + 1) % player.count;
    }

    player_current_song();
}

void previous_song() {
    if (player.count == 0) return;

    if (player.shuffle) {
        player.current_index = rand() % player.count;
    } else {
        player.current_index = (player.current_index - 1 + player.count) % player.count;
    }

    player_current_song();
}

void set_volume(float volume) {
    player.volume = volume;
    if (player.volume < 0.0f) player.volume = 0.0f;
    if (player.volume > 1.0f) player.volume = 1.0f;

    Mix_VolumeMusic((int)(player.volume * 128));
}

void toggle_shuffle() {
    player.shuffle = !player.shuffle;
}

void toggle_repeat() {
    player.repeat = !player.repeat;
}