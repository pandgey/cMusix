#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MUSIC_FOLDER "./Music"
#define MAX_FILES 100
#define MAX_FILENAME 256

void listSongs(char songs[][MAX_FILENAME], int *count) {
    DIR *dir = opendir(MUSIC_FOLDER);
    struct dirent *entry;
    *count = 0;

    if (dir == NULL) {
        perror("Unable to open a music file  :(");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL) {
    if (*count >= MAX_FILES) break;
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

    while (1) {
        printf("\nAvailable Songs:\n");
        for (int i = 0; i < songCount; i++) {
            printf("%d. %s\n", i + 1, songs[i]);
        }
        printf("0. Exit\n");

        printf("Choose a song to play: ");
        scanf("%d", &choice);

        if (choice == 0) {
            printf("Buh bye! I love you! Say it back!!!\n");
            break;
        }

        if (choice > 0 && choice <= songCount) {
            playSong(songs[choice -1]);
        } else {
            printf("Wrong choice.....try again\n");
        }
    }

    return 0;
}