#
#   Versatile IKE Control User Software for strongSwan
#   Copyright (C) 2026 David M. Syzdek <david@syzdek.net>.
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are
#   met:
#
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the name of David M. Syzdek nor the
#        names of its contributors may be used to endorse or promote products
#        derived from this software without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M SYZDEK BE LIABLE FOR
#   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#   SUCH DAMAGE.
#
#   acinclude.m4 - custom m4 macros used by configure.ac
#

# AC_VICUS_DAVICI_EXAMPLES()
# ______________________________________________________________________________
AC_DEFUN_ONCE([AC_VICUS_DAVICI_EXAMPLES],[dnl
   enableval=""
   AC_ARG_ENABLE(
      davici-examples,
      [AS_HELP_STRING([--enable-davici-examples], [build davici example programs])],
      [ EDAVICIEXAMPLES=$enableval ],
      [ EDAVICIEXAMPLES=$enableval ]
   )

   if test "x${EDAVICIEXAMPLES}" == "xyes";then
      ENABLE_DAVICI_EXAMPLES="yes"
   else
      ENABLE_DAVICI_EXAMPLES="no"
   fi

   AM_CONDITIONAL([ENABLE_DAVICI_EXAMPLES],  [test "$ENABLE_DAVICI_EXAMPLES" = "yes"])
   AM_CONDITIONAL([DISABLE_DAVICI_EXAMPLES], [test "$ENABLE_DAVICI_EXAMPLES" = "no"])
])dnl


# AC_VICUS_DAVICICTL()
# ______________________________________________________________________________
AC_DEFUN_ONCE([AC_VICUS_DAVICICTL],[dnl
   enableval=""
   AC_ARG_ENABLE(
      davicictl,
      [AS_HELP_STRING([--disable-davicictl], [install davicictl])],
      [ EDAVICICTL=$enableval ],
      [ EDAVICICTL=$enableval ]
   )

   if test "x${EDAVICICTL}" == "xyes";then
      ENABLE_DAVICICTL="yes"
   else
      ENABLE_DAVICICTL="no"
   fi

   AM_CONDITIONAL([ENABLE_DAVICICTL],  [test "$ENABLE_DAVICICTL" = "yes"])
   AM_CONDITIONAL([DISABLE_DAVICICTL], [test "$ENABLE_DAVICICTL" = "no"])
])dnl


# AC_VICUS_DAVICI_CHECKS()
# ______________________________________________________________________________
AC_DEFUN([AC_VICUS_DAVICI_CHECKS],[dnl

   # prerequists
   AC_REQUIRE([AC_VICUS_DAVICI_EXAMPLES])
   AC_REQUIRE([AC_VICUS_DAVICICTL])

   _VICUS_DAVICI_CHECK=no
   if test "x${ENABLE_DAVICI_EXAMPLES}" == "xyes"; then
      _VICUS_DAVICI_CHECK=yes
   fi
   if test "x${ENABLE_DAVICICTL}" == "xyes"; then
      _VICUS_DAVICI_CHECK=yes
   fi

   if test "${_VICUS_DAVICI_CHECK}" == "yes"; then
      OLD_LIBS="${LIBS}"
      AC_CHECK_HEADERS([davici.h],                        [], [AC_MSG_ERROR([missing required headers])])
      AC_SEARCH_LIBS([davici_cancel],           [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      AC_SEARCH_LIBS([davici_connect_unix],     [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      AC_SEARCH_LIBS([davici_disconnect],       [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      AC_SEARCH_LIBS([davici_get_name],         [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      AC_SEARCH_LIBS([davici_get_value_str],    [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      AC_SEARCH_LIBS([davici_new_cmd],          [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      AC_SEARCH_LIBS([davici_parse],            [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      AC_SEARCH_LIBS([davici_queue],            [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      AC_SEARCH_LIBS([davici_queue_streamed],   [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      AC_SEARCH_LIBS([davici_read],             [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      AC_SEARCH_LIBS([davici_write],            [davici], [], [AC_MSG_ERROR([missing required function in -ldavici])])
      LIBS="${OLD_LIBS}"
   fi
])dnl


# AC_VICUS_EXAMPLES()
# ______________________________________________________________________________
AC_DEFUN([AC_VICUS_EXAMPLES],[dnl
   enableval=""
   AC_ARG_ENABLE(
      examples,
      [AS_HELP_STRING([--enable-examples], [build example programs])],
      [ EEXAMPLES=$enableval ],
      [ EEXAMPLES=$enableval ]
   )

   if test "x${EEXAMPLES}" == "xyes";then
      ENABLE_EXAMPLES="build"
   else
      ENABLE_EXAMPLES="skip"
   fi

   AM_CONDITIONAL([ENABLE_EXAMPLES],  [test "$ENABLE_EXAMPLES"  = "build"])
   AM_CONDITIONAL([DISABLE_EXAMPLES], [test "$ENABLE_EXAMPLES" != "build"])
])dnl


# AC_VICUS_CYGNUSCTL()
# ______________________________________________________________________________
AC_DEFUN([AC_VICUS_CYGNUSCTL],[dnl
   enableval=""
   AC_ARG_ENABLE(
      cygnusctl,
      [AS_HELP_STRING([--disable-cygnusctl], [do not install cygnusctl])],
      [ ECYGNUSCTL=$enableval ],
      [ ECYGNUSCTL=$enableval ]
   )

   if test "x${ECYGNUSCTL}" == "xno";then
      ENABLE_CYGNUSCTL="skip"
   else
      ENABLE_CYGNUSCTL="install"
   fi

   AM_CONDITIONAL([ENABLE_CYGNUSCTL],  [test "$ENABLE_CYGNUSCTL"  = "install"])
   AM_CONDITIONAL([DISABLE_CYGNUSCTL], [test "$ENABLE_CYGNUSCTL" != "install"])
])dnl


# AC_VICUS_LIBVICUS()
# ______________________________________________________________________________
AC_DEFUN([AC_VICUS_LIBVICUS],[dnl

   # prerequists
   AC_REQUIRE([AC_VICUS_CYGNUSCTL])
   AC_REQUIRE([AC_VICUS_EXAMPLES])

   enableval=""
   AC_ARG_ENABLE(
      libvicus,
      [AS_HELP_STRING([--disable-libvicus], [do not install libvicus])],
      [ ELIBVICUS=$enableval ],
      [ ELIBVICUS=$enableval ]
   )

   if test "x${ELIBVICUS}" == "xno";then
      if test "x${ENABLE_CYGNUSCTL}" == "xinstall"; then
         ENABLE_LIBVICUS="build"
      elif test "x${ENABLE_EXAMPLES}" == "xbuild"; then
         ENABLE_LIBVICUS="build"
      else
         ENABLE_LIBVICUS="skip"
      fi
   else
      ENABLE_LIBVICUS="install"
   fi

   AM_CONDITIONAL([ENABLE_LIBVICUS_LA],  [test "$ENABLE_LIBVICUS"  = "install"])
   AM_CONDITIONAL([DISABLE_LIBVICUS_LA], [test "$ENABLE_LIBVICUS" != "install"])
   AM_CONDITIONAL([ENABLE_LIBVICUS_A],   [test "$ENABLE_LIBVICUS"  = "build"])
   AM_CONDITIONAL([DISABLE_LIBVICUS_A],  [test "$ENABLE_LIBVICUS" != "build"])
])dnl


# AC_VICUS_SOCKET()
# ______________________________________________________________________________
AC_DEFUN([AC_VICUS_SOCKET],[dnl
   withval=""
   AC_ARG_WITH(
      socket,
      [AS_HELP_STRING([--with-socket=type], [socket type to use (bsd or winsock2)])],
      [ WSOCKET=$withval ],
      [ WSOCKET=$withval ]
   )

   if test "x${WSOCKET}" == "xbsd";then
      USE_BSDSOCKETS=yes
      USE_WINSOCK2=no
   elif test "x${WSOCKET}" == "xwinsock2";then
      USE_BSDSOCKETS=no
      USE_WINSOCK2=yes
   else
      USE_BSDSOCKETS=maybe
      USE_WINSOCK2=maybe
   fi

   HAVE_BSDSOCKETS=yes
   if test "x${USE_BSDSOCKETS}" != "xno"; then
      AC_CHECK_HEADERS([arpa/inet.h],              [], [HAVE_BSDSOCKETS=no])
      AC_CHECK_HEADERS([netdb.h],                  [], [HAVE_BSDSOCKETS=no])
      AC_CHECK_HEADERS([sys/socket.h],             [], [HAVE_BSDSOCKETS=no])
      if test "x${HAVE_BSDSOCKETS}" = "xyes"; then
         USE_WINSOCK2=no
      fi
   else
      HAVE_BSDSOCKETS=no
   fi

   HAVE_WINSOCK2=yes
   if test "x${USE_WINSOCK2}" != "xno"; then
      AC_SEARCH_LIBS([WSAStartup],        [ws2_32],   [], [], [-lmswsock])
      AC_CHECK_HEADERS([windows.h],       [], [HAVE_WINSOCK2=no])
      AC_CHECK_HEADERS([winsock2.h],      [], [HAVE_WINSOCK2=no])
      AC_CHECK_HEADERS([ws2tcpip.h],      [], [HAVE_WINSOCK2=no])
      AC_CHECK_FUNCS([WSACleanup],        [], [HAVE_WINSOCK2=no])
      AC_CHECK_FUNCS([WSAStartup],        [], [HAVE_WINSOCK2=no])
      AC_CHECK_FUNCS([closesocket],       [], [HAVE_WINSOCK2=no])
      AC_CHECK_FUNCS([shutdown],          [], [HAVE_WINSOCK2=no])
   else
      HAVE_WINSOCK2=no
   fi

   AC_CHECK_FUNCS([getaddrinfo],    [], [AC_MSG_ERROR([missing required functions])])
   AC_CHECK_FUNCS([recv],           [], [AC_MSG_ERROR([missing required functions])])
   AC_CHECK_FUNCS([send],           [], [AC_MSG_ERROR([missing required functions])])
   AC_CHECK_FUNCS([socket],         [], [AC_MSG_ERROR([missing required functions])])

   USE_SOCKET_TYPE="none"
   if test "x${USE_BSDSOCKETS}" = "xyes"; then
      if test "x${HAVE_BSDSOCKETS}" != "xyes"; then
         AC_MSG_ERROR([missing requirements for BSD sockets])
      fi
      USE_SOCKET_TYPE=bsd
   elif test "x${USE_WINSOCK2}" = "xyes"; then
      if test "x${HAVE_WINSOCK2}" != "xyes"; then
         AC_MSG_ERROR([missing requirements for Winsock2])
      fi
      USE_SOCKET_TYPE=winsock2
   elif test "x${HAVE_BSDSOCKETS}" = "xyes"; then
      USE_SOCKET_TYPE=bsd
   elif test "x${HAVE_WINSOCK2}" = "xyes"; then
      USE_SOCKET_TYPE=winsock2
   else
      AC_MSG_ERROR([unable to determine network socket type])
   fi

   if test "x${USE_SOCKET_TYPE}" = "xwinsock2"; then
      AC_DEFINE_UNQUOTED(VICUS_WITH_WINSOCK2,      1, [Enable Winsock2 Code])
   else
      AC_DEFINE_UNQUOTED(VICUS_WITHOUT_WINSOCK2,   1, [Disable Winsock2 Code])
   fi

   AM_CONDITIONAL([WITH_BSD_SOCKETS],     [test "$USE_SOCKET_TYPE"  = "bsd"])
   AM_CONDITIONAL([WITHOUT_BSD_SOCKETS],  [test "$USE_SOCKET_TYPE" != "bsd"])
   AM_CONDITIONAL([WITH_WINSOCK2],        [test "$USE_SOCKET_TYPE"  = "winsock2"])
   AM_CONDITIONAL([WITHOUT_WINSOCK2],     [test "$USE_SOCKET_TYPE" != "winsock2"])
])dnl


# end of m4 file

