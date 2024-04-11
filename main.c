#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

int main(int argc, char *argv[]) {
    int show_inode = 0;
    int long_listing = 0;
    for(int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            show_inode = 1;
        }
        if (strcmp(argv[i], "-l") == 0) {
            long_listing = 1;
        }
    }

    DIR *d;
    struct dirent *dir;
    d = opendir(".");

    int total_size = 0;
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] != '.') {
                struct stat fileStat;
                stat(dir->d_name, &fileStat);
                total_size += fileStat.st_blocks;
            }
        }
        closedir(d);
    }

    printf("total %d\n", total_size/2); // st_blocks is in 512-byte units, convert to 1K units

    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_name[0] != '.') {
                struct stat fileStat;
                stat(dir->d_name, &fileStat);

                if (long_listing) {
                    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
                    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
                    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
                    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
                    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
                    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");

                    struct passwd *pw = getpwuid(fileStat.st_uid);
                    struct group  *gr = getgrgid(fileStat.st_gid);
                    printf(" %hu", fileStat.st_nlink);
                    printf(" %s", pw->pw_name);
                    printf(" %s", gr->gr_name);
                    printf(" %5d", (int)fileStat.st_size);

                    char date[20];
                    strftime(date, 20, "%b %d %H:%M", localtime(&(fileStat.st_mtime)));
                    printf(" %s", date);
                }

                if (show_inode) {
                    printf("%lu ", (unsigned long)dir->d_ino);
                }

                printf(" %s\n", dir->d_name);
            }
        }
        closedir(d);
    }
    return 0;
}