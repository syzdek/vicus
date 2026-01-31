/*
 *  Versatile IKE Control User Software for strongSwan
 *  Copyright (C) 2026 David M. Syzdek <david@syzdek.net>.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *
 *     3. Neither the name of the copyright holder nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define __LIB_LIBVICUS_LDEBUG_C 1
#include "libvicus.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdarg.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
// MARK: - Macros


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

int            vicus_opt_debug               = VICUS_DBG_NONE;
int            vicus_opt_debug_stderr        = VICUS_FALSE;
const char *   vicus_opt_debug_prefix        = "libvicus";
char *         vicus_opt_debug_prefix_buff   = NULL;


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
vicus_debug(
         const char *                  file,
         int                           line,
         const char *                  fmt,
         ... )
{
   FILE *         fs;
   int            len;
   va_list        ap;

   if (!(vicus_opt_debug & VICUS_DBG_INFO))
      return(0);

   len   = 0;
   fs    = (vicus_opt_debug_stderr == VICUS_FALSE)
         ? stdout
         : stderr;

   if ((vicus_opt_debug_prefix))
      len += fprintf(fs, "%s: ", vicus_opt_debug_prefix);

   if ((vicus_opt_debug & VICUS_DBG_SRC))
      len += fprintf(fs, "%s: %i: ", file, line);

   va_start(ap, fmt);
      len += vfprintf(fs, fmt, ap);
   va_end(ap);

   return(len);
}


int
vicus_debug_trace(
         const char *                  file,
         int                           line,
         const char *                  func )
{
   int            len;
   FILE *         fs;

   if (!(vicus_opt_debug & VICUS_DBG_TRACE))
      return(0);

   len   = 0;
   fs    = (vicus_opt_debug_stderr == VICUS_FALSE)
         ? stdout
         : stderr;

   if ((vicus_opt_debug_prefix))
      len += fprintf(fs, "%s: ", vicus_opt_debug_prefix);

   if ((vicus_opt_debug & VICUS_DBG_SRC))
      len += fprintf(fs, "%s: %i: ", file, line);

   len += fprintf(fs, "%s()\n", func);

   return(len);
}


/* end of source */
