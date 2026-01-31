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
#define __LIB_LIBVICUS_LNET_WINSOCK2_C 1
#include "libvicus.h"
#ifdef VICUS_WITH_WINSOCK2


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

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>


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

struct _libvicus_socket
{  SOCKET                     s;
};


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

static int
vicus_net_connect_tcp(
         vicus_t *                     vd,
         vicus_addrinfo_t *            ai,
         SOCKET *                      sp );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
vicus_close(
         vicus_t *                     vd )
{
   VicusTrace();
   assert(vd != NULL);

   if ((vd->sock))
   {  if (vd->sock->s != INVALID_SOCKET)
         closesocket(vd->sock->s);
      free(vd->sock);
   };
   vd->sock       = NULL;
   vd->sock_ai    = NULL;
   vd->sock_vudp  = NULL;

   return(0);
}


int
vicus_net_connect(
         vicus_t *                     vd )
{
   int                     rc;
   vicus_urldesc_t *       vudp;
   vicus_addrinfo_t *      ai;
   SOCKET                  s;

   VicusTrace();
   assert(vd != NULL);

   if (!(vd->vudp))
      return(VICUS_ENOTSUP);

   for(vudp = vd->vudp; ((vudp)); vudp = vudp->vud_next)
   {  VicusDebug("   using %s ...\n", vudp->vud_uri);
      for(ai = vudp->vud_addrinfo; ((ai)); ai = ai->ai_next)
      {  switch(ai->ai_family)
         {  case PF_INET:     rc = vicus_connect_tcp(vd, ai, &s);  break;
            case PF_INET6:    rc = vicus_connect_tcp(vd, ai, &s);  break;
            default:          return(VICUS_ENOTSUP);
         };
         if (rc == VICUS_SUCCESS)
         {  if ((vd->sock = malloc(sizeof(vicus_socket_t))) == NULL)
            {  closesocket(s);
               return(VICUS_ENOMEM);
            };
            vd->sock->s    = s;
            vd->sock_ai    = ai;
            vd->sock_vudp  = vudp;
            return(0);
         };
      };
   };

   return(VICUS_ECONNECT);
}


int
vicus_net_connect_tcp(
         vicus_t *                     vd,
         vicus_addrinfo_t *            ai,
         SOCKET *                      sp )
{
   SOCKET                        s;
   int                           opt;
   int                           rc;
   struct sockaddr_storage *     sa;
   char                          addrstr[INET_ADDRSTRLEN+INET6_ADDRSTRLEN];
   u_long                        mode;

   VicusTrace();
   assert(vd != NULL);
   assert(ai != NULL);

   vicus_ntop(ai, addrstr, sizeof(addrstr));
   VicusDebug("   connecting to %s ...\n", addrstr);

   sa       = ai->ai_addr;

   // create socket
   if ((s = socket(sa->ss_family, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
      return(-1);

   // set socket options
   opt = 1; setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int));
   opt = 1; setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(int));

   // connect to server
   if ((rc = connect(s, (struct sockaddr *)sa, ai->ai_addrlen)) == -1)
   {  closesocket(s);
      return(-1);
   };

   // set non-blocking
   mode = 1; ioctlsocket(s, FIONBIO, &mode);

   *sp = s;

   return(0);
}


int
vicus_getunixinfo(
         const char *                  path,
         vicus_addrinfo_t **           resp )
{
   VicusTrace();
   assert(path != NULL);
   assert(resp != NULL);

   if (!(path))
      return(VICUS_EINVAL);
   if (!(resp))
      return(VICUS_EINVAL);

   return(VICUS_ENOTSUP);
}


int
vicus_net_get_fd(
         vicus_t *                     vd,
         int *                         fdp )
{
   VicusTrace();
   assert(vd  != NULL);
   assert(fdp != NULL);
   return(VICUS_ENOTSUP);
}


int
vicus_net_initialize(
         vicus_t *                     vd )
{
   int         rc;
   WSADATA     wsaData;

   VicusTrace();
   assert(vd != NULL);

   if ((rc = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
      return(VICUS_EUNKNOWN);

   return(0);
}


int
vicus_net_set_fd(
         vicus_t *                     vd,
         int                           fd )
{
   VicusTrace();
   assert(vd != NULL);
   assert(fd != -1);
   return(VICUS_ENOTSUP);
}



int
vicus_net_terminate(
         vicus_t *                     vd )
{
   VicusTrace();
   assert(vd != NULL);
   WSACleanup();
   return(0);
}


ssize_t
vicus_net_recv(
         vicus_t *                     vd,
         void *                        buff,
         size_t                        len )
{
   int               size;
   FD_SET            readfds;
   int               rc;

   VicusTrace();
   assert(vd != NULL);

   if (vd->sock == NULL)
      return(VICUS_ECONNECT);

   FD_ZERO(&readfds);
   FD_SET(vd->sock->s, &readfds);
   if ((rc = select(0, &readfds, NULL, NULL, NULL)) == SOCKET_ERROR)
   {  vicus_close(vd);
      return(VICUS_ESERVER);
   };
   if (!(FD_ISSET(vd->sock->s, &readfds)))
   {  vicus_close(vd);
      return(VICUS_EUNKNOWN);
   };
   if ((size = recv(vd->sock->s, buff, len, 0)) == -1)
   {  vicus_close(vd);
      return(VICUS_EUNKNOWN);
   };

   return((ssize_t)size);
}


ssize_t
vicus_net_send(
         vicus_t *                     vd,
         const void *                  buff,
         size_t                        len )
{
   int               size;
   FD_SET            writefds;
   int               rc;

   VicusTrace();
   assert(vd != NULL);

   if (vd->sock == NULL)
      return(VICUS_ECONNECT);

   FD_ZERO(&writefds);
   FD_SET(vd->sock->s, &writefds);
   if ((rc = select(0, NULL, &writefds, NULL, NULL)) == SOCKET_ERROR)
   {  vicus_close(vd);
      return(VICUS_ESERVER);
   };
   if (!(FD_ISSET(vd->sock->s, &writefds)))
   {  vicus_close(vd);
      return(VICUS_EUNKNOWN);
   };
   if ((size = send(vd->sock->s, buff, len, 0)) == -1)
   {  vicus_close(vd);
      return(VICUS_EUNKNOWN);
   };

   return((ssize_t)size);
}


#endif /* end of source */
