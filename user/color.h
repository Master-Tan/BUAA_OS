/* Define color modes */
#define BOLD_GREEN(str) "\033[0;32;32m\033[1m" # str "\033[m"
#define RED(str) "\033[0;32;31m" # str "\033[m"
#define LIGHT_RED(str) "\033[1;31m" # str "\033[m"
#define GREEN(str) "\033[0;32;32m" # str "\033[m"
#define LIGHT_GREEN(str) "\033[1;32m" # str "\033[m"
#define BLUE(str) "\033[0;32;34m" # str "\033[m"
#define LIGHT_BLUE(str) "\033[1;34m" # str "\033[m"
#define DARK_GRAY(str) "\033[1;30m" # str "\033[m"
#define CYAN(str) "\033[0;36m" # str "\033[m"
#define LIGHT_CYAN(str) "\033[1;36m" # str "\033[m"
#define PURPLE(str) "\033[0;35m" # str "\033[m"
#define LIGHT_PURPLE(str) "\033[1;35m" # str "\033[m"
#define BROWN(str) "\033[0;33m" # str "\033[m"
#define YELLOW(str) "\033[1;33m" # str "\033[m"
#define LIGHT_GRAY(str) "\033[0;37m" # str "\033[m"
#define WHITE(str) "\033[1;37m" # str "\033[m"

/*set_background_color*/
#define BACK_RED printf("\033[41m"); //red
#define BACK_GRE printf("\033[42m"); //green 
#define BACK_YEL printf("\033[43m"); //yellow
