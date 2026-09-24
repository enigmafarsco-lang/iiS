#ifndef LIBINI_H
#define LIBINI_H

#include <receiver/ini.h>
#include <iio.h>
#include <stdio.h>
#include <stdint.h>

class libini2
{

private:

    void loops_params_init(struct loops_parse_params *p, FILE *i, FILE *o);
    bool ini_line_begins_with_keyword(char *line, char *extracted_keyword);
    int loop_expand(struct loops_parse_params *parse_params,
            char *buf_with_loop, char *loop_name);
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
    int ini_next_section(INI *ini, const char **name, size_t *name_len);
    static bool skip_comments(INI *ini);
    static bool skip_line(INI *ini);
    static int save_to_ini_chn_cb(struct iio_channel *chn,
            const char *attr, const char *val, size_t len, void *d);
    static int save_to_ini_dev_cb(struct iio_device *dev,
            const char *attr, const char *val, size_t len, void *d);
    static void write_to_ini(struct load_store_params *params, const char *dev_name,
            size_t name_len, const char *attr, const char *val, size_t len);

public:
    libini2();
    char *read_token_from_ini(const char *ini_file,const char *driver_name, const char *token);
    void update_from_ini(const char *ini_file,
            const char *driver_name, struct iio_device *dev,
            const char * const *whitelist, size_t list_len);

    void save_to_ini(FILE *f, const char *driver_name, struct iio_device *dev,
            const char * const *whitelist, size_t list_len);

    int foreach_in_ini(const char *ini_file,
            int (*cb)(int, const char *, const char *, const char *));

    int ini_unroll(const char *input, const char *output);

    void write_driver_name_to_ini(FILE *f, const char *driver_name);
};

#endif // LIBINI_H
