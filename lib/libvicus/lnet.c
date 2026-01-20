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
#define __LIB_LIBVICUS_LNET_C 1
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
#ifdef VICUS_WITH_WINSOCK2
#   define WIN32_LEAN_AND_MEAN
#   include <winsock2.h>
#   include <ws2tcpip.h>
#else
#   include <arpa/inet.h>
#   include <sys/socket.h>
#   include <sys/un.h>
#   include <netdb.h>
#endif


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

static int
vicus_getaddrinfo_copy(
         vicus_addrinfo_t **           dstp,
         struct addrinfo  *            src );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

void
vicus_freeaddrinfo(
         vicus_addrinfo_t *            res )
{
   vicus_addrinfo_t *   res_rec;
   vicus_addrinfo_t *   res_next;

   VicusTrace();

   if (!(res))
      return;

   for(res_rec = res; ((res_rec)); res_rec = res_next)
   {  res_next = res_rec->ai_next;
      if ((res_rec->ai_addr))
         free(res_rec->ai_addr);
      free(res_rec);
   };

   return;
}


int
vicus_getaddrinfo(
         const char *                  hostname,
         const char *                  servname,
         vicus_addrinfo_t **           resp )
{
   int                  rc;
   struct addrinfo      hints;
   struct addrinfo *    ai;
   struct addrinfo *    ai_rec;
   vicus_addrinfo_t *   res;
   vicus_addrinfo_t *   res_rec;

   VicusTrace();
   assert(hostname != NULL);
   assert(resp != NULL);

   // resolve hostname/IP address
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family   = AF_UNSPEC;
   hints.ai_protocol = IPPROTO_TCP;
   hints.ai_socktype = SOCK_STREAM;
   if ((rc = getaddrinfo(hostname, servname, &hints, &ai)) != 0)
   {  switch(rc)
      {  case EAI_AGAIN:   return(VICUS_EDNSRES);
         case EAI_FAIL:    return(VICUS_EDNSRES);
         case EAI_MEMORY:  return(VICUS_ENOMEM);
         case EAI_NODATA:  return(VICUS_EDNSRES);
         case EAI_SERVICE: return(VICUS_EDNSRES);
         default:          break;
      };
      return(VICUS_EURI);
   };

   if ((rc = vicus_getaddrinfo_copy(&res, ai)) != VICUS_SUCCESS)
   {  freeaddrinfo(ai);
      return(rc);
   };
   res_rec = res;

   // count results
   for(ai_rec = ai->ai_next; ((ai_rec)); ai_rec = ai_rec->ai_next)
   {  if ((rc = vicus_getaddrinfo_copy(&res_rec->ai_next, ai_rec)) != VICUS_SUCCESS)
      {  freeaddrinfo(ai);
         vicus_freeaddrinfo(res);
         return(rc);
      };
      res_rec = res_rec->ai_next;
   };

   *resp = res;

   return(0);
}


int
vicus_getaddrinfo_copy(
         vicus_addrinfo_t **           dstp,
         struct addrinfo  *            src )
{
   unsigned                addrlen;
   vicus_addrinfo_t *      dst;

   VicusTrace();
   assert(dstp != NULL);
   assert(src  != NULL);

   // check results
   if (src->ai_family != src->ai_addr->sa_family)
      return(VICUS_EUNKNOWN);
   switch(src->ai_family)
   {  case PF_INET:  addrlen = sizeof(struct sockaddr_in);  break;
      case PF_INET6: addrlen = sizeof(struct sockaddr_in6); break;
      default:       return(VICUS_ENOTSUP);
   };
   if (addrlen != src->ai_addrlen)
      return(VICUS_EUNKNOWN);

   // allocate memory
   if ((dst = malloc(sizeof(vicus_addrinfo_t))) == NULL)
      return(VICUS_ENOMEM);
   memset(dst, 0, sizeof(vicus_addrinfo_t));
   dst->ai_addrlen   = src->ai_addrlen;
   dst->ai_family    = src->ai_family;
   dst->ai_protocol  = src->ai_protocol;
   dst->ai_socktype  = src->ai_socktype;

   // copy ai_addr
   if ((src->ai_addr))
   {  if ((dst->ai_addr = malloc(sizeof(struct sockaddr_storage))) == NULL)
      {  vicus_freeaddrinfo(dst);
         return(VICUS_ENOMEM);
      };
      memset(dst->ai_addr, 0, sizeof(struct sockaddr_storage));
      memcpy(dst->ai_addr, src->ai_addr, src->ai_addrlen);
   };

   *dstp = dst;

   return(0);
}


int
vicus_ntop(
         vicus_addrinfo_t *            ai,
         char *                        dst,
         size_t                        len )
{
   void *               addr;

   assert(ai   != NULL);
   assert(dst  != NULL);

   switch(ai->ai_family)
   {  case PF_INET:
         addr = &((struct sockaddr_in *)ai->ai_addr)->sin_addr;
         if ((inet_ntop(ai->ai_family, addr, dst, (socklen_t)len)))
            return(VICUS_EUNKNOWN);
         break;

      case PF_INET6:
         addr = &((struct sockaddr_in6 *)ai->ai_addr)->sin6_addr;
         if ((inet_ntop(ai->ai_family, addr, dst, (socklen_t)len)))
            return(VICUS_EUNKNOWN);
         break;

#ifndef VICUS_WITH_WINSOCK2
      case PF_UNIX:
         addr = &((struct sockaddr_un *)ai->ai_addr)->sun_path;
         if ((len+1) < strlen((const char *)addr))
            return(VICUS_EUNKNOWN);
         vicus_strlcpy(dst, (const char *)addr, len);
         break;
#endif

      default:
         return(VICUS_ENOTSUP);
   };

   return(0);
}


/* end of source */
