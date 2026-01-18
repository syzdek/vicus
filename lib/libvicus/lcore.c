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
#define __LIB_LIBVICUS_LCORE_C 1
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
vicus_alloc(
         vicus_t **                    vdp );


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
vicus_alloc(
         vicus_t **                    vdp )
{
   int            rc;
   vicus_t *      vd;

   assert(vdp != NULL);

   if ((vd = malloc(sizeof(vicus_t))) == NULL)
      return(VICUS_ENOMEM);
   memset(vd, 0, sizeof(vicus_t));

   if ((rc = vicus_net_initialize(vd)) != VICUS_SUCCESS)
   {  free(vd);
      return(rc);
   };

   *vdp = vd;

   return(0);
}


//int
//vicus_connect(
//         vicus_t *                     vd );


int
vicus_disconnect(
         vicus_t *                     vd )
{
   if (!(vd))
      return(0);

   vicus_net_terminate(vd);

   if (vd->s != -1)
      close(vd->s);
   ldap_free_urldesc(vd->vudp);

   free(vd);

   return(0);
}


int
vicus_init_fd(
         vicus_t **                    vdp,
         int                           fd,
         int                           proto )
{
   int                  rc;
   vicus_t *            vd;

   assert(vdp != NULL);

   switch(proto)
   {  case VICUS_PROTO_TCP:      break;
      case VICUS_PROTO_UNIX:     break;
      default:                   return(-1);
   };

   if ((rc = vicus_alloc(&vd)) != VICUS_SUCCESS)
      return(rc);

   vd->s       = fd;
   vd->s_proto = proto;

   *vdp = vd;

   return(0);
}


int
vicus_initialize(
         vicus_t **                    vdp,
         const char *                  uri )
{
   int                  rc;
   vicus_t *            vd;
   vicus_urldesc_t *    vudp;

   assert(vdp != NULL);

   if ((rc = vicus_alloc(&vd)) != VICUS_SUCCESS)
      return(rc);

   if ((rc = vicus_url_parse(uri, &vudp)) != VICUS_SUCCESS)
      return(rc);
   vd->vudp = vudp;

   *vdp = vd;

   return(0);
}


/* end of source */
