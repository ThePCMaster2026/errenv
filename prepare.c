#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int get_host_ip(char *ip, size_t size) {
    struct ifaddrs *ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *addr =
                (struct sockaddr_in *)ifa->ifa_addr;

            const char *result =
                inet_ntop(AF_INET, &addr->sin_addr, ip, size);

            if (result && strcmp(ip, "127.0.0.1") != 0) {
                freeifaddrs(ifaddr);
                return 0;
            }
        }
    }

    freeifaddrs(ifaddr);
    return -1;
}


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
char ip[INET_ADDRSTRLEN];
if (get_host_ip(ip, sizeof(ip)) == -1) {
        fprintf(stderr, "Could not find host IP\n");
        return 1;
    }
    FILE *in = fopen(argv[1], "r");
    if (!in) {
        perror("fopen");
        return 1;
    }

char tempname[1024];
    snprintf(tempname, sizeof(tempname), "%s.tmp", argv[1]);

    FILE *out = fopen(tempname, "w");
    if (!out) {
        perror("fopen");
        fclose(in);
        return 1;
    }

    char line[4096];

    while (fgets(line, sizeof(line), in)) {
        char *p = line;

        while (1) {
            char *match = strstr(p, "1.1.1.1");

            if (!match) {
                fputs(p, out);
                break;
            }

            /* Write everything before "1.1.1.1" */
            fwrite(p, 1, match - p, out);

            /* HERE is where 1.1.1.1 is replaced with the host IP */
            fputs(ip, out);

            p = match + strlen("1.1.1.1");
        }
    }

    fclose(in);
    fclose(out);

    if (rename(tempname, argv[1]) == -1) {
        perror("rename");
        return 1;
    }

    mode_t new_mode = st.st_mode |
                      S_IXUSR | S_IXGRP | S_IXOTH;

    if (chmod(argv[1], new_mode) == -1) {
        perror("chmod");
        return 1;
    }





    printf("Prepare success for %s\n", argv[1]);
    return 0;
}
