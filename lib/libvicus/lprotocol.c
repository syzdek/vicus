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
#define __LIB_LIBVICUS_LPACKET_C 1
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
#include <stdio.h>
#include <ctype.h>


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


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

//-------------------//
// message functions //
//-------------------//
// MARK: message functions

int
vicus_cmd_init(
         const char *                  command,
         vicus_msg_t **                msgp )
{
   VicusTrace();
   assert(command != NULL);
   assert(msgp    != NULL);
   return(vicus_msg_alloc(VICUS_CMD_REQUEST, command, msgp));
}


int
vicus_cmd_reinit(
         const char *                  command,
         vicus_msg_t *                 msg )
{
   VicusTrace();
   assert(command != NULL);
   assert(msg     != NULL);
   if (msg->pkt->msg[0] != VICUS_CMD_REQUEST)
      return(VICUS_ENOTSUP);
   return(vicus_msg_reinit(VICUS_CMD_REQUEST, command, msg));
}


int
vicus_msg_append(
         vicus_msg_t *                 msg,
         int                           type,
         const char *                  name,
         const void *                  val,
         size_t                        vallen )
{
   size_t         namelen;
   size_t         pktlen;
   size_t         off;
   size_t         size;
   void *         ptr;
   vicus_pkt_t *  pkt;

   VicusTrace();
   assert(msg  != NULL);

   // basic sanity checks
   namelen = ((name)) ? strlen(name) : 0;
   if (namelen > 255)
      return(VICUS_EINVAL);
   if (vallen > 0xffff)
      return(VICUS_EINVAL);
   if (msg->pkt->msg[0] != VICUS_CMD_REQUEST)
      return(VICUS_ENOTSUP);

   // type specific checks and determine new packet length
   pktlen = vicus_pkt_len(msg->pkt) + 1; // 1 byte: element type
   switch(type)
   {  case VICUS_KEY_VALUE:
         if ( (!(name)) || (!(val)) || (!(vallen)) )
            return(VICUS_EINVAL);
         pktlen += 3 + namelen + vallen; // 1 byte: name length; 2 bytes: value length
         break;

      case VICUS_LIST_END:
         if ( ((name)) || ((val)) || ((vallen)) )
            return(VICUS_EINVAL);
         if ( (!(msg->depth)) || (!(msg->levels)) )
            return(VICUS_EUNBAL);
         if (msg->levels[msg->depth] != VICUS_LIST_START)
            return(VICUS_EUNBAL);
         break;

      case VICUS_SECT_END:
         if ( ((name)) || ((val)) || ((vallen)) )
            return(VICUS_EINVAL);
         if ( (!(msg->depth)) || (!(msg->levels)) )
            return(VICUS_EUNBAL);
         if (msg->levels[msg->depth] != VICUS_SECT_START)
            return(VICUS_EUNBAL);
         break;

      case VICUS_LIST_ITEM:
         if ( ((name)) || (!(val)) || (!(vallen)) )
            return(VICUS_EINVAL);
         pktlen += 2 + vallen; // 2 bytes: value length
         break;

      case VICUS_LIST_START:
      case VICUS_SECT_START:
         if ( (!(name)) || ((val)) || ((vallen)) )
            return(VICUS_EINVAL);
         if ((ptr = realloc(msg->levels, ((msg->depth+1)*(sizeof(int))))) == NULL)
            return(VICUS_ENOMEM);
         msg->levels  = ptr;
         pktlen      += 1 + namelen;  // 1 byte: name length
         break;

      default:
         return(VICUS_ENOTSUP);
   };

   // increases memory allocation for packet
   size = pktlen + 4;
   if (size > msg->pkt_size)
   {  size += ((size % 128))
            ? (128 - (size % 128))
            : 0;
      if ((pkt = realloc(msg->pkt, size)) == NULL)
         return(VICUS_ENOMEM);
      msg->pkt       = pkt;
      msg->pkt_size  = size;
   };
   pkt = msg->pkt;

   // copies data into packet
   off                  = vicus_pkt_len(msg->pkt);
   msg->pkt->msg[off++] = (uint8_t)type;
   switch(type)
   {  case VICUS_KEY_VALUE:
         msg->pkt->msg[off++]  = (uint8_t)namelen;
         memcpy(&msg->pkt->msg[off], name, namelen);
         off += namelen;
         msg->pkt->msg[off++]  = (vallen >> 8) & 0x00ff;
         msg->pkt->msg[off++]  = (vallen >> 0) & 0x00ff;
         memcpy(&msg->pkt->msg[off], val, vallen);
         off += vallen;
         break;

      case VICUS_LIST_END:
      case VICUS_SECT_END:
         break;

      case VICUS_LIST_ITEM:
         msg->pkt->msg[off++]  = (vallen >> 8) & 0x00ff;
         msg->pkt->msg[off++]  = (vallen >> 0) & 0x00ff;
         memcpy(&msg->pkt->msg[off], val, vallen);
         off += vallen;
         break;

      case VICUS_LIST_START:
      case VICUS_SECT_START:
         msg->pkt->msg[off++]  = (uint8_t)namelen;
         memcpy(&msg->pkt->msg[off], name, namelen);
         off += namelen;
         break;

      default:
         return(VICUS_ENOTSUP);
   };
   msg->pkt->len = vicus_hton32((uint32_t)pktlen);

   return(VICUS_SUCCESS);
}


int
vicus_msg_append_str(
         vicus_msg_t *                 msg,
         int                           type,
         const char *                  name,
         const char *                  str )
{
   size_t len;
   VicusTrace();
   assert(msg  != NULL);
   len = ((str)) ? strlen(str) : 0;
   return(vicus_msg_append(msg, type, name, str, len));
}


int
vicus_msg_alloc(
         int                           type,
         const char *                  name,
         vicus_msg_t **                msgp )
{
   int               rc;
   vicus_msg_t *     msg;

   VicusTrace();
   assert(msgp != NULL);

   if ((msg = malloc(sizeof(vicus_msg_t))) == NULL)
      return(VICUS_ENOMEM);
   memset(msg, 0, sizeof(vicus_msg_t));

   if ((rc = vicus_pkt_alloc(type, name, &msg->pkt, 0)) != VICUS_SUCCESS)
   {  vicus_msg_free(msg);
      return(rc);
   };
   msg->pkt_size = vicus_pkt_len(msg->pkt);

   *msgp = msg;

   return(VICUS_SUCCESS);
}


void
vicus_msg_free(
         vicus_msg_t *                 msg )
{
   VicusTrace();
   if (!(msg))
      return;
   if ((msg->levels))
      free(msg->levels);
   vicus_pkt_free(msg->pkt);
   free(msg);
   return;
}


int
vicus_msg_pkt(
         vicus_msg_t *                 msg,
         const vicus_pkt_t **          pktp,
         size_t *                      pktlenp )
{
   VicusTrace();
   assert(msg  != NULL);
   if ((pktp))
      *pktp = msg->pkt;
   if ((pktlenp))
      *pktlenp = vicus_pkt_len(msg->pkt) + 4;
   return(0);
}


int
vicus_msg_reinit(
         int                           type,
         const char *                  name,
         vicus_msg_t *                 msg )
{
   int      rc;

   VicusTrace();

   if ((rc = vicus_pkt_alloc(type, name, &msg->pkt, msg->pkt_size)) != VICUS_SUCCESS)
      return(rc);
   if (vicus_pkt_len(msg->pkt) > msg->pkt_size)
      msg->pkt_size = vicus_pkt_len(msg->pkt);

   return(VICUS_SUCCESS);
}


//------------------//
// packet functions //
//------------------//
// MARK: packet functions

int
vicus_pkt_alloc(
         int                           type,
         const char *                  name,
         vicus_pkt_t **                pktp,
         size_t                        pktsize )
{
   size_t               size;
   size_t               namelen;
   vicus_pkt_t *        pkt;

   VicusTrace();
   assert(pktp != NULL);

   namelen  = ((name)) ? strlen(name) + 1 : 0;
   size     = 5 + namelen;

   if (namelen > 255)
      return(VICUS_EINVAL);

   if (size > pktsize)
   {  if ((pkt = realloc(*pktp, size)) == NULL)
         return(VICUS_ENOMEM);
      *pktp = pkt;
   };
   pkt = *pktp;

   pkt->msg[0] = (uint8_t)type;
   vicus_pkt_len_set(pkt, (1 + namelen));

   if ((name))
   {  namelen--;
      pkt->msg[1] = namelen;
      memcpy(&pkt->msg[2], name, namelen);
   };

   return(VICUS_SUCCESS);
}


void
vicus_pkt_free(
         vicus_pkt_t *                 pkt )
{
   VicusTrace();
   if (!(pkt))
      return;
   free(pkt);
   return;
}


size_t
vicus_pkt_len(
         const vicus_pkt_t *           pkt )
{
   assert(pkt != NULL);
   return( vicus_hton32(pkt->len) );
}


size_t
vicus_pkt_len_set(
         vicus_pkt_t *                 pkt,
         size_t                        len )
{
   assert(pkt != NULL);
   pkt->len = vicus_hton32( (uint32_t)len );
   return( len );
}


int
vicus_pkt_validate(
         const vicus_pkt_t *           pkt )
{
   size_t      pktlen;
   size_t      off;
   size_t      len;

   assert(pkt != NULL);

   pktlen   = vicus_pkt_len(pkt);
   off      = 1;

   if (pktlen < 1)
      return(VICUS_EPKT);

   switch(pkt->msg[0])
   {  case VICUS_CMD_RESPONSE:
      case VICUS_CMD_UNKNOWN:
      case VICUS_EVENT_CONFIRM:
      case VICUS_EVENT_UNKNOWN:
         if (pktlen != 1)
            return(VICUS_EPKT);
         return(VICUS_SUCCESS);

      case VICUS_CMD_REQUEST:
      case VICUS_EVENT_REGISTER:
      case VICUS_EVENT_UNREGISTER:
      case VICUS_EVENT:
         if (pktlen < 3)
            return(VICUS_EPKT);
         if (pktlen < (off+pkt->msg[off]+1))
            return(VICUS_EPKT);
         off += 1 + pkt->msg[off];
         break;

      default:
         return(VICUS_EPKT);
   }

   for(off = off+1; (off < pktlen); off++)
   {  switch(pkt->msg[off])
      {  case VICUS_LIST_END:
         case VICUS_SECT_END:
            break;

         case VICUS_LIST_START:
         case VICUS_SECT_START:
            off += pkt->msg[off] + 1;
            break;

         case VICUS_KEY_VALUE:
            off += pkt->msg[off] + 1;
            len  = ((pkt->msg[off] & 0xff) << 8) | (pkt->msg[off+1] & 0xff);
            off += len +2;
            break;

         case VICUS_LIST_ITEM:
            printf("VICUS_LIST_ITEM\n");
            len  = ((pkt->msg[off] & 0xff) << 8) | (pkt->msg[off+1] & 0xff);
            off += len + 2;
            break;

         default:
            printf("UNKNOWN\n");
            return(VICUS_EPKT);
      };
      if (off > pktlen)
         return(VICUS_EPKT);
   };

   if ((off-1) != pktlen)
      return(VICUS_EPKT);

   return(VICUS_SUCCESS);
}


/* end of source */
