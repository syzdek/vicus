
Versatile IKE Control User Software for strongSwan
==================================================


Copyright
=========

Versatile IKE Control User Software for strongSwan
Copyright (C) 2026 David M. Syzdek <david@syzdek.net>.  
All rights reserved.  

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of David M. Syzdek nor the
     names of its contributors may be used to endorse or promote products
     derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID M. SYZDEK BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.


Overview
==========

Versatile IKE Control User Software for strongSwan provides the vicus library
and the cygnusctl tool.

The vicus library is an alternative library to the [davici](https://github.com/strongswan/davici)
library for accessing [Versatile IKE Control Interface (VICI) protocol](https://github.com/strongswan/strongswan/blob/master/src/libcharon/plugins/vici/README.md)
sockets.  Key design differences between the vicus library and the davici
library include:

   * improving code portablity
   * adding thread safe
   * supporting for TCP/IP
   * reducing use of call back functions

The cygnusctl tool is an alterative to the [swanctl tool](https://docs.strongswan.org/docs/latest/swanctl/swanctl.html)
for some IKE adminsitrative tasks. Key design differences between the
cygnusctl tool and the swanctl tool include:

   * adding support for formatted output (JSON, YAML, and XML)
   * adding ability to call all vici command and register all vici events
 
_Vicus_ is the Latin term for village or neighbourhood. The Latin plural of
vicus is _vici_. Since the charon daemon's vici socket could potentially have
muliple sockets opened concurrently by clients, it seemd appropiate that the
library, which only supports a single connection concurrently, should be named
for the singlular form. _Cygnus_ is the Latin term for swan.


Maintainers
===========

* David M. Syzdek <david@syzdek.net>


Source Code
===========

The source code for this project is maintained using git (http://git-scm.com).
The following contains information to checkout the source code from the git
repository.

Browse Source:

   * https://github.com/syzdek/vicus.git

Git URLs:

   * https://github.com/syzdek/vicus.git

Preparing Source:

    $ git clone https://github.com/syzdek/vicus.git
    $ cd vicus
    $ ./autogen.sh


Package Maintenance Notes
=========================

This is a collection of notes for developers to use when maintaining this
package.

New Release Checklist:

   * Switch to 'master' branch in Git repository.
   * Update version in configure.ac.
   * Update date and version in ChangeLog.
   * Commit configure.ac and ChangeLog changes to repository.
   * Create tag in git repository:
   
         $ git tag -s v${MAJOR}.${MINOR}
       
   * Push repository to publishing server:
   
         $ git push --tags origin master:master next:next pu:pu

 Creating Source Distribution Archives:

    $ ./configure
    $ make update
    $ make distcheck
    $ make dist-xz
    $ make dist-mingw32

