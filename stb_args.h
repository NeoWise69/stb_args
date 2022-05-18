#ifndef STB_ARGS_H
#define STB_ARGS_H

/* 
stb_args.h - header-only utility for easy handle command line arguments.
LISENCE: Apache Lisence 2.0
First author: NeoWise69 [https://github.com/NeoWise69/stb_args]

simple example usage: (this one creates one global args context, and uses it)
int main(int argc, char** argv) {
	stba_init_global(argc, argv);
	...
	arg program_name = stba_next();
	{
		arg second_arg = stba_next();
	}
	arg i_need_second_arg_again = stba_last();
	arg third_arg = stba_next();
	...
	stba_shutdown_global();
}

little more advanced example usage:
int main(int argc, char** argv) {
	args_t* args = stba_init(argc, argv);
	...
	arg program_name = stba_next(args);
	{
		arg second_arg = stba_next(args);
	}
	arg i_need_second_arg_again = stba_last(args);
	arg third_arg = stba_next(args);
	...
	stba_shutdown(args);
}

VersionList:
v0.01 - initial commit
v0.02 - code cleanup + using strdup
*/

typedef char*  arg;           // minimal argument type
typedef struct args_t args_t; // opaque context type

/**
 * create heap context, based on arguments you passed in.
 * always needs to be destroyed (calloc, free used).
 */
args_t* stba_init(int argc, char** argv);

/**
 * static version of 'stba_init()'
 * create global context, user don't see it.
 */
void    stba_init_global(int argc, char** argv);

/**
 * return current argument and move "counter" forward to second one.
 */
arg     stba_next(args_t* args = 0);

/**
 * return just the current argument in "queue".
 */
arg     stba_current(args_t* args = 0);

/**
 * return last argument, if there was 'stba_next()' before it, e.g. in 
 * hidden code space of '{ ..code.. }'.
 */
arg     stba_last(args_t* args = 0);

/**
 * return count of arguments, may be used if here no forward access
 * to 'argc' constant.
 */
size_t  stba_count(args_t* args = 0);

/**
 * return data in passed position of 'n' parameter, if last one not 
 * on a out of bounds situation.
 */
arg     stba_data(size_t n, args_t* args = 0);

/**
 * destroys context, that was created by user.
 */
void    stba_shutdown(args_t* args);

/**
 * destroys hidden context, if one was created.
 */
void    stba_shutdown_global();

// IMPLEMENTATION

#include <stdlib.h> // calloc() free()
#include <string.h> // _strdup()

#ifdef STB_ARGS_LEGACY_FUNCS
#define args_calloc(size)   
#define args_free(block)    
#define args_strdup(str)    
#else
#define args_calloc(num, size) calloc(num, size)
#define args_free(block) free(block)
#define args_strdup(str) _strdup(str)
#endif

static struct args_t* g_a;
    
typedef struct __args_ctx {
    arg* m_data;
    size_t  m_size;
    size_t  m_temp;
} __args_ctx;

args_t* stba_init(int argc, char** argv) {
    __args_ctx* ctx = (__args_ctx*)args_calloc(1, sizeof(struct __args_ctx));
    size_t i = 0;
    if (ctx) {
        ctx->m_data = (arg*)args_calloc(argc, sizeof(arg));
        if (ctx->m_data) {
            while (i < (ctx->m_size = argc)) 
                ctx->m_data[i++] = args_strdup(argv[i]);
			return (args_t*)(ctx);
        }
		return NULL;
    }
    return NULL;
}

void stba_init_global(int argc, char** argv) {
    g_a = stba_init(argc, argv);
}

arg stba_next(args_t* a) {
    __args_ctx* args = (__args_ctx*)(a);
    __args_ctx* g_args = (__args_ctx*)(g_a);

	if (args && args->m_temp < args->m_size)
        return args->m_data[args->m_temp++];
	if (g_args && g_args->m_temp < g_args->m_size)
		return g_args->m_data[g_args->m_temp++];
    return NULL;
}

arg stba_current(args_t* a) {
	__args_ctx* args = (__args_ctx*)(a);
	__args_ctx* g_args = (__args_ctx*)(g_a);

	if (args)
		return args->m_data[args->m_temp];
	if (g_args)
		return g_args->m_data[g_args->m_temp];
	return NULL;
}

arg stba_last(args_t* a) {
	__args_ctx* args = (__args_ctx*)(a);
	__args_ctx* g_args = (__args_ctx*)(g_a);

	if (args && args->m_temp > 0)
		return args->m_data[args->m_temp - 1];
	if (g_args && g_args->m_temp > 0)
		return g_args->m_data[g_args->m_temp - 1];
    return NULL;
}

size_t stba_count(args_t* a) {
	__args_ctx* args = (__args_ctx*)(a);
	__args_ctx* g_args = (__args_ctx*)(g_a);

	if (args)
		return args->m_size;
	if (g_args)
		return g_args->m_size;
    return -1;
}

arg stba_data(size_t n, args_t* a) {
	__args_ctx* args = (__args_ctx*)(a);
	__args_ctx* g_args = (__args_ctx*)(g_a);

	if (args && n <= args->m_size)
        return args->m_data[n];
    if (g_args && n <= g_args->m_size)
        return g_args->m_data[n];
    return 0;
}

void stba_shutdown(args_t* a) {
	__args_ctx* args = (__args_ctx*)(a);

	if (args) {
		if (args->m_data) args_free(args->m_data);
        args_free(args);
    }
}

void stba_shutdown_global()
{
    stba_shutdown(g_a);
}

#endif
