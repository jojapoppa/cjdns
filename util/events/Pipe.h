/* vim: set expandtab ts=4 sw=4: */
/*
 * You may redistribute this program and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef Pipe_H
#define Pipe_H

#include "memory/Allocator.h"
#include "exception/Except.h"
#include "interface/Iface.h"
#include "util/events/EventBase.h"
#include "util/log/Log.h"
#include "util/Linker.h"
Linker_require("util/events/libuv/Pipe.c");

#include <stdbool.h>

struct Pipe;
typedef void (* Pipe_callback)(struct Pipe* p, int status);

struct Pipe
{
    struct Iface iface;

    /** The name of the file eg: "/tmp/cjdns_pipe_foo" */
    const char* const fullName;

    /** Pointer to platform specific file desc or handle */
    void* fd;

    void* userData;

    struct EventBase* const base;

    Pipe_callback onConnection;
    Pipe_callback onClose;

    /** the name as provided by the user eg: "foo" */
    const char* const name;
    struct Log* logger;
};

#define Pipe_PADDING_AMOUNT 512
#define Pipe_BUFFER_CAP 4000

#ifdef win32
    #define Pipe_PATH_SEP "\\"
#else
    #define Pipe_PATH_SEP "/"
#endif

#ifndef Pipe_PATH
    #ifdef win32
        #define Pipe_PATH "\\\\.\\pipe"
    #elif defined(android)
        #define Pipe_PATH "/data/local/tmp"
    #else
        #define Pipe_PATH "/tmp"
    #endif
#endif


struct Pipe* Pipe_named(const char* path,
                        const char* name,
                        struct EventBase* eb,
                        struct Except* eh,
                        struct Allocator* userAlloc);

struct Pipe* Pipe_namedConnect(const char* fullPath,
                               bool attemptToCreate,
                               struct EventBase* eb,
                               struct Except* eh,
                               struct Allocator* userAlloc);

struct Pipe* Pipe_forFiles(int inFd,
                           int outFd,
                           struct EventBase* eb,
                           struct Except* eh,
                           struct Log* logger,
                           struct Allocator* userAlloc);

Er_DEFUN(struct Pipe* Pipe_forFd(int fd,
                        bool ipc,
                        struct EventBase* eb,
                        struct Log* log,
                        struct Allocator* userAlloc));

Er_DEFUN(bool Pipe_exists(const char* path, struct Allocator* errAlloc));

#endif
