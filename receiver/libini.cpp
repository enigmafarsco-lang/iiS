#include "ini.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

ini::ini()
{

}

bool ini::skip_line(INI *ini)
{
    const char *curr = ini->curr;
    const char *end = ini->end;

    for (; curr != end && *curr != '\n'; curr++);
    if (curr == end) {
        ini->curr = end;
        return true;
    } else {
        ini->curr = curr + 1;
        return false;
    }
}

bool ini::skip_comments(INI *ini)
{
    const char *curr = ini->curr;
    const char *end = ini->end;

    while (curr != end) {
        if (*curr == '\r' || *curr == '\n')
            curr++;
        else if (*curr == '#')
            do { curr++; } while (curr != end && *curr != '\n');
        else
        break;
    }

    ini->curr = curr;
    return curr == end;
}

INI *ini::_ini_open_mem(const char *buf, size_t len, bool free_buf_on_exit)
{
    INI *ini =(INI*) malloc(sizeof(*ini));
        if (!ini) {
            perror("Unable to allocate memory");
            return NULL;
        }

        ini->buf = ini->curr = buf;
        ini->end = buf + len;
        ini->free_buf_on_exit = free_buf_on_exit;
        return ini;
}

INI *ini::ini_open(const char *file)
{
    FILE *f;
        char *buf, *ptr;
        size_t len, left;
        INI *ini = NULL;

        f = fopen(file, "r");
        if (!f) {
            perror("Unable to open file");
            return NULL;
        }

        fseek(f, 0, SEEK_END);
        len = ftell(f);

        if (!len) {
            fprintf(stderr, "ERROR: File is empty\n");
            goto error_fclose;
        }

        buf =(char*) malloc(len);
        if (!buf) {
            perror("Unable to allocate memory");
            goto error_fclose;
        }

        rewind(f);

        for (left = len, ptr = buf; left; ) {
            size_t tmp = fread(ptr, 1, left, f);
            if (tmp == 0) {
                if (feof(f))
                    break;

                free(buf);
                perror("Unable to read file");
                goto error_fclose;
            }

            left -= tmp;
            ptr += tmp;
        }

        ini = _ini_open_mem(buf, len - left, true);

    error_fclose:
        fclose(f);
        return ini;
}

INI *ini::ini_open_mem(const char *buf, size_t len)
{
    return _ini_open_mem(buf, len, false);
}

void ini::ini_close(INI *ini)
{
    if (ini->free_buf_on_exit)
        free((char *) ini->buf);
    free(ini);
}

int ini::ini_next_section(INI *ini, const char **name, size_t *name_len)
{
    const char *_name;
    if (ini->curr == ini->end)
        return 0; /* EOF: no more sections */

    if (ini->curr == ini->buf) {
        if (skip_comments(ini) || *ini->curr != '[') {
            fprintf(stderr, "Malformed INI file (missing section header)\n");
            return -EIO;
        }
    }
    else
        while (*ini->curr != '[' && !skip_line(ini));

    if (ini->curr == ini->end)
        return 0; /* EOF: no more sections */

    _name = ++ini->curr;
    do {
        ini->curr++;
        if (ini->curr == ini->end || *ini->curr == '\n') {
            fprintf(stderr, "Malformed INI file (malformed section header)\n");
            return -EIO;
        }
    } while (*ini->curr != ']');


    if (name && name_len) {
        *name = _name;
        *name_len = ini->curr - _name;
    }

    ini->curr++;
    return 1;
}

int ini::ini_read_pair(INI *ini, const char **key, size_t *key_len, const char **value, size_t *value_len)
{
    size_t _key_len = 0;
    const char *_key, *_value, *curr, *end = ini->end;

    if (skip_comments(ini))
        return 0;
    curr = _key = ini->curr;

    if (*curr == '[')
        return 0;

    while (true) {
        curr++;

        if (curr == end || *curr == '\n') {
            fprintf(stderr, "ERROR: Unexpected end of line\n");
            return -EIO;

        } else if (*curr == '=') {
            const char *tmp = curr;
            _key_len = curr - _key;
            for (tmp = curr - 1; tmp > ini->curr &&
                 (*tmp == ' ' || *tmp == '\t'); tmp--)
                _key_len--;
            curr++;
            break;
        }
    }

    /* Skip whitespaces. */
    while (curr != end && (*curr == ' ' || *curr == '\t')) curr++;
    if (curr == end) {
        fprintf(stderr, "ERROR: Unexpected end of line\n");
        return -EIO;
    }

    _value = curr;

    while (curr != end && *curr != '\n') curr++;
    if (curr == end) {
        fprintf(stderr, "ERROR: Unexpected end of line\n");
        return -EIO;
    }

    *value = _value;
    *value_len = curr - _value - (*(curr - 1) == '\r');
    *key = _key;
    *key_len = _key_len;

    ini->curr = ++curr;
    return 1;
}

void ini::ini_set_read_pointer(INI *ini, const char *pointer)
{
    if ((uintptr_t) pointer < (uintptr_t) ini->buf)
        ini->curr = ini->buf;
    else if ((uintptr_t) pointer > (uintptr_t) ini->end)
        ini->curr = ini->end;
    else
        ini->curr = pointer;
}

int ini::ini_get_line_number(INI *ini,const char *pointer)
{
    int line = 1;
    const char *it;

    if ((uintptr_t) pointer < (uintptr_t) ini->buf)
        return -EINVAL;
    if ((uintptr_t) pointer > (uintptr_t) ini->end)
        return -EINVAL;

    for (it = ini->buf; (uintptr_t) it < (uintptr_t) pointer; it++)
        line += (*it == '\n');

    return line;
}
