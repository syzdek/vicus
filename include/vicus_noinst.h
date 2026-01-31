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
 *  include/vicus_noinst.h - common includes and prototypes
 */
#ifndef __VICUS_NOINST_H
#define __VICUS_NOINST_H 1

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <vicus.h>
#include <sys/types.h>


//////////////
//          //
//  Macros  //
//          //
//////////////
// MARK: - Macros

#ifdef USE_DEBUG
#  define VicusDebug(...)  vicus_debug(__FILE__, __LINE__, __VA_ARGS__)
#  define VicusTrace()     vicus_debug_trace(__FILE__, __LINE__, __func__)
#else
#  define VicusDebug(...)  /* vicus_debug(__FILE__, __LINE__, __VA_ARGS__) */
#  define VicusTrace()     /* vicus_debug_trace(__FILE__, __LINE__, __func__) */
#endif


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types


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
vicus_debug(
         const char *                  file,
         int                           line,
         const char *                  fmt,
         ... );


_VICUS_F int
vicus_debug_trace(
         const char *                  file,
         int                           line,
         const char *                  func );


//--------------------------//
// miscellaneous prototypes //
//--------------------------//
#pragma mark miscellaneous prototypes

_VICUS_F int
vicus_base64_encode(
         char *                        dst,
         size_t                        s,
         const uint8_t *               src,
         size_t                        n );


_VICUS_F int
vicus_hexdump(
         const void *                  dat,
         size_t                        datlen );


_VICUS_F int
vicus_hexdump_prefix(
         const char *                  prefix,
         const void *                  dat,
         size_t                        datlen );


_VICUS_F uint16_t
vicus_hton16(
         uint16_t                      u );


_VICUS_F uint32_t
vicus_hton32(
         uint32_t                      u );


_VICUS_F uint64_t
vicus_hton64(
         uint64_t                      u );


_VICUS_F uint16_t
vicus_ntoh16(
         uint16_t                      u );


_VICUS_F uint32_t
vicus_ntoh32(
         uint32_t                      u );


_VICUS_F uint64_t
vicus_ntoh64(
         uint64_t                      u );


_VICUS_F size_t
vicus_strlcat(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


_VICUS_F size_t
vicus_strlcpy(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize );


//--------------------//
// network prototypes //
//--------------------//
#pragma mark network prototypes

_VICUS_F ssize_t
vicus_recv(
         vicus_t *                     vd,
         void *                        buff,
         size_t                        len );


_VICUS_F ssize_t
vicus_send(
         vicus_t *                     vd,
         const void *                  buff,
         size_t                        len );


//--------------------//
// message prototypes //
//--------------------//
// MARK: message prototypes

_VICUS_F int
vicus_msg_pkt(
         vicus_msg_t *                 msg,
         const vicus_pkt_t **          pktp,
         size_t *                      pktlenp );


//-------------------//
// packet prototypes //
//-------------------//
// MARK: packet prototypes

_VICUS_F int
vicus_pkt_validate(
         const vicus_pkt_t *           pkt );


#endif /* end of header */
