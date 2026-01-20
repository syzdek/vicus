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
/*
 *  src/cygnusctl.h - common includes and prototypes
 */
#ifndef __SRC_CYGNUSCTL_H
#define __SRC_CYGNUSCTL_H 1

///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

// defined in the Single UNIX Specification
#ifndef _XOPEN_SOURCE
#   define _XOPEN_SOURCE 600
#endif

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <vicus.h>
#include <vicus_noinst.h>
#include <inttypes.h>


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

#ifndef PROGRAM_NAME
#   define PROGRAM_NAME "cygnusctl"
#endif
#ifndef PACKAGE_BUGREPORT
#   define PACKAGE_BUGREPORT ""
#endif
#ifndef PACKAGE_COPYRIGHT
#   define PACKAGE_COPYRIGHT "Copyright (C) 2026 David M. Syzdek <david@syzdek.net>."
#endif
#ifndef PACKAGE_NAME
#   define PACKAGE_NAME "Versatile IKE Control User Software for strongSwan"
#endif
#ifndef PACKAGE_TARNAME
#   define PACKAGE_TARNAME "vicus"
#endif
#ifndef PACKAGE_VERSION
#   define PACKAGE_VERSION ""
#endif

#define MY_FLG_NOBLOCK        0x00000001
#define MY_FLG_PRETTY         0x00000002
#define MY_FLG_STREAM         0x00000004
#define MY_FLG_ALL_IKE        0x00000008
#define MY_FLG_LEASES         0x00000010
#define MY_FLG_FORCE          0x00000020
#define MY_FLG_POLS_DROP      0x00000040
#define MY_FLG_POLS_BYPASS    0x00000080
#define MY_FLG_POLS_TRAP      0x00000100
#define MY_FLG_REAUTH         0x00000200

#define MY_FMT_DEFAULT        0x00000000
#define MY_FMT_DEBUG          0x00000001
#define MY_FMT_VICI           0x00000002
#define MY_FMT_JSON           0x00000003
#define MY_FMT_YAML           0x00000004
#define MY_FMT_XML            0x00000005


//////////////////
//              //
//  Data Types  //
//              //
//////////////////
// MARK: - Data Types

typedef struct _my_config        my_config_t;
typedef struct _my_widget        my_widget_t;


struct _my_config
{  int                           verbose;
   int                           quiet;
   int                           symlinked;
   int                           queued;
   int                           argc;
   int                           flags;
   int                           format_out;
   int                           last_was_item;
   vicus_t *                     vd;
   char * const *                argv;
   const char *                  prog_name;
   const char *                  vici_uri;
   char *                        res_last_name;
   const char *                  alt_command;
   const char *                  alt_event;
   const char *                  ike_sa;
   const char *                  ike_sa_id;
   const char *                  child_sa;
   const char *                  child_sa_id;
   const char *                  opt_name;
   const char *                  opt_timeout;
   const char *                  opt_loglevel;
   const my_widget_t *           widget;
};


struct _my_widget
{  const char *               name;
   const char *               desc;
   const char *               vici_cmd;
   const char *               vici_event;
   const char * const *       aliases;
   const char *               usage;
   const char *               short_opt;
   const struct option *      long_opt;
   int                        arg_min;
   int                        arg_max;
   int                        alias_idx;
   int                        flags;
   int  (*func_exec)(my_config_t * cnf);
   int  (*func_usage)(my_config_t * cnf);
};


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

extern char *
my_prog_name(
         my_config_t *                 cnf );


extern void
my_verbose(
         my_config_t *                 cnf,
         const char *                  fmt,
         ... );


#endif /* end of header */
