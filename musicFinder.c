#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <ctype.h>

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

void scan_for_music(const char* dir_path, int depth) {
    if (depth > 3) return; // Limit recursion depth
    
    DIR* dir = opendir(dir_path);
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        struct stat file_stat;
        if (stat(full_path, &file_stat) == 0) {
            if (S_ISDIR(file_stat.st_mode)) {
                // If it's a directory named Music, check it
                if (strcasecmp(entry->d_name, "Music") == 0) {
                    printf("Found Music directory: %s\n", full_path);
                    
                    // Check what's inside
                    DIR* music_dir = opendir(full_path);
                    if (music_dir) {
                        struct dirent* music_entry;
                        int music_count = 0;
                        while ((music_entry = readdir(music_dir)) != NULL) {
                            if (music_entry->d_name[0] == '.') continue;
                            
                            char music_path[1024];
                            snprintf(music_path, sizeof(music_path), "%s/%s", full_path, music_entry->d_name);
                            
                            struct stat music_stat;
                            if (stat(music_path, &music_stat) == 0 && S_ISREG(music_stat.st_mode)) {
                                if (audio_file(music_entry->d_name)) {
                                    printf("  -> %s\n", music_entry->d_name);
                                    music_count++;
                                }
                            }
                        }
                        closedir(music_dir);
                        printf("  Total audio files: %d\n\n", music_count);
                    }
                } else {
                    // Recursively search other directories
                    scan_for_music(full_path, depth + 1);
                }
            } else if (S_ISREG(file_stat.st_mode) && audio_file(entry->d_name)) {
                // Found an audio file in current directory
                printf("Audio file in %s: %s\n", dir_path, entry->d_name);
            }
        }
    }
    
    closedir(dir);
}

int main() {
    printf("=== Finding Music Files ===\n\n");
    
    // Show current directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current directory: %s\n\n", cwd);
    }
    
    // Try common music locations
    const char* music_locations[] = {
        ".",                    // Current directory
        "./Music",             // Music in current directory
        "Music",               // Music subdirectory
        NULL
    };
    
    // Also try home directory
    struct passwd *pw = getpwuid(getuid());
    char home_music[1024];
    if (pw) {
        snprintf(home_music, sizeof(home_music), "%s/Music", pw->pw_dir);
        printf("Checking home Music directory: %s\n", home_music);
        
        struct stat st;
        if (stat(home_music, &st) == 0 && S_ISDIR(st.st_mode)) {
            printf("Found home Music directory!\n");
            scan_for_music(home_music, 0);
        } else {
            printf("Home Music directory not found.\n");
        }
        printf("\n");
    }
    
    // Search in common locations
    for (int i = 0; music_locations[i] != NULL; i++) {
        printf("Searching in: %s\n", music_locations[i]);
        scan_for_music(music_locations[i], 0);
    }
    
    return 0;
}