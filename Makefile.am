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
#   @configure_input@
#
#   Makefile.am - automate compiling on a unix platform
#

# Sub directories
SUBDIRS					=
DIST_SUBDIRS				=

# GNU make options
V					= 0

# directory locations
moddir					= $(pkglibexecdir)/modules

# Global flags
AM_LIBS					=
AM_LDFLAGS				=
AM_CFLAGS				= -O2 $(CFLAGS_WARNINGS)
AM_CXXFLAGS				= -O2 @AM_CXXFLAGS@
AM_OBJCFLAGS				= -O2 @AM_OBJCFLAGS@
AM_CPPFLAGS				= -O2 \
					  -DHAVE_CONFIG_H=1 \
					  -I$(top_builddir)/include \
					  -I$(top_srcdir)/include \
					  -DLOCALSTATEDIR="\"$(localstatedir)\"" \
					  -DRUNSTATEDIR="\"$(runstatedir)\"" \
					  -DSYSCONFDIR="\"$(sysconfdir)\""
ACLOCAL_AMFLAGS				= -I m4 -W all -W error
AM_MAKEINFOFLAGS                        = --no-split
DEFS					=


# custom directories


# automake targets
check_PROGRAMS				=
doc_DATA				=
html_DATA				=
include_HEADERS				=
lib_LTLIBRARIES				=
lib_LIBRARIES				=
man_MANS				=
info_TEXINFOS				=
noinst_LTLIBRARIES			=
noinst_LIBRARIES			=
noinst_HEADERS				= include/vicus.h
noinst_PROGRAMS				=
bin_PROGRAMS				=
bin_SCRIPTS				=
pkgdata_DATA				=
sbin_SCRIPTS				=
sbin_PROGRAMS				=
mod_LTLIBRARIES				=
EXTRA_PROGRAMS				= examples/davici-command \
					  examples/davici-event \
					  examples/davici-multiple \
					  examples/davici-stream \
					  src/davici/davicictl
EXTRA					=


# lists
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT	=
BUILT_SOURCES				=
TESTS					=
XFAIL_TESTS				=
EXTRA_MANS				=
EXTRA_DIST				= AUTHORS.md \
					  ChangeLog.md \
					  COPYING.md \
					  NEWS.md \
					  README.md \
					  TODO.md \
					  lib/libvicus/libvicus.sym
CLEANFILES				= $(builddir)/a.out   $(srcdir)/a.out \
					  $(builddir)/*/a.out $(srcdir)/*/a.out \
					  config.h.in~ $(srcdir)/config.h.in~ \
					  $(EXTRA_PROGRAMS) \
					  @PACKAGE_TARNAME@-*.tar.* \
					  @PACKAGE_TARNAME@-*.txz \
					  @PACKAGE_TARNAME@-*.zip
DISTCHECK_CONFIGURE_FLAGS		= --enable-strictwarnings \
					  --enable-examples \
					  --enable-davicictl \
					  --enable-davici-examples \
					  CFLAGS="$(CFLAGS)" \
					  CPPFLAGS="$(CPPFLAGS)" \
					  LDFLAGS="$(LDFLAGS)"


# conditional build
if ENABLE_LIBVICUS_A
   noinst_LIBRARIES 			+= lib/libvicus.a
endif
if ENABLE_LIBVICUS_LA
   include_HEADERS 			+= include/vicus.h
   lib_LTLIBRARIES 			+= lib/libvicus.la
endif
if ENABLE_DAVICICTL
   sbin_PROGRAMS			+= src/davici/davicictl
endif
if ENABLE_DAVICI_EXAMPLES
   noinst_PROGRAMS			+= examples/davici-command
   noinst_PROGRAMS			+= examples/davici-event
   noinst_PROGRAMS			+= examples/davici-multiple
   noinst_PROGRAMS			+= examples/davici-stream
endif


# macros for examples/davici-command
examples_davici_command_DEPENDENCIES	= Makefile config.h
examples_davici_command_CPPFLAGS	= -DPROGRAM_NAME="\"davici-command\"" $(AM_CPPFLAGS)
examples_davici_command_LDADD		= $(AM_LDADD) -ldavici
examples_davici_command_SOURCES		= examples/davici-command.c


# macros for examples/davici-event
examples_davici_event_DEPENDENCIES	= Makefile config.h
examples_davici_event_CPPFLAGS		= -DPROGRAM_NAME="\"davici-event\"" $(AM_CPPFLAGS)
examples_davici_event_LDADD		= $(AM_LDADD) -ldavici
examples_davici_event_SOURCES		= examples/davici-event.c


# macros for examples/davici-multiple
examples_davici_multiple_DEPENDENCIES	= Makefile config.h
examples_davici_multiple_CPPFLAGS	= -DPROGRAM_NAME="\"davici-multiple\"" $(AM_CPPFLAGS)
examples_davici_multiple_LDADD		= $(AM_LDADD) -ldavici
examples_davici_multiple_SOURCES	= examples/davici-multiple.c


# macros for examples/davici-stream
examples_davici_stream_DEPENDENCIES	= Makefile config.h
examples_davici_stream_CPPFLAGS		= -DPROGRAM_NAME="\"davici-stream\"" $(AM_CPPFLAGS)
examples_davici_stream_LDADD		= $(AM_LDADD) -ldavici
examples_davici_stream_SOURCES		= examples/davici-stream.c


# macros for lib/libvicus.a
lib_libvicus_a_DEPENDENCIES		= Makefile config.h
lib_libvicus_a_CPPFLAGS			= $(AM_CPPFLAGS) -I$(srcdir)/lib/libvicus
lib_libvicus_a_SOURCES			= include/vicus.h \
					  lib/libvicus/libvicus.h \
					  lib/libvicus/lcore.c


# macros for lib/libvicus.la
lib_libvicus_la_DEPENDENCIES		= Makefile config.h lib/libvicus/libvicus.sym
lib_libvicus_la_CPPFLAGS		= $(AM_CPPFLAGS) -I$(srcdir)/lib/libvicus
lib_libvicus_la_LDFLAGS			= -rpath'$(libdir)' \
					  -version-info $(LIB_VERSION_INFO) \
					  $(AM_LDFLAGS) \
					  -export-symbols $(srcdir)/lib/libvicus/libvicus.sym
lib_libvicus_la_SOURCES			= $(lib_libvicus_a_SOURCES)


# macros for src/davici/davicictl
src_davici_davicictl_DEPENDENCIES	= Makefile config.h
src_davici_davicictl_CPPFLAGS		= -DPROGRAM_NAME="\"davicictl\"" $(AM_CPPFLAGS)
src_davici_davicictl_LDADD		= $(AM_LDADD) -ldavici
src_davici_davicictl_SOURCES		= src/davici/davicictl.h \
					  src/davici/davicictl.c \
					  src/davici/davicictl-misc.c \
					  src/davici/davicictl-parser.c \
					  src/davici/widget-counters.c \
					  src/davici/widget-diagnostics.c \
					  src/davici/widget-raw.c \
					  src/davici/widget-rekey.c


# Makefile includes
GIT_PACKAGE_VERSION_DIR=include
SUBST_EXPRESSIONS =
include contrib/bindletools/build-aux/makefile-subst.am
include contrib/bindletools/build-aux/makefile-version.am


# custom targets
.PHONY: git-clean

git-clean:
	git fsck --full --unreachable
	git gc --auto --aggressive


# local targets
install-exec-local:

install-data-local:

install-data-hook:

uninstall-local:

clean-local:

distclean-local:
	rm -fR $(srcdir)/autom4te.cache

# end of automake file
