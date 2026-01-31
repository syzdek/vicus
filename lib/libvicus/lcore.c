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
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdarg.h>


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


static int
vicus_get_option_global(
         int                           option,
         void *                        outvalue );


static int
vicus_set_option_global(
         int                           option,
         const void *                  invalue );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

int            vicus_opt_debug               = VICUS_DBG_NONE;
int            vicus_opt_debug_stderr        = VICUS_FALSE;
const char *   vicus_opt_debug_prefix        = "libvicus";
char *         vicus_opt_debug_prefix_buff   = NULL;


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

//----------------//
// core functions //
//----------------//
// MARK: core functions

int
vicus_alloc(
         vicus_t **                    vdp )
{
   int            rc;
   vicus_t *      vd;

   VicusTrace();
   assert(vdp != NULL);

   if ((vd = malloc(sizeof(vicus_t))) == NULL)
      return(VICUS_ENOMEM);
   memset(vd, 0, sizeof(vicus_t));
   vd->net_timeout   = VICUS_DFLT_NETTIME;
   vd->req_timeout   = VICUS_DFLT_REQTIME;

   if ((rc = vicus_mutext_alloc(&vd->mutex)) != VICUS_SUCCESS)
   {  vicus_disconnect(vd);
      return(rc);
   };

   if ((rc = vicus_net_initialize(vd)) != VICUS_SUCCESS)
   {  free(vd);
      return(rc);
   };

   *vdp = vd;

   return(0);
}


int
vicus_debug(
         const char *                  file,
         int                           line,
         const char *                  fmt,
         ... )
{
   FILE *         fs;
   int            len;
   va_list        ap;

   if (!(vicus_opt_debug & VICUS_DBG_INFO))
      return(0);

   len   = 0;
   fs    = (vicus_opt_debug_stderr == VICUS_FALSE)
         ? stdout
         : stderr;

   if ((vicus_opt_debug_prefix))
      len += fprintf(fs, "%s: ", vicus_opt_debug_prefix);

   if ((vicus_opt_debug & VICUS_DBG_SRC))
      len += fprintf(fs, "%s: %i: ", file, line);

   va_start(ap, fmt);
      len += vfprintf(fs, fmt, ap);
   va_end(ap);

   return(len);
}


int
vicus_debug_trace(
         const char *                  file,
         int                           line,
         const char *                  func )
{
   int            len;
   FILE *         fs;

   if (!(vicus_opt_debug & VICUS_DBG_TRACE))
      return(0);

   len   = 0;
   fs    = (vicus_opt_debug_stderr == VICUS_FALSE)
         ? stdout
         : stderr;

   if ((vicus_opt_debug_prefix))
      len += fprintf(fs, "%s: ", vicus_opt_debug_prefix);

   if ((vicus_opt_debug & VICUS_DBG_SRC))
      len += fprintf(fs, "%s: %i: ", file, line);

   len += fprintf(fs, "%s()\n", func);

   return(len);
}


int
vicus_disconnect(
         vicus_t *                     vd )
{
   VicusTrace();

   if (!(vd))
      return(0);

   vicus_mutext_lock(vd->mutex);
   vicus_mutext_free(&vd->mutex);

   vicus_close(vd);

   ldap_free_urldesc(vd->vudp);

   vicus_net_terminate(vd);

   free(vd);

   return(0);
}


void
vicus_free(
         void *                        ptr )
{
   VicusTrace();
   if (!(ptr))
      return;
   free(ptr);
   return;
}


int
vicus_get_option(
         vicus_t *                     vd,
         int                           option,
         void *                        outvalue )
{
   int         rc;

   VicusTrace();
   assert(outvalue != NULL);

   if (!(vd))
      return(vicus_get_option_global(option, outvalue));

   if ((rc = vicus_mutext_lock(vd->mutex)) != VICUS_SUCCESS)
      return(rc);

   rc = vicus_get_option_local(vd, option, outvalue);

   vicus_mutext_unlock(vd->mutex);

   return(rc);
}


int
vicus_get_option_local(
         vicus_t *                     vd,
         int                           option,
         void *                        outvalue )
{
   int         rc;
   char *      str;
   char        buff[256];
   size_t      len;

   VicusTrace();
   assert(outvalue != NULL);

   switch(option)
   {  case VICUS_OPT_CUR_ADDR:
         if (!(vd->sock_ai))
            return(VICUS_ENOTSUP);
         if (vicus_ntop(vd->sock_ai, buff, sizeof(buff)) != VICUS_SUCCESS)
            return(VICUS_EUNKNOWN);
         len = strlen(buff) + 1;
         if ((str = malloc(len)) == NULL)
            return(VICUS_ENOMEM);
         vicus_strlcpy(str, buff, len);
         *((char **)outvalue) = str;
         break;

      case VICUS_OPT_CUR_URL:
         if (!(vd->sock_vudp))
            return(VICUS_ENOTSUP);
         if ((str = strdup(vd->sock_vudp->vud_uri)) == NULL)
            return(VICUS_ENOMEM);
         *((char **)outvalue) = str;
         break;

      case VICUS_OPT_NETTIME:
         *((int *)outvalue) = vd->net_timeout;
         break;

      case VICUS_OPT_REQTIME:
         *((int *)outvalue) = vd->req_timeout;
         break;

      case VICUS_OPT_SOCKET:
         if ((rc = vicus_net_get_fd(vd, outvalue)) != VICUS_SUCCESS)
            return(rc);
         break;

      default:
         return(VICUS_EINVAL);
   };

   return(0);
}


int
vicus_get_option_global(
         int                           option,
         void *                        outvalue )
{
   char *      str;

   VicusTrace();
   assert(outvalue != NULL);

   switch(option)
   {
      case VICUS_OPT_DEBUG:
         *((int *)outvalue) = vicus_opt_debug;
         break;

      case VICUS_OPT_DEBUG_PREFIX:
         str = NULL;
         if ((vicus_opt_debug_prefix))
            if (!(str = strdup(vicus_opt_debug_prefix)))
               return(VICUS_ENOMEM);
         *((char **)outvalue)  = str;
         break;

      case VICUS_OPT_DEBUG_STDERR:
         *((int *)outvalue)  = vicus_opt_debug_stderr;
         break;

      default:
         return(VICUS_EINVAL);
   };

   return(0);
}


int
vicus_init_fd(
         vicus_t **                    vdp,
         int                           fd )
{
   int                  rc;
   vicus_t *            vd;

   VicusTrace();
   assert(vdp != NULL);

   if ((rc = vicus_alloc(&vd)) != VICUS_SUCCESS)
      return(rc);

   if ((rc = vicus_net_set_fd(vd, fd)) != VICUS_SUCCESS)
   {  vicus_disconnect(vd);
      return(rc);
   };

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

   VicusTrace();
   assert(vdp != NULL);

   uri = ((uri)) ? uri : VICUS_DFLT_URI;

   if ((rc = vicus_alloc(&vd)) != VICUS_SUCCESS)
      return(rc);

   if ((rc = vicus_url_parse(uri, &vudp)) != VICUS_SUCCESS)
      return(rc);
   vd->vudp = vudp;

   *vdp = vd;

   return(0);
}


int
vicus_set_option(
         vicus_t *                     vd,
         int                           option,
         const void *                  invalue )
{
   int   rc;

   VicusTrace();
   assert(invalue != NULL);

   if (!(vd))
      return(vicus_set_option_global(option, invalue));

   if ((rc = vicus_mutext_lock(vd->mutex)) != VICUS_SUCCESS)
      return(rc);

   rc = vicus_set_option_local(vd, option, invalue);

   vicus_mutext_unlock(vd->mutex);

   return(rc);
}


int
vicus_set_option_local(
         vicus_t *                     vd,
         int                           option,
         const void *                  invalue )
{
   switch(option)
   {  case VICUS_OPT_CUR_ADDR:
         return(VICUS_ENOTSUP);

      case VICUS_OPT_CUR_URL:
         return(VICUS_ENOTSUP);

      case VICUS_OPT_NETTIME:
         vd->net_timeout = *((const int *)invalue);
         break;

      case VICUS_OPT_REQTIME:
         vd->req_timeout = *((const int *)invalue);
         break;

      case VICUS_OPT_SOCKET:
         return(VICUS_ENOTSUP);

      default:
         return(VICUS_EINVAL);
   };

   return(0);
}


int
vicus_set_option_global(
         int                           option,
         const void *                  invalue )
{
   int      ival;
   char *   str;

   VicusTrace();
   assert(invalue != NULL);

   ival = *((const int *)invalue);

   switch(option)
   {
      case VICUS_OPT_DEBUG:
         vicus_opt_debug   = ival;
         break;

      case VICUS_OPT_DEBUG_PREFIX:
         str = NULL;
         if (( (const char *)invalue ))
            if (!(str = strdup( (const char *)invalue )))
               return(VICUS_ENOMEM);
         if ((vicus_opt_debug_prefix_buff))
            free(vicus_opt_debug_prefix_buff);
         vicus_opt_debug_prefix_buff   = str;
         vicus_opt_debug_prefix        = vicus_opt_debug_prefix_buff;
         break;

      case VICUS_OPT_DEBUG_STDERR:
         vicus_opt_debug_stderr  = ((ival)) ? VICUS_TRUE : VICUS_FALSE;
         break;

      default:
         return(VICUS_EINVAL);
   };

   return(0);
}


//------------------//
// thread functions //
//------------------//
// MARK: thread functions

int
vicus_mutext_alloc(
         pthread_mutex_t **            mutexp )
{
   pthread_mutex_t *    mutex;

   VicusTrace();
   assert(mutexp != NULL);

   if ((mutex = malloc(sizeof(pthread_mutex_t))) == NULL)
      return(VICUS_ENOMEM);
   if ((pthread_mutex_init(mutex, NULL)))
   {  free(mutex);
      return(VICUS_ENOMEM);
   };

   *mutexp = mutex;

   return(VICUS_SUCCESS);
}


void
vicus_mutext_free(
         pthread_mutex_t **            mutexp )
{
   VicusTrace();
   assert(mutexp != NULL);
   if (!(*mutexp))
      return;
   if (!(pthread_mutex_unlock(*mutexp)))
      pthread_mutex_destroy(*mutexp);
   free(*mutexp);
   *mutexp = NULL;
   return;
}


int
vicus_mutext_lock(
         pthread_mutex_t *             mutex )
{
   VicusTrace();
   if (!(mutex))
      return(VICUS_SUCCESS);
   if ((pthread_mutex_lock(mutex)))
      return(VICUS_EUNKNOWN);
   return(VICUS_SUCCESS);
}


int
vicus_mutext_unlock(
         pthread_mutex_t *             mutex )
{
   VicusTrace();
   if (!(mutex))
      return(0);
   pthread_mutex_unlock(mutex);
   return(0);
}

/* end of source */
