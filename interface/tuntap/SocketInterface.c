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
#include "interface/tuntap/SocketInterface.h"
#include "exception/Er.h"
#include "exception/Jmp.h"
#include "memory/Allocator.h"
#include "util/events/EventBase.h"
#include "util/events/Pipe.h"
#include "crypto/random/Random.h"
#include "util/CString.h"
#include "benc/String.h"

Er_DEFUN(struct Iface* SocketInterface_new(const char* socketFullPath,
                                    struct EventBase* base,
                                    struct Log* logger,
                                    struct Allocator* alloc))
{
    Log_info(logger, "Initializing socket: %s\n", socketFullPath);

    struct Random* rand = Random_new(alloc, logger, NULL);
    char randName[32] = {0};
    Random_base32(rand, (uint8_t*)randName, 31);
    String* name = String_printf(alloc, "cjdns-sock-%s", randName);

    struct Pipe* p;
    struct Jmp jmp;
    Jmp_try(jmp) {
        p = Pipe_named(socketFullPath, name->bytes, base, &jmp.handler, alloc);
    } Jmp_catch {
        String* error = String_printf(alloc, "Failed to configure socket [%s]", jmp.message);
        Log_info(logger, "issue in Pipe_named: %s\n", error->bytes);
    }

    Er_ret(&p->iface);
}
