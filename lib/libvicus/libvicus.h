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
/*
 *  lib/libnetcalc/libnetcalc.h - common includes and prototypes
 */
#ifndef __LIB_LIBVICUS_H
#define __LIB_LIBVICUS_H 1

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

// defined in the Single UNIX Specification
#ifndef _XOPEN_SOURCE
#   define _XOPEN_SOURCE 600
#endif

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <sys/types.h>

#include <vicus.h>
#include <vicus_noinst.h>


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

#undef   VICUS_DOMAIN_LENGTH
#define  VICUS_DOMAIN_LENGTH  255         // RFC 1035

#undef   VICUS_URI_LENGTH
#define  VICUS_URI_LENGTH     268         // strlen("tcp://:65535") + VICUS_DOMAIN_LENGTH + 1


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types

struct _libvicus
{  int                        s;
   int                        s_timeout;
   int                        req_timeout;
   int                        __int_padding;
   vicus_addrinfo_t *         s_ai;
   vicus_urldesc_t *          s_vudp;
   vicus_urldesc_t *          vudp;       // vicus URL description pointer
};


struct _libvicus_addrinfo
{  int                        ai_family;
   int                        ai_socktype;
   int                        ai_protocol;
   unsigned                   ai_addrlen;
   void *                     ai_addr;
   vicus_addrinfo_t *         ai_next;
};


struct _libvicus_urldesc
{  char *                     vud_uri;
   vicus_addrinfo_t *         vud_addrinfo;
   vicus_urldesc_t *          vud_next;
};


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

extern int     vicus_opt_debug;
extern int     vicus_opt_debug_stderr;
extern int     vicus_opt_debug_source;
extern int     vicus_opt_trace;


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

//--------------------//
// network prototypes //
//--------------------//
// MARK: network prototypes

extern int
vicus_close(
         vicus_t *                     vd );


extern void
vicus_freeaddrinfo(
         vicus_addrinfo_t *            res );


extern int
vicus_getaddrinfo(
         const char *                  hostname,
         const char *                  servname,
         vicus_addrinfo_t **           resp );


extern int
vicus_getunixinfo(
         const char *                  path,
         vicus_addrinfo_t **           resp );


extern int
vicus_net_initialize(
         vicus_t *                     vd );


extern int
vicus_net_terminate(
         vicus_t *                     vd );

extern int
vicus_ntop(
         vicus_addrinfo_t *            ai,
         char *                        dst,
         size_t                        len );


#endif /* end of header */

