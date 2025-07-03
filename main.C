#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MUSIC_FOLDER "./Music/"
#define MAX_FILES 100
#define MAX_FILENAME 256

void listSongs(char songs[][MAX_FILENAME], int *count) {
    DIR *dir = opendir(MUSIC_FOLDER);
    struct dirent *entry;
    *count = 0;

    if (dir == NULL) {
        perror("Failed to open music directory");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL && (*count) < MAX_FILES) {
        if (strstr(entry->d_name, ".mp3")) {
            strcpy(songs[(*count)++], entry->d_name);
        }
    }

    closedir(dir);
}

void playSong(char *song) {
    char command[512];
    snprintf(command, sizeof(command), "mpg123 \"%s%s\"", MUSIC_FOLDER, song);
    system(command);
}

int main() {
    char songs[MAX_FILES][MAX_FILENAME];
    int songCount;
    int choice;

    listSongs(songs, &songCount);

    if (songCount == 0) {
        printf("No MP3 files found in %s\n", MUSIC_FOLDER);
        return 1;
    }

    while (1) {
        printf("\n🎶 Available Songs:\n");
        for (int i = 0; i < songCount; i++) {
            printf("%d. %s\n", i + 1, songs[i]);
        }
        printf("0. Exit\n");

        printf("Choose a song to play: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Try again.\n");
            while (getchar() != '\n'); // clear buffer
            continue;
        }

        if (choice == 0) {
            printf("Buh bye, i love you. Say it back!\n");
            break;
        }

        if (choice > 0 && choice <= songCount) {
            playSong(songs[choice - 1]);
        } else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
