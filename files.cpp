//
// Created by xukaiwe on 18-2-23.
//

#include "files.h"
#include <string.h>
#include <dirent.h>
int readFileList(string basePath, vector<string> &files)
{
    DIR *dir;
    struct dirent *ptr;

    if ((dir=opendir(basePath.c_str())) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
            continue;
        else if(ptr->d_type == 8)
        {
//            string path = basePath;
//            path.append("/");
//            path.append(ptr->d_name);
            string file_name = ptr->d_name;
            files.push_back(file_name);
        }

    }
    closedir(dir);
    return 1;
}
