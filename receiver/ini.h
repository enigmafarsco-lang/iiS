/**
 * @file ini.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef __INI_H
#define __INI_H

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

typedef struct _INI  INI;

struct _INI {
    const char *buf, *end, *curr;
    bool free_buf_on_exit;
};

/**
 * @class ini
 * @brief High-level controller class for ini.
 * @details Manages state, algorithms, signals, and hardware interfaces.
 */
class ini
{

private:
    /**
     * @brief Executes the skip line procedure for ini.
     * @param ini Input parameter specifying ini (INI).
     * @return Output value of type static bool.
     */
    static bool skip_line(INI *ini);
    /**
     * @brief Executes the skip comments procedure for ini.
     * @param ini Input parameter specifying ini (INI).
     * @return Output value of type static bool.
     */
    static bool skip_comments(INI *ini);
    static INI *_ini_open_mem(const char *buf,
                size_t len, bool free_buf_on_exit);

public:
    /**
     * @brief Constructs a new ini instance and initializes subsystem components and UI state.
     */
    ini();
    INI *ini_open(const char *file);
    INI *ini_open_mem(const char *buf, size_t len);
    /**
     * @brief Executes the ini close procedure for ini.
     * @param ini Input parameter specifying ini (INI).
     */
    void ini_close(INI *ini);

    /* Jump to the next section.
     * if 'name' is set, the pointer passed as argument
     * points to the name of the section. 'name_len' is set to the length
     * of the char array.
     * XXX: the pointer will be invalid as soon as ini_close() is called.
     *
     * Returns:
     * 	-EIO if an error occured while reading the file,
     * 	0 if no more section can be found,
     * 	1 otherwise.
     */
    int ini_next_section(INI *ini, const char **name, size_t *name_len);

    /* Read a key/value pair.
     * 'key' and 'value' must be valid pointers. The pointers passed as arguments
     * will point to the key and value read. 'key_len' and 'value_len' are
     * set to the length of their respective char arrays.
     * XXX: the pointers will be invalid as soon as ini_close() is called.
     *
     * Returns:
     *  -EIO if an error occured while reading the file,
     *  0 if no more key/value pairs can be found,
     *  1 otherwise.
     */
    int ini_read_pair(INI *ini,
                      const char **key, size_t *key_len,
                      const char **value, size_t *value_len);

    /* Set the read head to a specified offset. */
    void ini_set_read_pointer(INI *ini,const char *pointer);

    /* Get the number of the line that contains the specified address.
     *
     * Returns:
     * -EINVAL if the pointer points outside the INI string,
     *  The line number otherwise.
     */
    int ini_get_line_number(INI *ini,const char *pointer);


};

#endif // __INI_H
