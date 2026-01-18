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
#define __LIB_LIBVICUS_LURL_C 1
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

static int
vicus_url_parse_tcp(
         char *                        str,
         vicus_urldesc_t **            vudpp,
         int *                         vudp_idxp );


static int
vicus_url_parse_unix(
         char *                        path,
         vicus_urldesc_t **            vudpp,
         int *                         vudp_idxp );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

void
ldap_free_urldesc(
         vicus_urldesc_t *             vudp )
{
   int idx;

   if (!(vudp))
      return;

   for(idx = 0; ((vudp[idx].vud_proto)); idx++)
   {  if ((vudp[idx].vud_host))
         free(vudp[idx].vud_host);
      if ((vudp[idx].vud_uri))
         free(vudp[idx].vud_uri);
      if ((vudp[idx].vud_addrinfo))
         freeaddrinfo(vudp[idx].vud_addrinfo);
   };
   free(vudp);

   return;
}


int
vicus_url_parse(
         const char *                  url,
         vicus_urldesc_t **            vudpp )
{
   int                  rc;
   int                  vudp_idx;
   const char *         url_next;
   char                 url_str[VICUS_URI_LENGTH];
   size_t               url_strlen;
   size_t               len;
   vicus_urldesc_t *    vudp;

   assert(url     != NULL);
   assert(vudpp   != NULL);

   vudp     = NULL;
   vudp_idx = 0;

   while(url[0] != '\0')
   {  // determine end of URL
      if ((url_next = strchr(url, ' ')) == NULL)
      {  url_strlen = strlen(url);
         url_next = &url[url_strlen-1];
      } else
      {  url_strlen = (size_t)(url_next - url);
      };
      if (!(url_strlen))
      {  url = &url_next[1];
         continue;
      };
      if (url_strlen >= VICUS_URI_LENGTH)
      {  ldap_free_urldesc(vudp);
         return(VICUS_EURI);
      };
      vicus_strlcpy(url_str, url, url_strlen+1);

      // process path as UNIX socket
      if (url_str[0] == '/')
      {  if ((rc = vicus_url_parse_unix(url_str, &vudp, &vudp_idx)) != VICUS_SUCCESS)
         {  ldap_free_urldesc(vudp);
            return(rc);
         };
         url = &url_next[1];
         continue;
      };

      // process UNIX scheme
      len = strlen("unix://");
      if (!(strncasecmp(url_str, "unix://", len)))
      {  if ((rc = vicus_url_parse_unix(&url_str[len], &vudp, &vudp_idx)) != VICUS_SUCCESS)
         {  ldap_free_urldesc(vudp);
            return(rc);
         };
         url = &url_next[1];
         continue;
      };

      // process TCP scheme
      len = strlen("tcp://");
      if (!(strncasecmp(url_str, "tcp://", len)))
      {  if ((rc = vicus_url_parse_tcp(&url_str[len], &vudp, &vudp_idx)) != VICUS_SUCCESS)
         {  ldap_free_urldesc(vudp);
            return(rc);
         };
         url = &url_next[1];
         continue;
      };

      ldap_free_urldesc(vudp);
      return(VICUS_EURI);
   };

   if (!(vudp))
      return(VICUS_EURI);

   *vudpp = vudp;

   return(VICUS_SUCCESS);
}


int
vicus_url_parse_tcp(
         char *                        str,
         vicus_urldesc_t **            vudpp,
         int *                         vudp_idxp )
{
   int                  rc;
   int                  vudp_idx;
   int                  is_ipv6;
   int                  port;
   size_t               size;
   size_t               urilen;
   vicus_urldesc_t *    vudp;
   char *               host_str;
   char *               port_str;
   char *               ptr;
   struct addrinfo      hints;
   struct addrinfo *    ai;

   assert(str        != NULL);
   assert(vudpp      != NULL);
   assert(vudp_idxp  != NULL);

   vudp_idx = *vudp_idxp;

   is_ipv6  = 0;
   host_str = NULL;
   port_str = NULL;
   port     = 0;

   if (str[0] == '\0')
      return(VICUS_EURI);

   // process hostname/IP address
   host_str = str;
   if (str[0] == '[')
   {  host_str = &str[1];
      if (!(ptr = strchr(str, ']')))
         return(VICUS_EURI);
      str      = ptr;
      str[0]   = '\0';
      str++;
      is_ipv6  = 1;
   };
   if (host_str[0] == '\0')
      return(VICUS_EURI);

   // process port
   if ((ptr = strchr(str, ':')) != NULL)
   {  port_str    = &ptr[0];
      port_str[0] = '\0';
      port_str++;
      if (port_str[0] == '\0')
         return(VICUS_EURI);
      if ((port = (int)strtol(port_str, NULL, 10)) == 0)
         return(VICUS_EURI);
      if (port < 0)
         return(VICUS_EURI);
   };

   // resolve hostname/IP address
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_protocol = IPPROTO_TCP;
   hints.ai_socktype = SOCK_STREAM;
   if ((rc = getaddrinfo(host_str, port_str, &hints, &ai)) != 0)
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

   // allocate memory for new URL description
   size = sizeof(vicus_urldesc_t) * (size_t)(vudp_idx+2);
   if ((vudp = realloc(*vudpp, size)) == NULL)
   {  freeaddrinfo(ai);
      return(VICUS_ENOMEM);
   };
   *vudpp = vudp;
   memset(&vudp[vudp_idx+0], 0, sizeof(vicus_urldesc_t));
   memset(&vudp[vudp_idx+1], 0, sizeof(vicus_urldesc_t));

   vudp[vudp_idx].vud_proto = VICUS_PROTO_TCP;
   vudp[vudp_idx].vud_port  = -1;

   // copies URL string
   urilen = strlen(host_str) + strlen("tcp://") + 1;
   if ((is_ipv6))
      urilen += 2;
   if ((port_str))
      urilen += 1 + strlen(port_str);
   if ((vudp[vudp_idx].vud_uri = malloc(urilen)) == NULL)
      return(VICUS_ENOMEM);
   vicus_strlcpy(vudp[vudp_idx].vud_uri, "tcp://",    urilen);
   if ((is_ipv6))
      vicus_strlcat(vudp[vudp_idx].vud_uri, "[",      urilen);
   vicus_strlcat(vudp[vudp_idx].vud_uri, host_str,    urilen);
   if ((is_ipv6))
      vicus_strlcat(vudp[vudp_idx].vud_uri, "]",      urilen);
   if ((port_str))
   {  vicus_strlcat(vudp[vudp_idx].vud_uri, ":",      urilen);
      vicus_strlcat(vudp[vudp_idx].vud_uri, port_str, urilen);
   };

   vudp[vudp_idx].vud_addrinfo  = ai;

   if ((vudp[vudp_idx].vud_host = strdup(host_str)) == NULL)
      return(VICUS_ENOMEM);

   if ((port_str))
      vudp[vudp_idx].vud_port = port;

   (*vudp_idxp)++;

   return(0);
}


int
vicus_url_parse_unix(
         char *                        path,
         vicus_urldesc_t **            vudpp,
         int *                         vudp_idxp )
{
   int                  vudp_idx;
   size_t               size;
   size_t               urilen;
   vicus_urldesc_t *    vudp;

   assert(path       != NULL);
   assert(vudpp      != NULL);
   assert(vudp_idxp  != NULL);

   vudp_idx = *vudp_idxp;

   if (path[0] == '\0')
      return(VICUS_EURI);

   // allocate memory for new URL description
   size = sizeof(vicus_urldesc_t) * (size_t)(vudp_idx+2);
   if ((vudp = realloc(*vudpp, size)) == NULL)
      return(VICUS_ENOMEM);
   *vudpp = vudp;
   memset(&vudp[vudp_idx+0], 0, sizeof(vicus_urldesc_t));
   memset(&vudp[vudp_idx+1], 0, sizeof(vicus_urldesc_t));

   vudp[vudp_idx].vud_proto = VICUS_PROTO_UNIX;
   vudp[vudp_idx].vud_port  = -1;

   urilen = strlen(path) + strlen("unix://") + 1;
   if ((vudp[vudp_idx].vud_uri = malloc(urilen)) == NULL)
      return(VICUS_ENOMEM);
   vicus_strlcpy(vudp[vudp_idx].vud_uri, "unix://", urilen);
   vicus_strlcat(vudp[vudp_idx].vud_uri, path,      urilen);

   if ((vudp[vudp_idx].vud_host = strdup(path)) == NULL)
      return(VICUS_ENOMEM);

   (*vudp_idxp)++;

   return(0);
}


/* end of source */
