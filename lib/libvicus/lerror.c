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
#define __LIB_LIBVICUS_LERROR_C 1
#include "libvicus.h"


///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include <assert.h>


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

const char *
vicus_strerror(
         int                           err )
{
   VicusTrace();
   switch(err)
   {  case VICUS_SUCCESS:              return("success");
      case VICUS_ECONNECT:             return("connection error");
      case VICUS_EDNSRES:              return("unable to resolve hostname");
      case VICUS_EINVAL:               return("invalid argument");
      case VICUS_ENOMEM:               return("out of memory");
      case VICUS_ENOTSUP:              return("request or operation not supported");
      case VICUS_EPKT:                 return("invalid packet");
      case VICUS_ESERVER:              return("server down or connection closed");
      case VICUS_ESIZE:                return("input buffer too small");
      case VICUS_EUNBAL:               return("unbalanced message elements");
      case VICUS_EUNKNOWN:             break;
      case VICUS_EURI:                 return("invalid URI");
      default:                         break;
   };
   return("unknown error");
}


/* end of source */
