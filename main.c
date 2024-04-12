#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 100
#define MAX_NAME_LENGTH 512

void initializeArray(char **array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = NULL;
    }
}
void addWhereEmpty(char ** array, char * str) {
    for(int i = 0; i < MAX_PATH_LENGTH; i++) {
        if(array[i] == NULL) {
            array[i] = str;
            break;
        }
    }
}

void listFiles(char ** pathArray, bool show_inode, bool long_listing, int fileCount) {
    if(fileCount == 0) {
        pathArray[0] = ".";
        fileCount = 1;
    }
    for(int i = 0; i < fileCount; i++) {
        if(pathArray[i] == NULL) {
            break;
        }
        DIR *d;
        struct dirent *dir;
        d = opendir(pathArray[i]);
        bool symlink = false;
        char target[MAX_PATH_LENGTH + MAX_NAME_LENGTH];
        if (d) {
            printf("%s:\n", pathArray[i]);
            while ((dir = readdir(d)) != NULL) {
                if (dir->d_name[0] != '.' && dir->d_name[0] != '/') {
                    char absPath[MAX_PATH_LENGTH];
                    if (realpath(pathArray[i], absPath) == NULL) {
                        perror("realpath");
                        break;
                    }
                    char fullPath[MAX_PATH_LENGTH + MAX_NAME_LENGTH];
                    snprintf(fullPath, sizeof(fullPath), "%s/%s", absPath, dir->d_name);
                    struct stat fileStat;
                    if (lstat(fullPath, &fileStat) == -1) {
                        perror("Error in lstat");
                        printf("UnixLs: %s: No such file or directory.\n\n", dir->d_name);
                        printf("%s\n", fullPath);
                        break;
                    }
                    if (show_inode) {
                        printf("%15lu ", (unsigned long)dir->d_ino);
                    }
                    if (long_listing) {
                        if (S_ISLNK(fileStat.st_mode)) {
                            printf("l");
                            ssize_t len = readlink(fullPath, target, sizeof(target) - 1);
                            if (len != -1) {
                                target[len] = '\0';
                                symlink = true;

                            } else {
                                perror("Error in reading symbolic link.");
                                break;
                            }
                        } else if(S_ISDIR(fileStat.st_mode)) {
                            printf("d");
                        } else {
                            printf("-");
                        }

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
                        if (pw == NULL) {
                            perror("Error in get user id");
                            break;
                        }
                        struct group  *gr = getgrgid(fileStat.st_gid);
                        if (gr == NULL) {
                            perror("Error in get group id");
                            break;
                        }
                        printf("\t%hu\t", fileStat.st_nlink);
                        printf(" %9s ", pw->pw_name);
                        printf(" %9s ", gr->gr_name);
                        printf(" %9d ", (int)fileStat.st_size);
                        char date[20];
                        strftime(date, 20, "%b %d %Y %H:%M", localtime(&(fileStat.st_mtime)));
                        printf(" %21s  ", date);
                    }
                    if (symlink) {
                        printf(" %s -> %s\n", dir->d_name, target);
                        symlink = false;
                    } else {
                        printf(" %s\n", dir->d_name);
                    }
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
    initializeArray(paths, MAX_PATH_LENGTH);
    int fileCount = 0;
    for(int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0 && flag) {
            show_inode = true;
            continue;
        }
        if (strcmp(argv[i], "-l") == 0 && flag) {
            long_listing = true;
            continue;
        }
        if((strcmp(argv[i], "-il") == 0 || strcmp(argv[i], "-li") == 0) && flag) {
            show_inode = true;
            long_listing = true;
            continue;
        }
        else {
            addWhereEmpty(paths, argv[i]);
            flag = false;
            fileCount++;
        }
    }
    listFiles(paths, show_inode, long_listing, fileCount);
    return 0;
}

