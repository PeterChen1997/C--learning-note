//
//  util.c
//  linux-ls
//
//  Created by 谌杨 on 2018/11/23.
//  Copyright © 2018年 谌杨. All rights reserved.
//
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

struct fileDetailInfo
{
    char fileName[1024];
    struct stat fileStat;
};

char *getFileNameForPath(char *path)
{
    return basename(path);
}

// (ls command) print simple list
void printSimpleList(struct fileDetailInfo *fileDetail, int structIndex)
{
    for (; structIndex > 0; structIndex--)
    {
        printf("%s\n", getFileNameForPath(fileDetail[structIndex - 1].fileName));
    }
}

// (ls -i command) print inode list
void printInodeList(struct fileDetailInfo *fileDetail, int structIndex)
{
    for (; structIndex > 0; structIndex--)
    {
        printf("%-10lli %s \t\n",
               fileDetail[structIndex - 1].fileStat.st_ino,
               getFileNameForPath(fileDetail[structIndex - 1].fileName));
    }
}

char *getUserNameFromUid(uid_t uid)
{
    struct passwd *pw_str;
    pw_str = getpwuid(uid);
    return pw_str->pw_name;
}

char *getUserNameFromGid(gid_t gid)
{
    struct group *grp_ptr;
    grp_ptr = getgrgid(gid);
    return grp_ptr->gr_name;
}

char *getFilePropertyStrFromMode(int mode, char modeStr[])
{
    strcpy(modeStr, "----------");

    if (S_ISDIR(mode))
        modeStr[0] = 'd';
    if (S_ISCHR(mode))
        modeStr[0] = 'c';
    if (S_ISBLK(mode))
        modeStr[0] = 'b';

    if (mode & S_IRUSR)
        modeStr[1] = 'r';
    if (mode & S_IWUSR)
        modeStr[2] = 'w';
    if (mode & S_IXUSR)
        modeStr[3] = 'x';

    if (mode & S_IRGRP)
        modeStr[4] = 'r';
    if (mode & S_IWGRP)
        modeStr[5] = 'w';
    if (mode & S_IXGRP)
        modeStr[6] = 'x';

    if (mode & S_IROTH)
        modeStr[7] = 'r';
    if (mode & S_IWOTH)
        modeStr[8] = 'w';
    if (mode & S_IXOTH)
        modeStr[9] = 'x';

    return modeStr;
}

// (ls -l command) print detailed list
void printDetailedList(struct fileDetailInfo *fileDetail, int structIndex)
{
    char modeStr[11] = "----------";
    struct tm *htime;

    for (; structIndex > 0; structIndex--)
    {
        // process mode
        getFilePropertyStrFromMode(fileDetail[structIndex - 1].fileStat.st_mode, modeStr);

        printf("%-11s %-1d %-9s %-6s %5lld \t",
               modeStr,
               fileDetail[structIndex - 1].fileStat.st_nlink,
               getUserNameFromUid(fileDetail[structIndex - 1].fileStat.st_uid),
               getUserNameFromGid(fileDetail[structIndex - 1].fileStat.st_gid),
               fileDetail[structIndex - 1].fileStat.st_size);

        // process time
        htime = localtime(&fileDetail[structIndex - 1].fileStat.st_mtime);

        printf("%02d %02d %02d:%02d \t",
               htime->tm_mon + 1,
               htime->tm_mday,
               htime->tm_hour,
               htime->tm_min);

        printf(" %s\n", getFileNameForPath(fileDetail[structIndex - 1].fileName));
    }
}

int sortBySize(struct fileDetailInfo *fileOne, struct fileDetailInfo *fileTwo)
{
    if (fileOne->fileStat.st_size > fileTwo->fileStat.st_size)
    {
        return 1;
    }
    return -1;
}
// (ls -S command) print size-sorted list
void printSizeSortedList(struct fileDetailInfo fileDetail[], int structIndex)
{
    qsort(fileDetail, structIndex, sizeof(fileDetail[0]), sortBySize);
    printSimpleList(fileDetail, structIndex);
}

int sortByModifiedTime(struct fileDetailInfo *fileOne, struct fileDetailInfo *fileTwo)
{
    if (fileOne->fileStat.st_mtime > fileTwo->fileStat.st_mtime)
    {
        return 1;
    }
    return -1;
}
// (ls -t command) print modified-time sorted list
void printModiTimeSortedList(struct fileDetailInfo fileDetail[], int structIndex)
{
    qsort(fileDetail, structIndex, sizeof(fileDetail[0]), sortByModifiedTime);
    printSimpleList(fileDetail, structIndex);
}

char *getWorkingDirectoryPath()
{
    char *path;
    long size;
    char *buffer = NULL;

    // long pathconf( const char* path，int name );
    // _PC_PATH_MAX: 相对路径名的最大字节数，包括null
    size = pathconf(".", _PC_PATH_MAX);
    // char * getcwd(char * buf, size_t size);
    // get current working directory
    path = getcwd(buffer, (size_t)size);

    return path;
}

void optionsPrinter(char *option, struct fileDetailInfo fileDetail[], int structIndex)
{
    if (strcmp(option, "-l") == 0)
    {
        printDetailedList(fileDetail, structIndex);
    }
    else if (strcmp(option, "-S") == 0)
    {
        printSizeSortedList(fileDetail, structIndex);
    }
    else if (strcmp(option, "-i") == 0)
    {
        printInodeList(fileDetail, structIndex);
    }
    else if (strcmp(option, "-t") == 0)
    {
        printModiTimeSortedList(fileDetail, structIndex);
    }
    else
    {
        printSimpleList(fileDetail, structIndex);
    }
}
