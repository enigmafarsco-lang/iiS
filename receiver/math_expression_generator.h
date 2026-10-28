/**
 * @file math_expression_generator.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

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



/**
 * @class math_expression_generator
 * @brief High-level controller class for math_expression_generator.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class math_expression_generator
{
public:
    /**
     * @brief Constructs a new math_expression_generator instance and initializes subsystem components and UI state.
     */
    math_expression_generator();

    /**
     * @brief Executes the math expression get math function procedure for math_expression_generator.
     * @param expression_txt Input parameter specifying expression_txt (const char).
     * @param lib_handler Input parameter specifying lib_handler (void).
     * @param basenames Input parameter specifying basenames (GSList).
     * @return Output value of type math_function.
     */
    math_function math_expression_get_math_function(const char *expression_txt,void **lib_handler, GSList *basenames);
    /**
     * @brief Executes the math expression close lib handler procedure for math_expression_generator.
     * @param lib_handler Input parameter specifying lib_handler (void).
     */
    void math_expression_close_lib_handler(void *lib_handler);
    /**
     * @brief Executes the math expression objects clean procedure for math_expression_generator.
     */
    void math_expression_objects_clean(void);
    /**
     * @brief Executes the remove cb procedure for math_expression_generator.
     * @param fpath Input parameter specifying fpath (const char).
     * @param sb Input parameter specifying sb (const struct stat).
     * @param typeflag Input parameter specifying typeflag (int).
     * @param ftwbuf Input parameter specifying ftwbuf (struct FTW).
     * @return Output value of type static int.
     */
    static int remove_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
    /* Recursively remove the given path from the filesystem. */
    static int recursive_remove(char *dirpath);
    static gboolean eval(const GMatchInfo *info, GString *res, gpointer data);
    static char *string_replace(const char * string, const char *pattern,
                const char *replacement, GRegexEvalCallback eval);
    static char * c_file_create(const char *user_expression, GSList *basenames);
    /**
     * @brief Executes the shared object compile procedure for math_expression_generator.
     * @param base_filename Input parameter specifying base_filename (char).
     * @return Output value of type static int.
     */
    static int shared_object_compile(char *base_filename);


};

#endif // MATH_EXPRESSION_GENERATOR_H
