/**
 * @file libini2.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef LIBINI_H
#define LIBINI_H

#include <receiver/ini.h>
#include <iio.h>
#include <stdio.h>
#include <stdint.h>

/**
 * @class libini2
 * @brief High-level controller class for libini2.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class libini2
{

private:

    /**
     * @brief Executes the loops params init procedure for libini2.
     * @param p Input parameter specifying p (struct loops_parse_params).
     * @param i Input parameter specifying i (FILE).
     * @param o Input parameter specifying o (FILE).
     */
    void loops_params_init(struct loops_parse_params *p, FILE *i, FILE *o);
    /**
     * @brief Executes the ini line begins with keyword procedure for libini2.
     * @param line Input parameter specifying line (char).
     * @param extracted_keyword Input parameter specifying extracted_keyword (char).
     * @return Output value of type bool.
     */
    bool ini_line_begins_with_keyword(char *line, char *extracted_keyword);
    int loop_expand(struct loops_parse_params *parse_params,
            char *buf_with_loop, char *loop_name);
    /**
     * @brief Executes the ini loop new procedure for libini2.
     * @param buf_with_loop Input parameter specifying buf_with_loop (char).
     * @param loop_name Input parameter specifying loop_name (char).
     * @return Output value of type struct ini_loop *.
     */
    struct ini_loop * ini_loop_new(char *buf_with_loop, char *loop_name);
    char *ini_loop_get_iteration(struct ini_loop *loop);
    static ssize_t update_from_ini_chn_cb(struct iio_channel *chn,
            const char *attr, void *buf, size_t len, void *d);
    static ssize_t update_from_ini_dev_cb(struct iio_device *dev,
            const char *attr, void *buf, size_t len, void *d);
    static bool attr_in_whitelist(const char *attr,
            const char *dev_name, size_t dev_len, bool is_debug,
            const char * const *whitelist, size_t list_len);
    static ssize_t read_from_ini(struct load_store_params *params,
            const char *dev_name, size_t name_len,
            const char *attr, void *buf, size_t len);
    static bool attr_matches(const char *dev_name, size_t dev_len,
            const char *attr, size_t attr_len,
            const char *key, size_t len, bool debug);
    /**
     * @brief Executes the ini next section procedure for libini2.
     * @param ini Input parameter specifying ini (INI).
     * @param name Input parameter specifying name (const char).
     * @param name_len Input parameter specifying name_len (size_t).
     * @return Output value of type int.
     */
    int ini_next_section(INI *ini, const char **name, size_t *name_len);
    /**
     * @brief Executes the skip comments procedure for libini2.
     * @param ini Input parameter specifying ini (INI).
     * @return Output value of type static bool.
     */
    static bool skip_comments(INI *ini);
    /**
     * @brief Executes the skip line procedure for libini2.
     * @param ini Input parameter specifying ini (INI).
     * @return Output value of type static bool.
     */
    static bool skip_line(INI *ini);
    static int save_to_ini_chn_cb(struct iio_channel *chn,
            const char *attr, const char *val, size_t len, void *d);
    static int save_to_ini_dev_cb(struct iio_device *dev,
            const char *attr, const char *val, size_t len, void *d);
    static void write_to_ini(struct load_store_params *params, const char *dev_name,
            size_t name_len, const char *attr, const char *val, size_t len);

public:
    /**
     * @brief Constructs a new libini2 instance and initializes subsystem components and UI state.
     */
    libini2();
    char *read_token_from_ini(const char *ini_file,const char *driver_name, const char *token);
    void update_from_ini(const char *ini_file,
            const char *driver_name, struct iio_device *dev,
            const char * const *whitelist, size_t list_len);

    void save_to_ini(FILE *f, const char *driver_name, struct iio_device *dev,
            const char * const *whitelist, size_t list_len);

    int foreach_in_ini(const char *ini_file,
            /**
             * @brief Executes the int procedure for libini2.
             * @param cb Input parameter specifying cb (*cb).
             */
            int (*cb)(int, const char *, const char *, const char *));

    /**
     * @brief Executes the ini unroll procedure for libini2.
     * @param input Input parameter specifying input (const char).
     * @param output Input parameter specifying output (const char).
     * @return Output value of type int.
     */
    int ini_unroll(const char *input, const char *output);

    /**
     * @brief Transmits write_driver_name_to_ini command/packet over the communication bus.
     * @param f Input parameter specifying f (FILE).
     * @param driver_name Input parameter specifying driver_name (const char).
     */
    void write_driver_name_to_ini(FILE *f, const char *driver_name);
};

#endif // LIBINI_H
