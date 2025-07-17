#include "cMusix.h"

void add_song(const char* filepath) {
    if (player.count >= MAX_SONGS) {
        return;
    }

    strncpy(player.songs[player.count].path, filepath, MAX_PATH_LENGTH - 1);
    player.songs[player.count].path[MAX_PATH_LENGTH - 1] = '\0';

    const char* filename = strrchr(filepath, '/');
    if (filename) {
        filename++;
    } else {
        filename = filepath;
    }

    strncpy(player.songs[player.count].name, filename, MAX_FILENAME_LENGTH - 1);
    player.songs[player.count].name[MAX_FILENAME_LENGTH - 1] = '\0';
    player.count++;
}

void scan_directory(const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        printf("Warning: Could not open directory: %s\n", dir_path);
        return;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files and current/parent directory entries
        if (entry->d_name[0] == '.') continue;
        
        char full_path[MAX_PATH_LENGTH];
        int ret = snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        // Check if path was truncated
        if (ret >= (int)sizeof(full_path)) {
            printf("Warning: Path too long, skipping: %s/%s\n", dir_path, entry->d_name);
            continue;
        }
        
        struct stat file_stat;
        if (stat(full_path, &file_stat) == 0) {
            if (S_ISDIR(file_stat.st_mode)) {
                // Recursively scan subdirectory
                printf("  Scanning subdirectory: %s\n", entry->d_name);
                scan_directory(full_path);
            } else if (S_ISREG(file_stat.st_mode)) {
                // Check if it's an audio file
                printf("  Checking file: %s", entry->d_name);
                if (audio_file(entry->d_name)) {
                    add_song(full_path);
                    printf(" -> ADDED\n");
                } else {
                    printf(" -> skipped (not audio)\n");
                }
            }
        } else {
            printf("Warning: Could not stat file: %s\n", full_path);
        }
    }
    
    closedir(dir);
}

void load_folder(const char* folder_path) {
    player.count = 0;
    player.current_index = 0;
    player.list_offset = 0;
    
    // Try to resolve relative path, but continue even if realpath fails
    char resolved_path[MAX_PATH_LENGTH];
    const char* path_to_use = folder_path;
    
    if (realpath(folder_path, resolved_path) != NULL) {
        path_to_use = resolved_path;
    }
    
    // Check if directory exists
    struct stat dir_stat;
    if (stat(path_to_use, &dir_stat) != 0) {
        printf("Error: Directory does not exist: %s\n", path_to_use);
        return;
    }
    
    if (!S_ISDIR(dir_stat.st_mode)) {
        printf("Error: Path is not a directory: %s\n", path_to_use);
        return;
    }
    
    printf("Scanning directory: %s\n", path_to_use);
    
    // First, try to list directory contents to debug
    DIR* test_dir = opendir(path_to_use);
    if (!test_dir) {
        printf("Error: Cannot open directory: %s\n", path_to_use);
        return;
    }
    
    printf("Directory contents:\n");
    struct dirent* entry;
    while ((entry = readdir(test_dir)) != NULL) {
        if (entry->d_name[0] != '.') {
            printf("  Found: %s\n", entry->d_name);
        }
    }
    closedir(test_dir);
    
    // Now do the actual scan
    scan_directory(path_to_use);
    printf("Total found: %d audio files\n", player.count);
    
    // Debug: List all found songs
    if (player.count > 0) {
        printf("\nFound audio files:\n");
        for (int i = 0; i < player.count; i++) {
            printf("  %d: %s\n", i + 1, player.songs[i].name);
        }
    }
}