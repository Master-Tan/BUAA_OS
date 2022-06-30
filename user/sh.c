#include "color.h"
#include "lib.h"
#include <args.h>

int debug_ = 0;

//
// get the next token from string s
// set *p1 to the beginning of the token and
// *p2 just past the token.
// return:
//	0 for end-of-string
//	> for >
//	| for |
//	w for a word
//
// eventually (once we parse the space where the nul will go),
// words get nul-terminated.
#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"

int
_gettoken(char *s, char **p1, char **p2)
{
	int t;

	if (s == 0) {
		if (debug_ >= 1) writef(RED(GETTOKEN NULL\n));
		return 0;
	}

	if (debug_ >= 1) writef(RED(GETTOKEN: %s\n), s);

	*p1 = 0;
	*p2 = 0;

	while(strchr(WHITESPACE, *s))
		*s++ = 0;
	if(*s == 0) {
		if (debug_ >= 1) writef(RED(EOL\n));
		return 0;
	}

	// challenge begin
	if (*s == '\"') {
		*p1 = ++s;
		while (*s && (*s != '\"' && *(s - 1) != '\\')) {
			++s;
		}
		*s++ = 0;
		*p2 = s;
		return 'w';
	}
	// challenge end
	
	if(strchr(SYMBOLS, *s)){
		t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
		if (debug_ >= 1) writef(RED(TOK %c\n), t);
		return t;
	}
	*p1 = s;
	while(*s && !strchr(WHITESPACE SYMBOLS, *s))
		s++;
	*p2 = s;
	if (debug_ >= 1) {
		t = **p2;
		**p2 = 0;
		writef(RED(WORD: %s\n), *p1);
		**p2 = t;
	}
	return 'w';
}

int
gettoken(char *s, char **p1)
{
	static int c, nc;
	static char *np1, *np2;

	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	} else {   
	    c = nc; 
		*p1 = np1;
	    nc = _gettoken(np2, &np1, &np2);
		return c;
	}
}

#define MAXARGS 16
void
runcmd(char *s)
{
	history_save(s);
	char *argv[MAXARGS], *t;
	int argc, c, i, r, p[2], fd, rightpipe;
	int pid;
	int fdnum;
	int hangup;
	int input_fd = -1;
	int output_fd = -1;
	hangup = 0;
	rightpipe = 0;
	gettoken(s, 0);
again:
	argc = 0;
	for(;;){
		c = gettoken(0, &t);
		switch(c){
		case 0:
			goto runit;
		case 'w':
			if(argc == MAXARGS){
				writef("too many arguments\n");
				exit();
			}
			argv[argc++] = t;
			break;
		case '<':
			if(gettoken(0, &t) != 'w') {
				writef("syntax error: < not followed by word\n");
				exit();
			}
			// Your code here -- open t for reading,
			// dup it onto fd 0, and then close the fd you got.
            fdnum = open(t, O_RDONLY);
            dup(fdnum, 0);
            input_fd = fdnum;
			//close(fdnum);

			//user_panic("< redirection not implemented");
			break;
		case '>':
			if (gettoken(0, &t) != 'w') {
                writef("syntax error: > not followed by word\n");
                exit();
            }
			// Your code here -- open t for writing,
			// dup it onto fd 1, and then close the fd you got.
            if ((fdnum = open(t, O_WRONLY | O_CREAT)) < 0) {
				user_panic("> open failed");
			}
            //dup(fdnum, 1);
			output_fd = fd;
            //close(fdnum);

			//user_panic("> redirection not implemented");
			break;
		case '|':
			// Your code here.
			// 	First, allocate a pipe.
			//	Then fork.
			//	the child runs the right side of the pipe:
			//		dup the read end of the pipe onto 0
			//		close the read end of the pipe
			//		close the write end of the pipe
			//		goto again, to parse the rest of the command line
			//	the parent runs the left side of the pipe:
			//		dup the write end of the pipe onto 1
			//		close the write end of the pipe
			//		close the read end of the pipe
			//		set "rightpipe" to the child envid
			//		goto runit, to execute this piece of the pipeline
			//			and then wait for the right side to finish
			
			pipe(p);
            if ((rightpipe = fork()) == 0) {
				input_fd = p[0];
				//dup(p[0], 0);
                //close(p[0]);
                close(p[1]);
                goto again;
			} else {
				//dup(p[1], 1);
                output_fd = p[1];
				//close(p[1]);
				close(p[0]);
				goto runit;
			}
            break;
		
		// challenge begin
		case '&':
			hangup = 1;
			break;
		case ';':
			if ((pid = fork()) == 0) {
				hangup = 0;
				input_fd = -1;
				output_fd = -1;
				goto runit;
			}
			argc = 0;
			hangup = 0;
			rightpipe = 0;
			break;
		// challenge end	

		default:
			break;

			// user_panic("| not implemented");
			// break;
		}
	}

runit:
	if (input_fd != -1) {
		dup(input_fd, 0);
		close(input_fd);
	}
	if (output_fd != -1) {
		dup(output_fd, 1);
		close(output_fd);
	}
	if(argc == 0) {
		if (debug_) writef("EMPTY COMMAND\n");
		return;
	}
	argv[argc] = 0;
/*
	if (1) {
		writef("[%08x] SPAWN:", env->env_id);
		for (i=0; argv[i]; i++)
			writef(" %s", argv[i]);
		writef("\n");
	}
*/

	// spawn
	
	// challenge begin
	char prog_name[64];
    int prog_name_len = strlen(argv[0]);
    strcpy(prog_name, argv[0]);
    //prog_name[prog_name_len++] = '.';
	//prog_name[prog_name_len++] = 'b';
	strcat(prog_name, ".b");
    prog_name_len += 2;
    prog_name[prog_name_len] = '\0';

	int child_envid;
	if ((r = spawn(prog_name, argv)) < 0)
		writef("spawn %s: %e\n", argv[0], r);
	child_envid = r;

	if (hangup == 1) {
		writef(YELLOW([%d] HANG\t), child_envid);
		for (i = 0; i < argc; ++i) writef("%s ", argv[i]);
        writef("\n");
		//writef("Super Shell: command not found " RED([%s]) "\n", prog_name);
	}
	/*
	if ((r = syscall_set_env_status(child_envid, ENV_RUNNABLE)) < 0) {
        writef("set child runnable is wrong\n");
    }
	*/
	r = child_envid;
	// challenge end

	// spawn down
	close_all();
	if (r >= 0) {
		if (debug_ >= 1) writef("[%08x] WAIT %s %08x\n", env->env_id, argv[0], r);
		// challenge begin
		if (hangup == 0) {
			wait(r);
		} else {
			pid = fork();
			if (pid < 0) {
				writef("fork wrong!\n");
			}
			if (pid == 0) {
				wait(r);
				writef(LIGHT_BLUE(\n[%d] DONE\t), r);
				for (i = 0; i < argc; i++) {
					writef("%s ", argv[i]);
				}
				char curpath[MAXPATHLEN];
                curpath_get(curpath);
                writef("\n" LIGHT_BLUE(%s) " " BOLD_GREEN($) " ", curpath);
                writef("\b \b");
                exit();
			}
		}
	}
	if (rightpipe) {
		if (debug_ >= 1) writef("[%08x] WAIT right-pipe %08x\n", env->env_id, rightpipe);
		wait(rightpipe);
	}

	exit();
}

void
flush(char *buf) {
    int i;
    for (i = 0; i < strlen(buf); i++) writef("\b \b");
}


void
readline(char *buf, u_int n)
{
	int i, r, cmdi;
    char cmds[128][128];
	int cmdn = cmdi = history_read(cmds);
	//int cmdn = cmdi;
	r = 0;
	for(i=0; i<n; i++){
		if((r = read(0, buf+i, 1)) != 1){
			if(r < 0)
				writef("read error: %e", r);
			exit();
		}

		// challenge begin
		 if (i >= 2 && buf[i - 2] == 27 && buf[i - 1] == 91 && buf[i] == 65) { // Up arrow key
			i-=3;
			//writef(RED(UP));
			writef("%c%c%c", 27, 91, 66);
            for (i; i>=0; --i) writef("\b \b");

            if (cmdi) strcpy(buf, cmds[--cmdi]);
            else strcpy(buf, cmds[cmdi]);

            writef("%s", buf);
            i = strlen(buf) - 1;
        } else if (i >= 2 && buf[i - 2] == 27 && buf[i - 1] == 91 && buf[i] == 66) { // Down arrow key
			//writef(RED(DOWN));
			//writef("%c%c%c", 27, 91, 65);
            if (cmdi < cmdn - 1) {
				i -= 3;
                for (i; i>=0; --i) writef("\b \b");
                strcpy(buf, cmds[++cmdi]);
                writef("%s", buf);
            } else {
                buf[i - 2] = buf[i - 1] = buf[i] = '\0';
            }

            i = strlen(buf) - 1;
        }
		else if (i >= 2 && buf[i - 2] == 27 && buf[i - 1] == 91 && buf[i] == 68) { // Left arrow key
            writef("%c%c%c", 27, 91, 67);
			i-=3;
        }
		else if (i >= 2 && buf[i - 2] == 27 && buf[i - 1] == 91 && buf[i] == 67) { // Right arrow key
            writef("%c%c%c", 27, 91, 68);
			i-=3;
        }
		if (buf[i] == '\t') {
            writef("%c%c%c", 27, 91, 68);
            buf[i] = 0;
            i -= 1;
        }

        if (buf[i] == '\b' || buf[i] == 127) {
            if (i > 0) {
                buf[i] = 0;
                flush(buf);
                buf[i - 1] = 0;
                buf = strcat(buf, &buf[i]);
                writef("%s", buf);
                i -= 2;
            } else {
                buf[i] = 0;
                i -= 1;
            }
        }

		// challenge end
		/*
		if(buf[i] == '\b'){
			if(i > 0)
				i -= 2;
			else
				i = 0;
		}
		*/
		if(buf[i] == '\r' || buf[i] == '\n'){
			buf[i] = 0;
			return;
		}
	}
	writef("line too long\n");
	while((r = read(0, buf, 1)) == 1 && buf[0] != '\n')
		;
	buf[0] = 0;
}	

char buf[1024];

void
usage(void)
{
	writef("usage: sh [-dix] [command-file]\n");
	exit();
}

void
umain(int argc, char **argv)
{
	writef("\x1b[2J\x1b[H");
	int r, interactive, echocmds;
	interactive = '?';
	echocmds = 0;
	writef(LIGHT_CYAN(\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n));
	writef(LIGHT_CYAN(::) "                                                         " LIGHT_CYAN(::\n));
	writef(LIGHT_CYAN(::) "               ");
	writef("\033[1;35m");
	writef("Tan's Shell  V0.0.0_20373864");
	writef("\033[m");
	writef("              " LIGHT_CYAN(::\n));
	writef(LIGHT_CYAN(::) "                                                         " LIGHT_CYAN(::\n));
	writef(LIGHT_CYAN(:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n));
	ARGBEGIN{
	case 'd':
		debug_++;
		break;
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}ARGEND

	if(argc > 1)
		usage();
	if(argc == 1){
		close(0);
		if ((r = open(argv[1], O_RDONLY)) < 0)
			user_panic("open %s: %e", r);
		user_assert(r==0);
	}
	if(interactive == '?')
		interactive = iscons(0);
	
	// challenge begin
	history_init();
	//writef(RED(history_init end));
	curpath_init("/");
	syscall_env_var(0, 0, 7);
	// challenge end
	
	for(;;){
		char curpath[MAXPATHLEN];
		curpath_get(curpath);
		if (interactive) {
            writef("\n" LIGHT_BLUE(%s) " " BOLD_GREEN($) " ", curpath);
		}
			//			fwritef(1, GREEN(\n$ ));
		readline(buf, sizeof buf);
		
		if (buf[0] == '#')
			continue;
		if (echocmds)
			fwritef(1, "# %s\n", buf);
		if ((r = fork()) < 0)
			user_panic("fork: %e", r);
		if (r == 0) {
			runcmd(buf);
			exit();
			return;
		} else
			wait(r);
	}
}

