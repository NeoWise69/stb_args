#ifndef STB_ARGS_H
#define STB_ARGS_H
/* 
stb_args.h - header - only utility for easy handle command line arguments.
LISENCE: GNU Public Domain v3
First author: NeoWise69 [https://github.com/NeoWise69/stb_args]

simple example usage: (this one creates one global args context, and uses it)
int main(int argc, char** argv) {
     stba_create_context_static(argc, argv);
     ...
     arg program_name = stba_next();
     {
           arg second_arg = stba_next();
     }
     arg i_need_second_arg_again = stba_last();
     arg third_arg = stba_next();
     ...
     stba_destroy_context_static(); // always destroy context
}

little more advanced example usage:
int main(int argc, char** argv) {
     args_ctx* ctx = stba_create_context(argc, argv);
     ...
     arg program_name = stba_next(ctx);
     {
          arg second_arg = stba_next(ctx);
     }
      arg i_need_second_arg_again = stba_last(ctx);
      arg third_arg = stba_next(ctx);
      ...
      stba_destroy_context_static(ctx); // always destroy context
}

VersionList:
v0.01 - initial commit
*/

#include <stdlib.h> // calloc() free()
#include <string.h> // strlen()

typedef char*   arg; // minimal argument type
struct          args_ctx; // opaque context type

/**
 * create heap context, based on arguments you passed in.
 * always needs to be destroyed (calloc, free used).
 */
args_ctx*   stba_create_context(int argc, char** argv);

/**
 * static version of 'stba_create_context()'
 * create global context, user don't see it.
 */
void        stba_create_context_static(int argc, char** argv);

/**
 * return current argument and move "counter" forward to second one.
 */
arg         stba_next(args_ctx* args = 0);

/**
 * return just the current argument in "queue".
 */
arg         stba_current(args_ctx* args = 0);

/**
 * return last argument, if there was 'stba_next()' before it, e.g. in 
 * hidden code space of '{ ..code.. }'.
 */
arg         stba_last(args_ctx* args = 0);

/**
 * return count of arguments, may be used if here no forward access
 * to 'argc' constant.
 */
size_t      stba_count(args_ctx* args = 0);

/**
 * return data in passed position of 'n' parameter, if last one not 
 * on a out of bounds situation.
 */
arg         stba_data(size_t n, args_ctx* args = 0);

/**
 * destroys context, that was created by user.
 */
void        stba_destroy_context(args_ctx* args);

/**
 * destroys hidden context, if one was created.
 */
void        stba_destroy_context_static();

#ifdef STB_ARGS_IMPLEMENTATION
    
typedef struct args_ctx {
    arg* m_data;
    size_t  m_size;
    size_t  m_temp;
} args_ctx;

static struct args_ctx* g_args;

args_ctx* stba_create_context(int argc, char** argv) {
    args_ctx* args = (args_ctx*)(calloc)(1, sizeof(struct args_ctx));
    size_t i = 0;
    if (!!args) {
        args->m_data = (arg*)(calloc)(argc, sizeof(arg));
        if (!!args->m_data) {
            while (i < (args->m_size = argc)) (args->m_data[i++] = (arg)(calloc)(1, strlen(argv[i]))) = argv[i]; // some weird code here, needs refactoring
			return args;
        }
		return NULL;
    }
    return NULL;
}

void stba_create_context_static(int argc, char** argv) {
    g_args = stba_create_context(argc, argv);
}

arg stba_next(args_ctx* args) {
	if (args && args->m_temp < args->m_size)
        return args->m_data[args->m_temp++];
	if (g_args && g_args->m_temp < g_args->m_size)
		return g_args->m_data[g_args->m_temp++];
    return NULL;
}

arg stba_current(args_ctx* args) {
	if (args)
		return args->m_data[args->m_temp];
	if (g_args)
		return g_args->m_data[g_args->m_temp];
	return NULL;
}

arg stba_last(args_ctx* args) {
    if (args && args->m_temp > 0)
		return args->m_data[args->m_temp - 1];
	if (g_args && g_args->m_temp > 0)
		return g_args->m_data[g_args->m_temp - 1];
    return NULL;
}

size_t stba_count(args_ctx* args) {
	if (args)
		return args->m_size;
	if (g_args)
		return g_args->m_size;
    return -1;
}

arg stba_data(size_t n, args_ctx* args) {
    if (args && n <= args->m_size)
        return args->m_data[n];
    if (g_args && n <= g_args->m_size)
        return g_args->m_data[n];
    return 0;
}

void stba_destroy_context(args_ctx* args) {
    if (!!args) {
        if (args->m_data) free(args->m_data);
        free(args);
    }
}

void stba_destroy_context_static()
{
    stba_destroy_context(g_args);
}

#endif

#endif
