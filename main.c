#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <stdbool.h>

#define MAX_PATH_LENGTH 100
#define MAX_NAME_LENGTH 500

void addWhereEmpty(char ** array, char * str) {
    for(int i = 0; i < MAX_PATH_LENGTH; i++) {
        if(array[i] == NULL) {
            array[i] = str;
            break;
        }
    }
}

void listFiles(char ** pathArray, bool show_inode, bool long_listing) {
    if(pathArray[0] == NULL) {
        pathArray[0] = ".";
    }
    for(int i = 0; i < sizeof(pathArray); i++) {
        if(pathArray[i] == NULL) {
            break;
        }
        DIR *d;
        struct dirent *dir;
        d = opendir(pathArray[i]);
        if (d) {
            printf("%s:\n", pathArray[i]);
            while ((dir = readdir(d)) != NULL) {
                if (dir->d_name[0] != '.') {
                    char fullPath[MAX_PATH_LENGTH + MAX_NAME_LENGTH];
                    snprintf(fullPath, sizeof(fullPath), "%s/%s", pathArray[i], dir->d_name);
                    struct stat fileStat;
                    stat(fullPath, &fileStat);
                    if (show_inode) {
                        printf("%lu  ", (unsigned long)dir->d_ino);
                    }
                    if (long_listing) {
                        switch (fileStat.st_mode & S_IFMT) {
                            case S_IFBLK:  printf("b "); break;
                            case S_IFCHR:  printf("c "); break;
                            case S_IFDIR:  printf("d "); break; //It's a (sub)directory
                            case S_IFIFO:  printf("p "); break; //fifo
                            case S_IFLNK:  printf("l "); break; //Sym link
                            case S_IFSOCK: printf("s "); break;
                            default:       printf("- "); break;
                        }
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
                        printf(" %3hu ", fileStat.st_nlink);
                        printf(" %5s ", pw->pw_name);
                        printf(" %7s ", gr->gr_name);
                        printf(" %6d ", (int)fileStat.st_size);
                        char date[20];
                        strftime(date, 20, "%b %d %Y %H:%M", localtime(&(fileStat.st_mtime)));
                        printf(" %11s  ", date);
                    }
                    printf("%s\n", dir->d_name);
                }
            }
            closedir(d);
        } else {
            printf("UnixLs: %s: No such file or directory.\n", pathArray[i]);
        }
    }
}

int main(int argc, char *argv[]) {
    bool show_inode = false;
    bool long_listing = false;
    bool flag = true;
    char *paths[MAX_PATH_LENGTH];
    for(int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "-i") == 0) && flag) {
            show_inode = true;
        }
        if ((strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "-l") == 0) && flag) {
            long_listing = true;
        }
        if((strcmp(argv[i], "-il") == 0 || strcmp(argv[i], "-li") == 0) && flag) {
            show_inode = true;
            long_listing = true;
        }
        if(argv[i][0] != '-') {
            addWhereEmpty(paths, argv[i]);
            flag = false;
        }
    }
    listFiles(paths, show_inode, long_listing);
    return 0;
}

