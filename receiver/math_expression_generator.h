#ifndef MATH_EXPRESSION_GENERATOR_H
#define MATH_EXPRESSION_GENERATOR_H

#include <glib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <ftw.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>
#include <glib-object.h>

#define MATH_OBJECT_FILES_DIR "math_expressions"
#define MATH_EXPRESSION_BASE_FILE "math_expression"
#define MATH_FUNCTION_NAME "expression_function"

typedef void (*math_function)(float ***channels_data, float *out_data, unsigned long long chn_sample_cnt);



class math_expression_generator
{
public:
    math_expression_generator();

    math_function math_expression_get_math_function(const char *expression_txt,void **lib_handler, GSList *basenames);
    void math_expression_close_lib_handler(void *lib_handler);
    void math_expression_objects_clean(void);
    static int remove_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
    /* Recursively remove the given path from the filesystem. */
    static int recursive_remove(char *dirpath);
    static gboolean eval(const GMatchInfo *info, GString *res, gpointer data);
    static char *string_replace(const char * string, const char *pattern,
                const char *replacement, GRegexEvalCallback eval);
    static char * c_file_create(const char *user_expression, GSList *basenames);
    static int shared_object_compile(char *base_filename);


};

#endif // MATH_EXPRESSION_GENERATOR_H
