#include "color.h"
#include "lib.h"

int flag[256];

void
mkdir(char *path) {
    int r, fd; 
    struct Stat st; 
    char curpath[MAXPATHLEN] = {'\0'};

    if ((r = curpath_get(curpath)) < 0) {
        fwritef(1, "mkdir: cannot get environment var [curpath]\n");
    }   
    if (path[0] == '/') {
        strcpy(curpath, path);
    } else {
        if (curpath[strlen(curpath) - 1] != '/')
            strcat(curpath, "/");
        strcat(curpath, path);
    }   
    fd = open(curpath, O_RDONLY);
    if (fd >= 0)
    {   
        fwritef(1, RED(Directory %s duplicated!\n), curpath);
        return;
    }     
    if ((r = create(curpath, FTYPE_DIR)) < 0) {
        fwritef(1, "Directory %s Already Exists!\n", curpath);
        return;
    }
    fwritef(1, RED(Created Directory %s!), curpath);
}

void
usage(void) {
    fwritef(1, "usage: mkdir [file...]\n");
    exit();
}

void
umain(int argc, char **argv) {
    int i;
    ARGBEGIN
    {   
        default:
            usage();
        break;
    }
    ARGEND

    if (argc == 0) {
        return;
    } else {
        for (i = 0; i < argc; i++)
            mkdir(argv[i]);
    }
}
 
