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
         vicus_urldesc_t **            vudpp );


static int
vicus_url_parse_unix(
         char *                        path,
         vicus_urldesc_t **            vudpp );


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
   vicus_urldesc_t *    vudp_cur;
   vicus_urldesc_t *    vudp_next;

   if (!(vudp))
      return;

   for(vudp_cur = vudp; ((vudp_cur)); vudp_cur = vudp_next)
   {  vudp_next = vudp_cur->vud_next;
      if ((vudp_cur->vud_uri))
         free(vudp_cur->vud_uri);
      if ((vudp_cur->vud_addrinfo))
         vicus_freeaddrinfo(vudp_cur->vud_addrinfo);
      free(vudp_cur);
   };

   return;
}


int
vicus_url_debug(
         vicus_t *                     vd,
         vicus_urldesc_t *             vudp )
{
   vicus_urldesc_t *    vudp_cur;
   vicus_addrinfo_t *   ai;
   char                 addrstr[256];

   VicusTrace();

   if ((vd))
      vudp = vd->vudp;

   for(vudp_cur = vudp; ((vudp_cur)); vudp_cur = vudp_cur->vud_next)
   {  VicusDebug("   URL: %s\n", vudp_cur->vud_uri);
      for(ai = vudp_cur->vud_addrinfo; ((ai)); ai = ai->ai_next)
      {  vicus_ntop(ai, addrstr, sizeof(addrstr));
         VicusDebug("        addresses: %s\n", addrstr);
      };
   };

   return(0);
}


int
vicus_url_parse(
         const char *                  url,
         vicus_urldesc_t **            vudpp )
{
   int                  rc;
   const char *         url_next;
   char                 url_str[VICUS_URI_LENGTH];
   size_t               url_strlen;
   size_t               len;
   vicus_urldesc_t *    vudp;
   vicus_urldesc_t **   vudp_cur;

   VicusTrace();

   assert(url     != NULL);
   assert(vudpp   != NULL);

   vudp     = NULL;
   vudp_cur = &vudp;

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
      {  if ((rc = vicus_url_parse_unix(url_str, vudp_cur)) != VICUS_SUCCESS)
         {  ldap_free_urldesc(vudp);
            return(rc);
         };
         vudp_cur = &(*vudp_cur)->vud_next;
         url      = &url_next[1];
         continue;
      };

      // process UNIX scheme
      len = strlen("unix://");
      if (!(strncasecmp(url_str, "unix://", len)))
      {  if ((rc = vicus_url_parse_unix(&url_str[len], vudp_cur)) != VICUS_SUCCESS)
         {  ldap_free_urldesc(vudp);
            return(rc);
         };
         vudp_cur = &(*vudp_cur)->vud_next;
         url      = &url_next[1];
         continue;
      };

      // process TCP scheme
      len = strlen("tcp://");
      if (!(strncasecmp(url_str, "tcp://", len)))
      {  if ((rc = vicus_url_parse_tcp(&url_str[len], vudp_cur)) != VICUS_SUCCESS)
         {  ldap_free_urldesc(vudp);
            return(rc);
         };
         vudp_cur = &(*vudp_cur)->vud_next;
         url      = &url_next[1];
         continue;
      };

      ldap_free_urldesc(vudp);
      return(VICUS_EURI);
   };

   if (!(vudp))
      return(VICUS_EURI);

   if ((vicus_opt_debug))
      vicus_url_debug(NULL, vudp);

   *vudpp = vudp;

   return(VICUS_SUCCESS);
}


int
vicus_url_parse_tcp(
         char *                        str,
         vicus_urldesc_t **            vudpp )
{
   int                  rc;
   int                  is_ipv6;
   int                  port;
   size_t               urilen;
   vicus_urldesc_t *    vudp;
   char *               host_str;
   char *               port_str;
   char *               ptr;

   VicusTrace();

   assert(str        != NULL);
   assert(vudpp      != NULL);

   is_ipv6  = 0;
   host_str = NULL;
   port_str = NULL;

   if (str[0] == '\0')
      return(VICUS_EURI);

   // parse hostname/IP address
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

   // parse port
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

   // allocate memory for new URL description
   if ((vudp = malloc(sizeof(vicus_urldesc_t))) == NULL)
      return(VICUS_ENOMEM);
   memset(vudp, 0, sizeof(vicus_urldesc_t));

   *vudpp            = vudp;

   // copies URL string
   urilen = strlen(host_str) + strlen("tcp://") + 1;
   if ((is_ipv6))
      urilen += 2;
   if ((port_str))
      urilen += 1 + strlen(port_str);
   if ((vudp->vud_uri = malloc(urilen)) == NULL)
      return(VICUS_ENOMEM);
   vicus_strlcpy(vudp->vud_uri, "tcp://",    urilen);
   if ((is_ipv6))
      vicus_strlcat(vudp->vud_uri, "[",      urilen);
   vicus_strlcat(vudp->vud_uri, host_str,    urilen);
   if ((is_ipv6))
      vicus_strlcat(vudp->vud_uri, "]",      urilen);
   if ((port_str))
   {  vicus_strlcat(vudp->vud_uri, ":",      urilen);
      vicus_strlcat(vudp->vud_uri, port_str, urilen);
   };

   // resolve hostname/IP address
   if ((rc = vicus_getaddrinfo(host_str, port_str, &vudp->vud_addrinfo)) != VICUS_SUCCESS)
      return(rc);

   return(0);
}


int
vicus_url_parse_unix(
         char *                        path,
         vicus_urldesc_t **            vudpp )
{
   int                  rc;
   size_t               urilen;
   vicus_urldesc_t *    vudp;

   VicusTrace();

   assert(path       != NULL);
   assert(vudpp      != NULL);

   if (path[0] == '\0')
      return(VICUS_EURI);

   // allocate memory for new URL description
   if ((vudp = malloc(sizeof(vicus_urldesc_t))) == NULL)
      return(VICUS_ENOMEM);
   memset(vudp, 0, sizeof(vicus_urldesc_t));
   *vudpp = vudp;

   // copies URL string
   urilen = strlen(path) + strlen("unix://") + 1;
   if ((vudp->vud_uri = malloc(urilen)) == NULL)
      return(VICUS_ENOMEM);
   vicus_strlcpy(vudp->vud_uri, "unix://", urilen);
   vicus_strlcat(vudp->vud_uri, path,      urilen);

   // allocates struct sockaddr
   if ((rc = vicus_getunixinfo(path, &vudp->vud_addrinfo)) != VICUS_SUCCESS)
      return(rc);

   return(0);
}


/* end of source */
