//
//  util.h
//  linux-ls
//
//  Created by 谌杨 on 2018/11/23.
//  Copyright © 2018年 谌杨. All rights reserved.
//

#ifndef util_h
#define util_h

struct fileDetailInfo
{
    char fileName[1024];
    struct stat fileStat;
};

char *getWorkingDirectoryPath(void);
void optionsPrinter(char *option, struct fileDetailInfo fileDetail[], int structIndex);
void printSimpleList(char *filename, struct stat file);
char *getFileNameForPath(char *path);

#endif /* util_h */
