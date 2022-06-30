#include "color.h"
#include "lib.h"

int flag[256];

void
tree(char *path) {
	fwritef(1, GREEN(.)" "PURPLE(%s\n), path);
	walk(path, 0, -1);
	/*
    int r, fd;
    struct Stat st;
    if ((r = stat(path, &st)) < 0)
        user_panic("stat %s: %e", path, r);
    if (st.st_isdir && !flag['d']) {
        tree_start(path, -1);
    } else {
        tree_start(path, -1);
        //ls1(0, st.st_isdir, st.st_size, path);
    }
	*/
}
/*
void
tree_start(char *path, u_int recursive) {
    fwritef(1, GREEN(.)" "PURPLE(%s\n), path);
    walk(path, 0, recursive);
    return;
}
*/
void
walk(char *path, int level, int rec) {
    int fd, n;
    struct File f;
    struct File *dir;
    char new[MAXPATHLEN] = {0};
    if (rec == 0)
        return;
    if ((fd = open(path, O_RDONLY)) < 0)
        user_panic("open %s: %e", path, fd);
    while ((n = readn(fd, &f, sizeof f)) == sizeof f) {
        if (f.f_name[0]) {
            dir = &f;
            int i = 0;
		    for (i = 0; i < (level * 4); i++) fwritef(1, "%c", ' ');
			fwritef(1, YELLOW(|-- ));
            if (dir->f_type == FTYPE_REG)
                fwritef(1, "%s", dir->f_name);
            else
                fwritef(1, LIGHT_CYAN(%s), dir->f_name);

            fwritef(1, "\n");

            if (dir->f_type == FTYPE_DIR) {
                strcpy(new, path);
                strcat(new, "/");
                strcat(new, f.f_name);
                walk(new, level + 1, rec - 1);
            }
        }
    }
}
/*
void printline(char r, int n, int ret) {
    int i = 0;
    for (i = 0; i < n; i++) fwritef(1, "%c", r);
    if (ret) fwritef(1, "\n");
}
*/
void usage(void) {
    fwritef(1, "usage: tree [PATH]\n");
    exit();
}

void
umain(int argc, char **argv) {
	ARGBEGIN
    {
        default:
            usage();
        break;
    }
    ARGEND

    if (argc == 0) {
        tree("/");
    } else if (argc == 1) {
		tree(argv[0]);	
	} else {
		writef(RED(arg error!\n));
	}

}

