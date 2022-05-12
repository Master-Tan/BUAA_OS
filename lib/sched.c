#include <env.h>
#include <pmap.h>
#include <printf.h>

/* Overview:
 *  Implement simple round-robin scheduling.
 *
 *
 * Hints:
 *  1. The variable which is for counting should be defined as 'static'.
 *  2. Use variable 'env_sched_list', which is a pointer array.
 *  3. CANNOT use `return` statement!
 */
/*** exercise 3.15 ***/
void sched_yield(void)
{
	printf("\n");
    static int count = 0; // remaining time slices of current env
    static int point = 0; // current env_sched_list index
    
    /*  hint:
     *  1. if (count==0), insert `e` into `env_sched_list[1-point]`
     *     using LIST_REMOVE and LIST_INSERT_TAIL.
     *  2. if (env_sched_list[point] is empty), point = 1 - point;
     *     then search through `env_sched_list[point]` for a runnable env `e`, 
     *     and set count = e->env_pri
     *  3. count--
     *  4. env_run()
     *
     *  functions or macros below may be used (not all):
     *  LIST_INSERT_TAIL, LIST_REMOVE, LIST_FIRST, LIST_EMPTY
     */
	static int cur_lasttime = 1;
	static int cur_head_index = 0;
    struct Env *next_env;
    int now_have = -1;
    cur_lasttime--;
    if (cur_lasttime <= 0 || curenv == NULL || curenv->env_status != ENV_RUNNABLE) {
        now_have = -1;
        while(1) {
            if (LIST_EMPTY(&env_sched_list[cur_head_index])) {
                cur_head_index = (cur_head_index + 1) % 3;
                break;
            }
            next_env = LIST_FIRST(&env_sched_list[cur_head_index]);
            if (next_env->env_status == ENV_RUNNABLE) {
                now_have = cur_head_index;
				break;
            }
            LIST_REMOVE(next_env, env_sched_link);
			if (next_env->env_pri % 2 == 1) {
				LIST_INSERT_TAIL(&env_sched_list[((cur_head_index + 1) % 3)], next_env, env_sched_link);
			} else {
				LIST_INSERT_TAIL(&env_sched_list[((cur_head_index + 2) % 3)], next_env, env_sched_link);
			}
        }
        if (now_have == -1) {
            while (1) {
                if (LIST_EMPTY(&env_sched_list[cur_head_index])) {
                    cur_head_index = (cur_head_index + 1) % 3;
                }
                next_env = LIST_FIRST(&env_sched_list[cur_head_index]);
				if (next_env->env_status == ENV_RUNNABLE) {
       				now_have = cur_head_index;
                    break;
				}
				LIST_REMOVE(next_env, env_sched_link);
				if (next_env->env_pri % 2 == 1) {
             	   LIST_INSERT_TAIL(&env_sched_list[((cur_head_index + 1) % 3)], next_env, env_sched_link);
            	} else {
                	LIST_INSERT_TAIL(&env_sched_list[((cur_head_index + 2) % 3)], next_env, env_sched_link);
            	}
			}	
		}
		if (now_have == -1) {
            while (1) {
                if (LIST_EMPTY(&env_sched_list[cur_head_index])) {
					panic("^^^^^^No env is RUNNABLE!^^^^^^");
                }
                next_env = LIST_FIRST(&env_sched_list[cur_head_index]);
                if (next_env->env_status == ENV_RUNNABLE) {
                    now_have = cur_head_index;
                    break;
                }
                LIST_REMOVE(next_env, env_sched_link);
				if (next_env->env_pri % 2 == 1) {
             	   LIST_INSERT_TAIL(&env_sched_list[((cur_head_index + 1) % 3)], next_env, env_sched_link);
            	} else {
                	LIST_INSERT_TAIL(&env_sched_list[((cur_head_index + 2) % 3)], next_env, env_sched_link);
            	}
			}
        }
		LIST_REMOVE(next_env, env_sched_link);
        if (next_env->env_pri % 2 == 1) {
            LIST_INSERT_TAIL(&env_sched_list[((cur_head_index + 1) % 3)], next_env, env_sched_link);
        } else {
            LIST_INSERT_TAIL(&env_sched_list[((cur_head_index + 2) % 3)], next_env, env_sched_link);
        }
		if (now_have == 0) {
			cur_lasttime = next_env->env_pri * 1;
		} else if (now_have == 1){
			cur_lasttime = next_env->env_pri * 2;
		} else {
			cur_lasttime = next_env->env_pri * 4;
		}
        env_run(next_env);
        panic("^^^^^^sched yield jump faild^^^^^^");

	}
    env_run(curenv);
    panic("^^^^^^sched yield reached end^^^^^^");


/*
 * static int count = 0;
    static int point = 0;
    static struct Env *e = NULL;
    if (count == 0 || e == NULL || e->env_status != ENV_RUNNABLE) {
        if (e != NULL) {
            // move `e` to the tail of `env_sched_list[1 - point]` 
        }
        while (1) {
            while (LIST_EMPTY(&env_sched_list[point])) point = 1 - point;
            e = LIST_FIRST(&env_sched_list[point]);
            if (e->env_status == ENV_RUNNABLE) {
                count = e->env_pri; // PRIORITY
                break;
            } else {
                
                //  if the status of `e` is `ENV_NOT_RUNNABLE`,
                //      move it to the tail of `env_sched_list[1 - point]`
                 
            }
        }
    }
    count--;
    env_run(e);
*/

}
