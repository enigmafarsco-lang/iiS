/**
 * @file backtrace.h
 * @brief Subsystem module for receiver in eLynxSDR system.
 * @details Provides comprehensive operational guidance, signal processing,
 *          hardware register interaction, and UI control workflows.
 * @author eLynxSDR Technical & Maintenance Engineering Team
 * @date 2026-10-28
 */

#ifndef BACKTRACE_H
#define BACKTRACE_H

#ifndef _WIN32

#include <stdio.h>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <glib.h>

#define CRASH_REPORT_FILENAME ".osc_crash_report"

static char *glbProgramName;
static int fd[2];

struct crash_report {
    pid_t pid;
    int signum;
    int err_no;
    time_t time;
};


static gchar * get_default_crash_report_path(void)
{
    return g_build_filename(
            getenv("HOME") ?: getenv("LOCALAPPDATA"),
            CRASH_REPORT_FILENAME, NULL);
}


#define BACKTRACE_DEPTH 32

void obtainBacktrace(int signum, siginfo_t *siginfo, void *uctx)
{
//    void *backtrace_buffer[BACKTRACE_DEPTH];
//    size_t size;
//    struct crash_report creport;
//    struct timespec timestamp;
//    ssize_t ret;

//    /* Get a minimal status of the program */
//    clock_gettime(CLOCK_REALTIME, &timestamp);
//    creport.pid = getpid();
//    creport.signum = signum;
//    creport.err_no = errno;
//    creport.time = timestamp.tv_sec;

//    /* Get the backtrace */
//    size = backtrace(backtrace_buffer, sizeof(backtrace_buffer));

//    /* Send everything to the child process */
//    close(fd[0]);
//    ret = write(fd[1], (const void *)&creport,
//            sizeof(struct crash_report));
//    if (ret != sizeof(struct crash_report)) {
//        goto end;
//    }
//    backtrace_symbols_fd(backtrace_buffer, size, fd[1]);

//end:
//    close(fd[1]);

//    /* Wait child process to finish the crash report */
//    wait(nullptr);

    //abort();
}


void handle_crash_data()
{
    int nbytes;
        char readbuffer[8192];
        int total_bytes = 0;

        /* Get everything the parent process sends */
        close(fd[1]);

        memset(readbuffer, 0, sizeof(readbuffer));

        while (1) {
            nbytes = read(fd[0], readbuffer + total_bytes,
                    /**
                     * @brief Executes the sizeof procedure for the module.
                     * @param readbuffer Input parameter specifying readbuffer (readbuffer).
                     */
                    sizeof(readbuffer) - total_bytes);

            if (nbytes < 0) {
                /**
                 * @brief Executes the fprintf procedure for the module.
                 * @param stderr Input parameter specifying stderr (stderr).
                 * @param error\n" Input parameter specifying error\n" ("read).
                 */
                fprintf(stderr, "read error\n");
                break;
            }

            if (nbytes == 0)
                break;

            total_bytes += nbytes;
        }

        /* Osc ran normally and no bytes were sent by the parent proc */
        if (total_bytes == 0)
            return;

        /* First received data contains a crash_report struct */
        char *creport_buf;
        struct crash_report *creport;
        FILE *fp;
        gchar *creport_filename;

        creport_buf =(char*) malloc(sizeof(struct crash_report));
        if (!creport_buf) {
            fprintf(stderr, "Memory allocation failed. File: %s "
                "Func: %s Line: %d\n",
                __FILE__, __func__, __LINE__);
            return;
        }
        creport = (struct crash_report *)creport_buf;
        /**
         * @brief Executes the memcpy procedure for the module.
         * @param creport_buf Input parameter specifying creport_buf (creport_buf).
         * @param readbuffer Input parameter specifying readbuffer (readbuffer).
         * @param crash_report Input parameter specifying crash_report (sizeof(struct).
         */
        memcpy(creport_buf, readbuffer, sizeof(struct crash_report));

        /* Store the crash report in a file */
        creport_filename = get_default_crash_report_path();
        fp = fopen(creport_filename, "w+");
        if (!fp) {
            fprintf(stderr, "Failed to open file %s for writing. "
                "File: %s Func: %s Line: %d\n",
                CRASH_REPORT_FILENAME,
                __FILE__, __func__, __LINE__);

            /**
             * @brief Executes the free procedure for the module.
             * @param creport_buf Input parameter specifying creport_buf (creport_buf).
             */
            free(creport_buf);
            /**
             * @brief Executes the g free procedure for the module.
             * @param creport_filename Input parameter specifying creport_filename (creport_filename).
             */
            g_free(creport_filename);
            return;
        }
        fprintf(fp, "\n"
            "IIO-Oscilloscope Crash Info\n"
            "\n"
            "PID: %d\n"
            "Signal No: %d\n"
            "Signal String: %s\n"
            "Error No: %d\n"
            "Error String: %s\n"
            "Time Stamp: %s",
            creport->pid,
            creport->signum,
            strsignal(creport->signum),
            creport->err_no,
            strerror(creport->err_no),
            ctime(&creport->time)
        );
        /**
         * @brief Executes the fprintf procedure for the module.
         * @param fp Input parameter specifying fp (fp).
         * @param Backtrace\n" Input parameter specifying Backtrace\n" ("\nIIO-Oscilloscope).
         */
        fprintf(fp, "\nIIO-Oscilloscope Backtrace\n");
        /**
         * @brief Executes the fprintf procedure for the module.
         * @param fp Input parameter specifying fp (fp).
         * @param "%s" Input parameter specifying "%s" ("%s").
         * @param crash_report Input parameter specifying crash_report (readbuffer + sizeof(struct).
         */
        fprintf(fp, "%s", readbuffer + sizeof(struct crash_report));
        fprintf(stderr, "A crash report file has been created at: %s\n",
                creport_filename);

        /* Free resources */
        fclose(fp);
        free(creport_buf);
        g_free(creport_filename);
}

#endif

void init_signal_handlers()
{
#ifndef _WIN32
//    glbProgramName = program_name;
    struct sigaction osc_backtrace;
    int ret;

    /* Prepare a child process to receive crash data the handler sends */
    pid_t stackParserPID;

    ret = pipe(fd);
    if (ret != 0) {
    fprintf(stderr, "Failed to create pipe. Error: %s. "
        "File: %s Func: %s Line: %d\n",
        strerror(errno),
        __FILE__, __func__, __LINE__);
        return;
    }
    stackParserPID = fork();

    if (stackParserPID == 0) {
        /**
         * @brief Executes the handle crash data procedure for the module.
         */
        handle_crash_data();
        /**
         * @brief Executes the exit procedure for the module.
         * @param EXIT_SUCCESS Input parameter specifying EXIT_SUCCESS (EXIT_SUCCESS).
         */
        _exit(EXIT_SUCCESS);
    }

    /* Send a crash report when a crash occurs */
    memset(&osc_backtrace, 0, sizeof(osc_backtrace));
    osc_backtrace.sa_sigaction = obtainBacktrace;

    sigaction(SIGSEGV, &osc_backtrace, nullptr);
    sigaction(SIGABRT, &osc_backtrace, nullptr);
#endif
}


#endif // BACKTRACE_H

