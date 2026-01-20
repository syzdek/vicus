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

extern int
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
