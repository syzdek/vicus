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
 *  include/vicus.h - common includes and prototypes
 */
#ifndef __VICUS_H
#define __VICUS_H 1

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <inttypes.h>
#include <stddef.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
// MARK: - Macros

#undef _VICUS_I
#undef _VICUS_F
#undef _VICUS_V
#ifdef _LIB_LIBVICUS_H
#  define _VICUS_I      inline
#  define _VICUS_F      /* empty */
#  define _VICUS_V      extern
#else
#  define _VICUS_I      extern
#  define _VICUS_F      extern
#  define _VICUS_V      extern
#endif

#undef  VICUS_SET
#undef  VICUS_UNSET
#define VICUS_SET(flgs, flg)        ( flgs | flg )
#define VICUS_UNSET(flgs, flg)      ( flgs & ~flg )


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions

#define VICUS_SUCCESS                  0
#define VICUS_EUNKNOWN                 -1
#define VICUS_ENOMEM                   -2
#define VICUS_ECONNECT                 -3
//#define UNUSED                       -4
#define VICUS_ENOTSUP                  -5
#define VICUS_EINVAL                   -6
#define VICUS_EURI                     -7
#define VICUS_EDNSRES                  -8
#define VICUS_ESIZE                    -9
#define VICUS_ESERVER                  -10

#define VICUS_OPT_DEBUG                1
#define VICUS_OPT_TRACE                2
#define VICUS_OPT_DEBUG_STDERR         3
#define VICUS_OPT_SOCKET               4

#define VICUS_TRUE                     1
#define VICUS_FALSE                    0

#define VICUS_DFLT_URI                 "/var/run/charon.vici"


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types

typedef struct _libvicus               vicus_t;
typedef struct _libvicus_urldesc       vicus_urldesc_t;
typedef struct _libvicus_addrinfo      vicus_addrinfo_t;


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

//-----------------//
// core prototypes //
//-----------------//
// MARK: core prototypes

_VICUS_F int
vicus_connect(
         vicus_t *                     vd );


_VICUS_F int
vicus_disconnect(
         vicus_t *                     vd );


_VICUS_F int
vicus_get_option(
         vicus_t *                     vd,
         int                           option,
         void *                        outvalue );


_VICUS_F int
vicus_init_fd(
         vicus_t **                    vdp,
         int                           fd );


_VICUS_F int
vicus_initialize(
         vicus_t **                    vdp,
         const char *                  uri );


_VICUS_F int
vicus_set_option(
         vicus_t *                     vd,
         int                           option,
         const void *                  invalue );


//------------------//
// error prototypes //
//------------------//
// MARK: error prototypes

_VICUS_F const char *
vicus_strerror(
         int                           err );


//-----------------//
// URL prototypes //
//-----------------//
// MARK: URL prototypes

_VICUS_F void
ldap_free_urldesc(
         vicus_urldesc_t *             vudp );


_VICUS_F int
vicus_url_debug(
         vicus_t *                     vd,
         vicus_urldesc_t *             vudp );


_VICUS_F int
vicus_url_parse(
         const char *                  url,
         vicus_urldesc_t **            vudppp );


#endif /* end of header */
