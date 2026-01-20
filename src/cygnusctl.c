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
#define __SRC_CYGNUSCTL_C 1


///////////////
//           //
//  Headers  //
//           //
///////////////
// MARK: - Headers

#include "cygnusctl.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdarg.h>
#include <getopt.h>
#include <signal.h>


///////////////////
//               //
//  Definitions  //
//               //
///////////////////
// MARK: - Definitions

#define  MY_SOPT              "hO:Pqu:Vv"
#define  MY_SOPT_ALL_IKE      "a"
#define  MY_SOPT_BYPASS       "B"
#define  MY_SOPT_CHILD        "c:"
#define  MY_SOPT_CHILD_ID     "C:"
#define  MY_SOPT_DROP         "D"
#define  MY_SOPT_COMMAND      "e:"
#define  MY_SOPT_EVENT        "E:"
#define  MY_SOPT_FORCE        "f"
#define  MY_SOPT_IKE          "i:"
#define  MY_SOPT_IKE_ID       "I:"
#define  MY_SOPT_LEASES       "l"
#define  MY_SOPT_LOGLEVEL     "L:"
#define  MY_SOPT_NAME         "n:"
#define  MY_SOPT_NOBLOCK      "N"
#define  MY_SOPT_REAUTH       "A"
#define  MY_SOPT_TIMEOUT      "t:"
#define  MY_SOPT_TRAP         "T"


#define  MY_LOPT              { "help",            no_argument,         NULL, 'h' }, \
                              { "out-format",      required_argument,   NULL, 'O' }, \
                              { "pretty",          no_argument,         NULL, 'P' }, \
                              { "quiet",           no_argument,         NULL, 'q' }, \
                              { "silent",          no_argument,         NULL, 'q' }, \
                              { "socket",          required_argument,   NULL, 'u' }, \
                              { "version",         no_argument,         NULL, 'V' }, \
                              { "verbose",         no_argument,         NULL, 'v' }, \
                              { NULL, 0, NULL, 0 }
#define  MY_LOPT_ALL_IKE      { "all",             no_argument,         NULL, 'a' },
#define  MY_LOPT_BYPASS       { "bypass",          no_argument,         NULL, 'B' },
#define  MY_LOPT_CHILD        { "child",           required_argument,   NULL, 'c' },
#define  MY_LOPT_CHILD_ID     { "child-id",        required_argument,   NULL, 'C' },
#define  MY_LOPT_COMMAND      { "command",         required_argument,   NULL, 'e' },
#define  MY_LOPT_DROP         { "drop",            no_argument,         NULL, 'D' },
#define  MY_LOPT_EVENT        { "event",           required_argument,   NULL, 'E' },
#define  MY_LOPT_FORCE        { "force",           no_argument,         NULL, 'f' },
#define  MY_LOPT_IKE          { "ike",             required_argument,   NULL, 'i' },
#define  MY_LOPT_IKE_ID       { "ike-id",          required_argument,   NULL, 'I' },
#define  MY_LOPT_LEASES       { "leases",          no_argument,         NULL, 'l' },
#define  MY_LOPT_LOGLEVEL     { "loglevel",        required_argument,   NULL, 'L' },
#define  MY_LOPT_NAME         { "name",            required_argument,   NULL, 'n' },
#define  MY_LOPT_NOBLOCK      { "noblock",         no_argument,         NULL, 'N' },
#define  MY_LOPT_REAUTH       { "reauth",          no_argument,         NULL, 'A' },
#define  MY_LOPT_TIMEOUT      { "name",            required_argument,   NULL, 'n' },
#define  MY_LOPT_TRAP         { "trap",            no_argument,         NULL, 'T' },


//////////////
//          //
//  Macros  //
//          //
//////////////
// MARK: - Macros

#undef   MY_LOPTS
#define  MY_LOPTS(...) (const struct option []) { __VA_ARGS__ MY_LOPT }


/////////////////
//             //
//  Datatypes  //
//             //
/////////////////
#pragma mark - Datatypes


//////////////////
//              //
//  Prototypes  //
//              //
//////////////////
// MARK: - Prototypes

static int
my_version(
         my_config_t *                 cnf );


extern int
main(
         int                           argc,
         char **                       argv );


/////////////////
//             //
//  Variables  //
//             //
/////////////////
// MARK: - Variables

#pragma mark my_widget_map[]
static my_widget_t my_widget_map[] =
{
   // alert widget (TODO)
   {  .name          = "alert",
      .aliases       = NULL,
      .desc          = "displays alert events",
      .vici_cmd      = NULL,
      .vici_event    = "alert",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // child-updown widget (TODO)
   {  .name          = "child-updown",
      .aliases       = NULL,
      .desc          = "displays child-updown events",
      .vici_cmd      = NULL,
      .vici_event    = "child-updown",
      .flags         = MY_FLG_STREAM,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // child-rekey widget (TODO)
   {  .name          = "child-rekey",
      .aliases       = NULL,
      .desc          = "displays child-rekey events",
      .vici_cmd      = NULL,
      .vici_event    = "child-rekey",
      .flags         = MY_FLG_STREAM,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // clear-creds widget (TODO)
   {  .name          = "clear-creds",
      .aliases       = NULL,
      .desc          = "clears loaded certs, private keys and shared keys",
      .vici_cmd      = "clear-creds",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // diagnostics widget (TODO)
   {  .name          = "diagnostics",
      .aliases       = NULL,
      .desc          = "retreieves various diagnostics information",
      .vici_cmd      = "NONE",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT   MY_SOPT_IKE,
      .long_opt      = MY_LOPTS( MY_LOPT_IKE ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // flush-certs widget (TODO)
   {  .name          = "flush-certs",
      .aliases       = NULL,
      .desc          = "flushes the certificate cache",
      .vici_cmd      = "flush-certs",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // get-algorithms widget (TODO)
   {  .name          = "get-algorithms",
      .aliases       = NULL,
      .desc          = "lists loaded algorithms and their implementation",
      .vici_cmd      = "get-algorithms",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // get-authorities widget (TODO)
   {  .name          = "get-authorities",
      .aliases       = NULL,
      .desc          = "lists loaded CA names",
      .vici_cmd      = "get-authorities",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // get-conns widget (TODO)
   {  .name          = "get-conns",
      .aliases       = NULL,
      .desc          = "lists connections loaded over vici",
      .vici_cmd      = "get-conns",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // get-counters widget (TODO)
   {  .name          = "get-counters",
      .aliases       = NULL,
      .desc          = "lists global or connection-specific counters",
      .vici_cmd      = "get-counters",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT MY_SOPT_IKE MY_SOPT_ALL_IKE,
      .long_opt      = MY_LOPTS( MY_LOPT_ALL_IKE MY_LOPT_IKE ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // get-keys widget (TODO)
   {  .name          = "get-keys",
      .aliases       = NULL,
      .desc          = "lists private keys identifiers loaded over vici",
      .vici_cmd      = "get-keys",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // get-pools widget (TODO)
   {  .name          = "get-pools",
      .aliases       = NULL,
      .desc          = "lists loaded pools.",
      .vici_cmd      = "get-pools",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT MY_SOPT_LEASES MY_SOPT_NAME,
      .long_opt      = MY_LOPTS( MY_LOPT_LEASES MY_LOPT_NAME ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // get-shared widget (TODO)
   {  .name          = "get-shared",
      .aliases       = NULL,
      .desc          = "lists unique shared key identifiers loaded over vici",
      .vici_cmd      = "get-shared",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // ike-rekey widget (TODO)
   {  .name          = "ike-rekey",
      .aliases       = NULL,
      .desc          = "displays ike-rekey events",
      .vici_cmd      = NULL,
      .vici_event    = "ike-rekey",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // ike-update widget (TODO)
   {  .name          = "ike-update",
      .aliases       = NULL,
      .desc          = "displays ike-update events",
      .vici_cmd      = NULL,
      .vici_event    = "ike-update",
      .flags         = MY_FLG_STREAM,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // ike-updown widget (TODO)
   {  .name          = "ike-updown",
      .aliases       = NULL,
      .desc          = "displays ike-updown events",
      .vici_cmd      = NULL,
      .vici_event    = "ike-updown",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // initiate widget (TODO)
   {  .name          = "initiate",
      .aliases       = NULL,
      .desc          = "initiates an SA",
      .vici_cmd      = "initiate",
      .vici_event    = "control-log",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT MY_SOPT_CHILD MY_SOPT_IKE MY_SOPT_TIMEOUT MY_SOPT_LOGLEVEL,
      .long_opt      = MY_LOPTS( MY_LOPT_CHILD MY_LOPT_IKE MY_LOPT_TIMEOUT MY_LOPT_LOGLEVEL ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // install widget (TODO)
   {  .name          = "install",
      .aliases       = NULL,
      .desc          = "installs a CHILD_SA's 'trap, drop or bypass policy",
      .vici_cmd      = "install",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT MY_SOPT_CHILD MY_SOPT_IKE,
      .long_opt      = MY_LOPTS( MY_LOPT_CHILD MY_LOPT_IKE ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // list-authorities widget (TODO)
   {  .name          = "list-authorities",
      .aliases       = NULL,
      .desc          = "Lists loaded certification authorities",
      .vici_cmd      = "list-authorities",
      .vici_event    = "list-authority",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT MY_SOPT_NAME,
      .long_opt      = MY_LOPTS( MY_LOPT_NAME ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // list-certs widget (TODO)
   {  .name          = "list-certs",
      .aliases       = NULL,
      .desc          = "Lists loaded certificates",
      .vici_cmd      = "list-certs",
      .vici_event    = "list-cert",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // list-conns widget (TODO)
   {  .name          = "list-conns",
      .aliases       = NULL,
      .desc          = "lists all loaded connections",
      .vici_cmd      = "list-conns",
      .vici_event    = "list-conn",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT MY_SOPT_IKE,
      .long_opt      = MY_LOPTS( MY_LOPT_IKE ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // list-policies widget (TODO)
   {  .name          = "list-policies",
      .aliases       = NULL,
      .desc          = "lists installed trap, drop and bypass policies",
      .vici_cmd      = "list-policies",
      .vici_event    = "list-policy",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT   MY_SOPT_BYPASS MY_SOPT_CHILD MY_SOPT_DROP MY_SOPT_IKE MY_SOPT_TRAP,
      .long_opt      = MY_LOPTS( MY_LOPT_BYPASS MY_LOPT_CHILD MY_LOPT_DROP MY_LOPT_IKE MY_LOPT_TRAP ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // list-sas widget (TODO)
   {  .name          = "list-sas",
      .aliases       = NULL,
      .desc          = "lists active IKE_SAs and associated CHILD_SAs",
      .vici_cmd      = "list-sas",
      .vici_event    = "list-sa",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT MY_SOPT_CHILD MY_SOPT_CHILD_ID MY_SOPT_IKE MY_SOPT_IKE_ID MY_SOPT_NOBLOCK,
      .long_opt      = MY_LOPTS( MY_LOPT_CHILD MY_LOPT_CHILD_ID MY_LOPT_IKE MY_LOPT_IKE_ID MY_LOPT_NOBLOCK ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // load-authority widget (TODO)
   {  .name          = "load-authority",
      .aliases       = NULL,
      .desc          = "loads a certification authority into the daemon",
      .vici_cmd      = "load-authority",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // load-cert widget (TODO)
   {  .name          = "load-cert",
      .aliases       = NULL,
      .desc          = "loads a certificate into the daemon",
      .vici_cmd      = "load-cert",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // load-conn widget (TODO)
   {  .name          = "load-conn",
      .aliases       = NULL,
      .desc          = "loads a connection definition into the daemon",
      .vici_cmd      = "load-conn",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // load-key widget (TODO)
   {  .name          = "load-key",
      .aliases       = NULL,
      .desc          = "loads a private key into the daemon",
      .vici_cmd      = "load-key",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // load-pool widget (TODO)
   {  .name          = "load-pool",
      .aliases       = NULL,
      .desc          = "loads a virtual IP and attribute pool.",
      .vici_cmd      = "load-pool",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // load-shared widget (TODO)
   {  .name          = "load-shared",
      .aliases       = NULL,
      .desc          = "loads a shared IKE PSK, EAP, XAuth or NTLM secret into the daemon",
      .vici_cmd      = "load-shared",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // load-token widget (TODO)
   {  .name          = "load-token",
      .aliases       = NULL,
      .desc          = "loads a private key on a token into the daemon",
      .vici_cmd      = "load-token",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // log widget (TODO)
   {  .name          = "log",
      .aliases       = NULL,
      .desc          = "displays debug log messages",
      .vici_cmd      = NULL,
      .vici_event    = "log",
      .flags         = MY_FLG_STREAM,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // raw widget (TODO)
   {  .name          = "raw",
      .aliases       = NULL,
      .desc          = "queues a command or event to the vici control socket",
      .vici_cmd      = NULL,
      .vici_event    = NULL,
      .flags         = MY_FLG_STREAM,
      .usage         = "[OPTIONS] [ <key> <value> ] [ <key> <value> ] ... [ <key> <value> ]",
      .short_opt     = MY_SOPT MY_SOPT_COMMAND MY_SOPT_EVENT,
      .long_opt      = MY_LOPTS( MY_LOPT_COMMAND MY_LOPT_EVENT ),
      .arg_min       = 0,
      .arg_max       = -1,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // redirect widget (TODO)
   {  .name          = "redirect",
      .aliases       = NULL,
      .desc          = "redirects client-initiated IKE_SA to another gateway",
      .vici_cmd      = "redirect",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // rekey widget (TODO)
   {  .name          = "rekey",
      .aliases       = NULL,
      .desc          = "initiates rekeying of an SA",
      .vici_cmd      = "rekey",
      .vici_event    = "ike-rekey",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT   MY_SOPT_CHILD MY_SOPT_CHILD_ID MY_SOPT_IKE MY_SOPT_IKE_ID MY_SOPT_REAUTH,
      .long_opt      = MY_LOPTS( MY_LOPT_CHILD MY_LOPT_CHILD_ID MY_LOPT_IKE MY_LOPT_IKE_ID MY_LOPT_REAUTH ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // reload-settings widget (TODO)
   {  .name          = "reload-settings",
      .aliases       = NULL,
      .desc          = "reloads strongswan.conf settings and plugins",
      .vici_cmd      = "reload-settings",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // reset-counters widget (TODO)
   {  .name          = "reset-counters",
      .aliases       = NULL,
      .desc          = "resets global or connection-specific counters",
      .vici_cmd      = "reset-counters",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT MY_SOPT_IKE MY_SOPT_ALL_IKE,
      .long_opt      = MY_LOPTS( MY_LOPT_ALL_IKE MY_LOPT_IKE ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // stats widget (TODO)
   {  .name          = "stats",
      .aliases       = NULL,
      .desc          = "returns IKE daemon statistics",
      .vici_cmd      = "stats",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // terminate widget (TODO)
   {  .name          = "terminate",
      .aliases       = NULL,
      .desc          = "terminates an SA",
      .vici_cmd      = "terminate",
      .vici_event    = "control-log",
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT   MY_SOPT_CHILD MY_SOPT_CHILD_ID MY_SOPT_FORCE MY_SOPT_IKE MY_SOPT_IKE_ID MY_SOPT_TIMEOUT MY_SOPT_LOGLEVEL,
      .long_opt      = MY_LOPTS( MY_LOPT_CHILD MY_LOPT_CHILD_ID MY_LOPT_FORCE MY_LOPT_IKE MY_LOPT_IKE_ID MY_LOPT_TIMEOUT MY_LOPT_LOGLEVEL ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // uninstall widget (TODO)
   {  .name          = "uninstall",
      .aliases       = NULL,
      .desc          = "uninstalls a CHILD_SA's 'trap, drop or bypass policy",
      .vici_cmd      = "uninstall",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT MY_SOPT_CHILD MY_SOPT_IKE,
      .long_opt      = MY_LOPTS( MY_LOPT_CHILD MY_LOPT_IKE ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // unload-authority widget (TODO)
   {  .name          = "unload-authority",
      .aliases       = NULL,
      .desc          = "unloads a certification authority into the daemon",
      .vici_cmd      = "unload-authority",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT   MY_SOPT_NAME,
      .long_opt      = MY_LOPTS( MY_LOPT_NAME ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // unload-conn widget (TODO)
   {  .name          = "unload-conn",
      .aliases       = NULL,
      .desc          = "unloads a connection definition from the daemon",
      .vici_cmd      = "unload-conn",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT   MY_SOPT_NAME,
      .long_opt      = MY_LOPTS( MY_LOPT_NAME ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // unload-key widget (TODO)
   {  .name          = "unload-key",
      .aliases       = NULL,
      .desc          = "unloads a private key from the daemon",
      .vici_cmd      = "unload-key",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // unload-pool widget (TODO)
   {  .name          = "unload-pool",
      .aliases       = NULL,
      .desc          = "unloads a virtual IP and attribute pool.",
      .vici_cmd      = "unload-pool",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = MY_SOPT   MY_SOPT_NAME,
      .long_opt      = MY_LOPTS( MY_LOPT_NAME ),
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // unload-shared widget (TODO)
   {  .name          = "unload-shared",
      .aliases       = NULL,
      .desc          = "unloads a shared IKE PSK, EAP, XAuth or NTLM secret into the daemon",
      .vici_cmd      = "unload-shared",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   // version widget (TODO)
   {  .name          = "version",
      .aliases       = NULL,
      .desc          = "returns daemon and system versions",
      .vici_cmd      = "version",
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = "[OPTIONS]",
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },

   {  .name          = NULL,
      .aliases       = NULL,
      .desc          = NULL,
      .vici_cmd      = NULL,
      .vici_event    = NULL,
      .flags         = 0,
      .usage         = NULL,
      .short_opt     = NULL,
      .long_opt      = NULL,
      .arg_min       = 0,
      .arg_max       = 0,
      .func_exec     = NULL,
      .func_usage    = NULL,
   },
};


/////////////////
//             //
//  Functions  //
//             //
/////////////////
// MARK: - Functions

char *
my_prog_name(
         my_config_t *                 cnf )
{
   static char    buff[512];

   const char * prog_name;

   prog_name   = ((cnf->prog_name))
               ? cnf->prog_name
               : PROGRAM_NAME;

   if ( (!(cnf->widget)) || ((cnf->symlinked)) )
   {  snprintf(buff, sizeof(buff), "%s", prog_name);
      return(buff);
   };

   snprintf(buff, sizeof(buff), "%s %s", prog_name, cnf->widget->name);

   return(buff);
}


void
my_verbose(
         my_config_t *                 cnf,
         const char *                  fmt,
         ... )
{
   va_list     args;
   if (!(cnf->verbose))
      return;
   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);
   return;
}


int
my_version(
         my_config_t *                 cnf )
{
   const char * prog_name;
   prog_name = ((cnf)) ? cnf->prog_name : PROGRAM_NAME;
   printf("%s (%s) %s\n", prog_name, PACKAGE_NAME, PACKAGE_VERSION);
   printf("%s\n", PACKAGE_COPYRIGHT);
   printf("All rights reserved.\n");
   printf("\n");
   return(0);
}


int
main(
         int                           argc,
         char **                       argv )
{
   int                        i;

   for(i = 0; (i < argc); i++)
      printf("arg %i: %s\n", i, argv[i]);

   return(0);
}


/* end of source */
