#include "color.h"
#include "lib.h"

int flag[256];
char buf[8192];

void
declare(char *namevalue) {
	if (!namevalue && !flag['x'] && !flag['r']) {
		export();
	} else if (flag['x']) {
		if (!namevalue) {
			writef(RED(declare arg error));
		} else {
			char name[100];
            char value[100];
            char *i = namevalue;
			int cnt = 0;
            int flag1 = 0;
            while (*i != '\0') {
				if (*i == ' ') {
					i++;
					continue;
				}
				if (*i == '=') {
					name[cnt] = '\0';
					cnt = 0;
					flag1 = 1;
                } else {
                    if (flag1 == 0) {
                        name[cnt++] = *i;
                    } else {
                        value[cnt++] = *i;
                    }
                }
                i++;
            }
            value[cnt] = '\0';
			if (flag['r']) {
				syscall_env_var(name, value, 5);	
				writef(LIGHT_CYAN(Set Readonly Environment Var) ": ");
			} else {
				syscall_env_var(name, value, 0);
				writef(LIGHT_CYAN(Set Environment Var) ": ");
			}
			writef(RED(%s = %s\n), name, value);
		}
	} else {
		if (!namevalue) {
			writef(RED(declare arg error));
		} else {
			char name[100];
            char value[100];
            char *i = namevalue;
            int cnt = 0;
            int flag1 = 0;
            while (*i != '\0') {
                if (*i == ' ') {
                    i++;
                    continue;
                }
                if (*i == '=') {
                    name[cnt] = '\0';
                    cnt = 0;
                    flag1 = 1;
                } else {
                    if (flag1 == 0) {
                        name[cnt++] = *i;
                    } else {
                        value[cnt++] = *i;
                    }
                }
                i++;
            }
            value[cnt] = '\0';
			if (flag['r']) {
                syscall_env_var(name, value, 5);
				syscall_env_var(name, value, 6);
				writef(LIGHT_CYAN(Set Readonly Local Var) ": ");
            } else {
                syscall_env_var(name, value, 0);
				syscall_env_var(name, value, 6);
				writef(LIGHT_CYAN(Set Local Var) ": ");
            }
			writef(RED(%s = %s\n), name, value);
		}
	}
/*
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
*/
}

void
export() {

	writef(LIGHT_CYAN(-- -- Environment Variables -- --\n));
    syscall_env_var(0, 0, 4);
	writef(LIGHT_CYAN(-- --    Local Variables    -- --\n));
	syscall_env_var(0, 0, 8);

/*
	char *name_table[1 << 8];
    char *value_table[1 << 8];
    syscall_env_var(name_table, value_table, 4);
    int i = 0;
    while (name_table[i]) {
        fwritef(1, GREEN(%s) " = %s\n", name_table[i], value_table[i]);
        ++i;
    }

*/
}

/*
void
export_i(char *name) {
    char value[256];
    syscall_env_var(name, value, 1);
    fwritef(1, GREEN(%s) " = %s\n", name, value);
}
*/


void
usage(void) {
    fwritef(1, "usage: declare [-xr] [NAME [=VALUE]]\n");
    exit();
}
void
umain(int argc, char **argv) {
    int i;
    ARGBEGIN
    {
        default:
            usage();
        case 'x':
        case 'r':
            flag[(u_char) ARGC()]++;
        break;
    }
    ARGEND

    if (argc == 0) {
        declare(0);
    } else {
        for (i = 0; i < argc; i++)
			declare(argv[i]);
    }
}

