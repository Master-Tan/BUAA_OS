#include "color.h"
#include "lib.h"

int flag[256];

void
touch(char *path, char *prefix) {
    int r, fd;
    char curpath[MAXPATHLEN] = {'\0'};

    if ((r = curpath_get(curpath)) < 0) {
        fwritef(1, "mkdir: cannot get environment var [curpath]\n");
    }

    if (path[0] == '/') {
        // Do Nothing
    } else {
        if (curpath[strlen(curpath) - 1] != '/')
            strcat(curpath, "/");
        strcat(curpath, path);
    }
    fd = open(curpath, O_RDONLY);
    if (fd >= 0)
    {
        fwritef(1, RED(Directory %s duplicated\n), curpath);
        return;
    }

    if ((r = create(curpath, FTYPE_REG)) < 0) {
        fwritef(1, "File %s Already Exists!\n", curpath);
        return;
    }
    fwritef(1, RED(File %s created!), curpath);
}

void
usage(void) {
    fwritef(1, "usage: touch [file...]\n");
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
            touch(argv[i], argv[i]);
    }
}

