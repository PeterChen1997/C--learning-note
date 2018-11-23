//
//  main.c
//  linux-ls
//
//  Created by 谌杨 on 2018/11/23.
//  Copyright © 2018年 谌杨. All rights reserved.
//

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "util.h"

int main(int argc, const char *argv[])
{
    printf("========================================\n");
    printf("Welcome to the command line tool made by peter chen\n");
    printf("(support command: ls -i / -l / -S / -t)\n");
    printf("========================================\n");

    if (argc == 1 || argc == 2)
    {
        DIR *directory;
        int status;
        struct stat st;

        char *option = "";
        char *path = getWorkingDirectoryPath();

        printf("Current working directory: %s\n", path);
        printf("========================================\n");

        if (argc == 2)
        {
            option = strdup(argv[1]);
        }

        directory = opendir(path);
        status = stat(path, &st);
        size_t size = strlen(path);

        if (size > 0 && status != -1)
        {
            struct dirent *entry;
            char string[1024];

            struct stat st;
            int structIndex = 0;

            struct fileDetailInfo
            {
                char fileName[1024];
                struct stat fileStat;
            };
            struct fileDetailInfo fileDetail[100]; // 暂定只分析文件夹下的100个文件

            while ((entry = readdir(directory)) != NULL)
            {
                // jump . and ..
                if (strncmp(entry->d_name, ".", 1) != 0)
                {
                    sprintf(string, "%s/%s", path, entry->d_name);
                    stat(string, &st);

                    strcpy(fileDetail[structIndex].fileName, string);
                    memcpy(&fileDetail[structIndex].fileStat, &st, sizeof(struct stat));
                    structIndex++;
                }
            }
            closedir(directory);
            optionsPrinter(option, fileDetail, structIndex);
        }
        else
        {
            printf("Ops, there is something wrong in open dir...");
        }
    }
    else
    {
        printf("Please check you input command 233");
    }
    return 0;
}
