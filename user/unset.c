#include "lib.h"
#include "color.h"

int flag[256];

void
unset(char *name) {
    int r;

    if ((r = syscall_env_var(name, "", 3)) < 0) {
        fwritef(1, LIGHT_CYAN(Environment var) " " RED([%s]) " " LIGHT_CYAN( Is ReadOnly Or Not Exists!\n), name);
        return;
    }
	fwritef(1, LIGHT_CYAN(Environment var) " " RED([%s]) " " LIGHT_CYAN(Unset Successful!\n), name);
}

void
usage(void) {
    fwritef(1, "usage: unset [vars...]\n");
    exit();
}

void
umain(int argc, char **argv) {
    int i;
    if (argc == 0) {
        return;
    } else {
        for (i = 1; i < argc; i++)
            unset(argv[i]);
    }
}

