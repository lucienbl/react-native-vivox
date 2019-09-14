/* Copyright (c) 2016-2018 by Mercer Road Corp
 *
 * Permission to use, copy, modify or distribute this software in binary or source form
 * for any purpose is allowed only under explicit prior consent in writing from Mercer Road Corp
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND MERCER ROAD CORP DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL MERCER ROAD CORP
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <memory.h>

# include "Escape.h"

using namespace VivoxClientApi;

// static
bool MVCEscape::isUrlValidChar(unsigned char in)
{
    switch (in) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':

        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':

        case '-':
        case '.':
        case '_':
        case '~':
            return true;

        default:
            break;
    }

    return false;
}

// static
char *MVCEscape::escape(const char *str, unsigned int length)
{
    if (length == 0) {
        length = strlen(str);
    }

    // allocate for maximum conversion length; it is not cool, but fast
    size_t alloc = length * 4 + 1;

    // printf( "alloc = %lu\n", alloc );

    char *out = (char *) malloc(alloc);

    // printf( "out = 0x%08lx\n", (unsigned long) out );

    if (NULL == out) {
        return NULL;
    }

    const char *inptr = str;
    char *outptr = out;

    // printf( "Converting...\n" );
    for (unsigned int i = 0; i < length; i++, inptr++, outptr++) {
        if (isUrlValidChar((unsigned char) *inptr)) {
            // printf( "'%c' is valid\n", *inptr );
            *outptr = *inptr;
            continue;
        }

        snprintf(outptr, 4, "%%%02X", *inptr);

        // printf( "'%c' = %s\n", *inptr, outptr );

        outptr += 2; // +1 in for()
    }
    *outptr = '\0';

    // printf( "out = %s\n", out );

    return out;
}

// static
void MVCEscape::release(void *str)
{
    free(str);
}
