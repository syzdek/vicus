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
#define __LIB_LIBVICUS_LMISC_C 1
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

// MARK: vicus_base64_chars[]
static const char * vicus_base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

int
vicus_base64_encode(
         char *                        dst,
         size_t                        s,
         const uint8_t *               src,
         size_t                        n )
{
   size_t         req_len;
   ssize_t        len;
   size_t         dpos;
   size_t         spos;
   size_t         byte;
   uint8_t *      dat;

   assert(dst != NULL);
   assert(src != NULL);
   assert(s   >  0);

   // determine if enough space is available to encode using base64
   req_len = ((n / 3) + (((n % 3)) ? 1 : 0)) * 4;
   if (s <= (req_len+1))
      return(VICUS_ESIZE);

   dat = (uint8_t *)dst;

   // calculates each digit's value
   byte = 0;
   dpos = 0;
   for(spos = 0; (spos < n); spos++)
   {
      // MSB is Most Significant Bits  (0x80 == 10000000 ~= MSB)
      // MB is middle bits             (0x7E == 01111110 ~= MB)
      // LSB is Least Significant Bits (0x01 == 00000001 ~= LSB)
      switch(byte)
      {
         case 0:
         dat[dpos++]  = (src[spos] & 0xfc) >> 2;  // 6 MSB
         dat[dpos++]  = (src[spos] & 0x03) << 4;  // 2 LSB
         byte++;
         break;

         case 1:
         dat[dpos-1] |= (src[spos] & 0xf0) >> 4;  // 4 MSB
         dat[dpos++]  = (src[spos] & 0x0f) << 2;  // 4 LSB
         byte++;
         break;

         case 2:
         default:
         dat[dpos-1] |= (src[spos] & 0xc0) >> 6;  // 2 MSB
         dat[dpos++]  =  src[spos] & 0x3f;        // 6 LSB
         byte = 0;
         break;
      };
   };

   // encodes each value
   for(len = 0; ((size_t)len) < dpos; len++)
      dst[len] = vicus_base64_chars[dat[len]];

   // add padding
   for(; ((len % 4)); len++)
      dst[len] = '=';

   dst[len] = '\0';

   return((int)len);
}


int
vicus_hexdump(
         const void *                  dat,
         size_t                        datlen )
{
   size_t               x;
   const uint8_t *      b;
   char                 str[17];

   assert(dat != NULL);

   b        = dat;
   str[16]  = '\0';

   for(x = 0; (x < datlen); x++)
   {  if (!(x & 0x00ff))
         printf("\n offset    0  1  2  3   4  5  6  7   8  9  a  b   c  d  e  f  0123456789abcdef\n");
      str[x&0x0f] = ((isprint((int)b[x]))) ? (char)b[x] : '.';
      switch(x & 0x000f)
      {  case 0x00:  printf("%08x  %02x", (unsigned)(x & ~0x0f), b[x]); break;
         case 0x04:  printf("  %02x", b[x]); break;
         case 0x08:  printf("  %02x", b[x]); break;
         case 0x0c:  printf("  %02x", b[x]); break;
         case 0x0f:  printf(" %02x  %s\n", b[x], str); break;
         default:    printf(" %02x", b[x]); break;
      };
   };
   for(; ((x & 0x0f)); x++)
   {  str[x&0x0f] = ' ';
      switch(x & 0x000f)
      {  case 0x04:  printf("    "); break;
         case 0x08:  printf("    "); break;
         case 0x0c:  printf("    "); break;
         case 0x0f:  printf("     %s\n", str); break;
         default:    printf("   "); break;
      };
   };

   return(0);
}


size_t
vicus_strlcat(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize )
{
   size_t      len;

   assert(src     != NULL);
   assert(dstsize  > 0);

   for(len = 0; ((*dst)); len++, dst++);
   if (!(src))
      return(len);

   dstsize--;
   for(; ( (len < dstsize) && ((*dst = *src)) ); len++, dst++, src++);
   *dst = '\0';

   for(; ((*src)); len++, src++);

   return(len);
}


size_t
vicus_strlcpy(
         char * restrict               dst,
         const char * restrict         src,
         size_t                        dstsize )
{
   size_t      len;

   assert(dst     != NULL);
   assert(src     != NULL);
   assert(dstsize  > 0);

   dstsize--;
   for(len = 0; ( (len < dstsize) && ((*dst = *src)) ); len++, dst++, src++);
   *dst = '\0';

   for(; ((*src)); len++, src++);

   return(len);
}

/* end of source */
