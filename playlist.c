#include "cMusix.h"

void add_song(const char* filepath) {
    if (player.count >= MAX_SONGS) {
        return;
    }

    strncpy(player.songs[player.count].path, filepath, MAX_PATH_LENGTH -1);

    const char* filename = strrchr(filepath, '/');
    if (filename) {
        filename++;
    } else {
        filename = filepath;
    }

    strncpy(player.songs[player.count].name, filename, MAX_FILENAME_LENGTH -1);
    player.count++;
}

void scan_directory(const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        char full_path[MAX_PATH_LENGTH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        struct stat file_stat;
        if (stat(full_path, &file_stat) == 0) {
            if (S_ISDIR(file_stat.st_mode)) {
                scan_directory(full_path);
            } else if (S_ISREG(file_stat.st_mode) && is_audio_file(entry->d_name)) {
                add_song(full_path);
            }
        }
    }
    
    closedir(dir);
}

void load_folder(const char* folder_path) {
    player.count = 0;
    player.current_index = 0;
    player.list_offset = 0;
    
    scan_directory(folder_path);
}