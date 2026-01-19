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
#define __LIB_LIBVICUS_LNET_BSDSOCKET_C 1
#include "libvicus.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>


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
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
vicus_getunixinfo(
         const char *                  path,
         vicus_addrinfo_t **           resp )
{
   size_t                     path_maxlen;
   size_t                     size;
   vicus_addrinfo_t *         dst;
   struct sockaddr_un *       sa;

   assert(path != NULL);
   assert(resp != NULL);

   path_maxlen  = sizeof(struct sockaddr_un);
   path_maxlen -= offsetof(struct sockaddr_un, sun_path);
   if (strlen(path) >= (path_maxlen-1))
      return(VICUS_EURI);

   if ((dst = malloc(sizeof(vicus_addrinfo_t))) == NULL)
      return(VICUS_ENOMEM);
   memset(dst, 0, sizeof(vicus_addrinfo_t));
   dst->ai_addrlen   = sizeof(struct sockaddr_un);
   dst->ai_family    = PF_LOCAL;
   dst->ai_protocol  = 0;
   dst->ai_socktype  = SOCK_STREAM;

   // copy ai_addr
   size = sizeof(struct sockaddr_storage);
   if ((sa = malloc(size)) == NULL)
   {  vicus_freeaddrinfo(dst);
      return(VICUS_ENOMEM);
   };
   memset(sa, 0, sizeof(struct sockaddr_storage));
   vicus_strlcpy(sa->sun_path, path, path_maxlen);
   sa->sun_family = PF_LOCAL;
   sa->sun_len    = sizeof(struct sockaddr_un);
   dst->ai_addr   = sa;

   *resp = dst;

   return(0);
}


int
vicus_net_initialize(
         vicus_t *                     vd )
{
   assert(vd != NULL);
   return(0);
}


int
vicus_net_terminate(
         vicus_t *                     vd )
{
   assert(vd != NULL);
   return(0);
}

/* end of source */
