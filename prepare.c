#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    struct stat st;

    if (stat(argv[1], &st) == -1) {
        perror("stat");
        return 1;
    }

    mode_t new_mode = st.st_mode |
                      S_IXUSR | S_IXGRP | S_IXOTH;

    if (chmod(argv[1], new_mode) == -1) {
        perror("chmod");
        return 1;
    }

    printf("Added execute permissions to %s\n", argv[1]);
    return 0;
}
