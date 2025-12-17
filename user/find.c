#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char *path, char* fileToSearch){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // Fills file descriptor with current directory
    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type){

    // device file (don't know what this is)
    case T_DEVICE:

    // File
    case T_FILE:
        if(strcmp(fileToSearch, path))
            printf("that %s %s\n",path, fileToSearch);
        
        break;

    // Directory
    case T_DIR:
        
        // if path longer than buffer size
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("ls: path too long\n");
            break;
        }

        // copy into buffer
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';

        // Iterate through the entire file directory
        while(read(fd, &de, sizeof(de)) == sizeof(de)){

            // if . or .. skip over 
            if(de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, "..") )
                continue;


            // ./ + de.name for length of DIRSIZ
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            // if Stat is not valid
            // Loads stat too
            if(stat(buf, &st) < 0){
                    printf("ls: cannot stat %s\n", buf);
                    continue;
            }

            // check if stat is of type dir
            if(st.type == T_DIR){
                // start find in subdirectory
                find(buf, fileToSearch);
            } else {
            // Else if file matches file to search
                if(!strcmp(de.name, fileToSearch))
                    printf("%s\n", buf);
                }

            }
        break;
    }

    close(fd);
}

/** 
Pseudo code: 
1. Iterate through current directory 
2. if file and match print
3. if directory, fork process and repeat find on sub-directory back to 1)
4. Wait for all the processes to finish
*/

int main(int argc, char *argv[]){

    if(argc < 2){
        return exit(0);
    }

    char* path = argv[1];
    char* fileToSearch = argv[2];
    find(path, fileToSearch);

    exit(0);
}