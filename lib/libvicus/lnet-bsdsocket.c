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
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <poll.h>


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
{  int                        s;
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
         int *                         sp );


static int
vicus_connect_unix(
         vicus_t *                     vd,
         vicus_addrinfo_t *            ai,
         int *                         sp );


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
   {  if (vd->sock->s != -1)
         close(vd->sock->s);
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
   int                     s;
   vicus_urldesc_t *       vudp;
   vicus_addrinfo_t *      ai;

   VicusTrace();
   assert(vd != NULL);

   if (!(vd->vudp))
      return(VICUS_ENOTSUP);
   if ((vd->sock))
      return(0);

   for(vudp = vd->vudp; ((vudp)); vudp = vudp->vud_next)
   {  VicusDebug("   using %s ...\n", vudp->vud_uri);
      for(ai = vudp->vud_addrinfo; ((ai)); ai = ai->ai_next)
      {  switch(ai->ai_family)
         {  case PF_INET:     rc = vicus_net_connect_tcp(vd,  ai, &s); break;
            case PF_INET6:    rc = vicus_net_connect_tcp(vd,  ai, &s); break;
            case PF_UNIX:     rc = vicus_connect_unix(vd, ai, &s); break;
            default:          return(VICUS_ENOTSUP);
         };
         if (rc == VICUS_SUCCESS)
         {  if ((vd->sock = malloc(sizeof(vicus_socket_t))) == NULL)
            {  close(s);
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
         int *                         sp )
{
   int                           s;
   int                           opt;
   int                           flgs;
   struct sockaddr_storage *     sa;
   char                          addrstr[INET_ADDRSTRLEN+INET6_ADDRSTRLEN];

   VicusTrace();
   assert(vd != NULL);
   assert(ai != NULL);

   vicus_ntop(ai, addrstr, sizeof(addrstr));
   VicusDebug("   connecting to %s ...\n", addrstr);

   sa       = ai->ai_addr;

   // create socket
   if ((s = socket(sa->ss_family, SOCK_STREAM, IPPROTO_TCP)) == -1)
      return(-1);

   // set socket options
#ifdef SO_NOSIGPIPE
   opt = 1; setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, (void *)&opt, sizeof(int));
#endif
#ifdef SO_REUSEADDR
   opt = 1; setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(int));
#endif
#ifdef SO_REUSEPORT
   opt = 1; setsockopt(s, SOL_SOCKET, SO_REUSEPORT, (void *)&opt, sizeof(int));
#endif
#ifdef SO_KEEPALIVE
   opt = 1; setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(int));
#endif

   // set file descriptor flags
#ifdef F_SETNOSIGPIPE
   fcntl(s, F_SETNOSIGPIPE, 1);
#endif

   // connect to server
   if (connect(s, (struct sockaddr *)sa, ai->ai_addrlen) == -1)
   {  close(s);
      return(-1);
   };

   // set non-blocking
   flgs  = fcntl(s, F_GETFL);
   flgs |= O_NONBLOCK;
   if (fcntl(s, F_SETFL, flgs) == -1)
   {  close(s);
      return(-1);
   };

   *sp = s;

   return(0);
}


int
vicus_connect_unix(
         vicus_t *                     vd,
         vicus_addrinfo_t *            ai,
         int *                         sp )
{
   int                           s;
   int                           opt;
   int                           flgs;
   struct sockaddr_storage *     sa;
   char                          addrstr[INET_ADDRSTRLEN+INET6_ADDRSTRLEN];

   VicusTrace();
   assert(vd != NULL);
   assert(ai != NULL);

   vicus_ntop(ai, addrstr, sizeof(addrstr));
   VicusDebug("   connecting to %s ...\n", addrstr);

   sa       = ai->ai_addr;

   // create socket
   if ((s = socket(sa->ss_family, SOCK_STREAM, 0)) == -1)
      return(-1);

   // set socket options
#ifdef SO_NOSIGPIPE
   opt = 1; setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, (void *)&opt, sizeof(int));
#endif
#ifdef SO_KEEPALIVE
   opt = 1; setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(int));
#endif

   // set file descriptor flags
#ifdef F_SETNOSIGPIPE
   fcntl(s, F_SETNOSIGPIPE, 1);
#endif

   // connect to server
   if (connect(s, (struct sockaddr *)sa, ai->ai_addrlen) == -1)
   {  close(s);
      return(-1);
   };

   // set non-blocking
   flgs  = fcntl(s, F_GETFL);
   flgs |= O_NONBLOCK;
   if (fcntl(s, F_SETFL, flgs) == -1)
   {  close(s);
      return(-1);
   };

   *sp = s;

   return(0);
}


int
vicus_getunixinfo(
         const char *                  path,
         vicus_addrinfo_t **           resp )
{
   size_t                     path_maxlen;
   size_t                     size;
   vicus_addrinfo_t *         dst;
   struct sockaddr_un *       sa;

   VicusTrace();
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
   dst->ai_addr   = sa;

   *resp = dst;

   return(0);
}


int
vicus_net_get_fd(
         vicus_t *                     vd,
         int *                         fdp )
{
   VicusTrace();
   assert(vd  != NULL);
   assert(fdp != NULL);

   if (!(vd->sock))
   {  *fdp = -1;
      return(0);
   };

   *fdp = vd->sock->s;

   return(0);
}


int
vicus_net_initialize(
         vicus_t *                     vd )
{
   VicusTrace();
   assert(vd != NULL);
   return(0);
}


int
vicus_net_set_fd(
         vicus_t *                     vd,
         int                           fd )
{
   VicusTrace();
   assert(vd  != NULL);

   if (fd == -1)
      return(VICUS_EINVAL);
   if ((vd->sock))
      return(VICUS_ENOTSUP);

   if ((vd->sock = malloc(sizeof(vicus_socket_t))) == NULL)
      return(VICUS_ENOMEM);
   vd->sock->s = fd;

   return(0);
}


int
vicus_net_terminate(
         vicus_t *                     vd )
{
   VicusTrace();
   assert(vd != NULL);
   return(0);
}


ssize_t
vicus_recv(
         vicus_t *                     vd,
         void *                        buff,
         size_t                        len )
{
   ssize_t           size;
   struct pollfd     fds;

   VicusTrace();
   assert(vd != NULL);

   if (vd->sock == NULL)
      return(VICUS_ECONNECT);

   fds.fd      = vd->sock->s;
   fds.revents = 0;
   fds.events  = POLLIN;
   if (poll(&fds, 1, (vd->net_timeout*1000)) == -1)
      return(VICUS_EUNKNOWN);
   if ( ((fds.revents & POLLHUP)) || ((fds.revents & POLLNVAL)) )
   {  vicus_close(vd);
      return(VICUS_ESERVER);
   };
   if (!(fds.revents & POLLIN))
   {  vicus_close(vd);
      return(VICUS_EUNKNOWN);
   };
   if ((size = recv(vd->sock->s, buff, len, 0)) == -1)
   {  vicus_close(vd);
      return(VICUS_EUNKNOWN);
   };

   return(size);
}


ssize_t
vicus_send(
         vicus_t *                     vd,
         const void *                  buff,
         size_t                        len )
{
   ssize_t           size;
   struct pollfd     fds;

   VicusTrace();
   assert(vd != NULL);

   if (vd->sock == NULL)
      return(VICUS_ECONNECT);

   fds.fd      = vd->sock->s;
   fds.revents = 0;
   fds.events  = POLLOUT;
   if (poll(&fds, 1, (vd->net_timeout*1000)) == -1)
      return(VICUS_EUNKNOWN);
   if (!(fds.revents & POLLOUT))
   {  vicus_close(vd);
      return(VICUS_EUNKNOWN);
   };
   if ((size = send(vd->sock->s, buff, len, 0)) == -1)
   {  vicus_close(vd);
      return(VICUS_EUNKNOWN);
   };

   return(size);
}


/* end of source */
