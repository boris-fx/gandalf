# This script instructs qmake how to generate a makefile to build Gandalf.
#
# The following variables are expected to be set:
#  CONFIG - should contain `debug' and/or `release'; `shared' or `static'.
#
# This script presumes that MSVC toolchain is used in Windows.
# The following make-macros are injected into the makefile:
#  EXTRA_CFLAGS, EXTRA_CXXFLAGS, EXTRA_LDFLAGS
#
# In Windows, adjust config_msvc7.h before running qmake.

# \author  \$LastChangedBy$
# \version \$LastChangedRevision$
# \date    \$LastChangedDate$
#
# Copyright (c) 2008-2009 Imagineer Systems. All rights reserved.

# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

!shared: !static: message( CONFIG: $$CONFIG ): error( The CONFIG variable must contain either shared or static )
#shared:  static: message( CONFIG: $$CONFIG ): error( The CONFIG variable must contain either shared or static; not both )

# Include the list of sources and headers
include( gandalf.pri )

TEMPLATE = lib

isEmpty( TARGET ): TARGET = gandalf

CONFIG -= qt

CONFIG( static, static|shared ) {
   DEFINES += GANDALF_STATIC
}

CONFIG( debug, debug|release ) {
   BUILD_TYPE             = debug
   QMAKE_CFLAGS_DEBUG     = $(EXTRA_CFLAGS)    # override
   QMAKE_CXXFLAGS_DEBUG   = $(EXTRA_CXXFLAGS)  # override

   win32: DEFINES += _DEBUG  # used at least in *.rc
} else {
   BUILD_TYPE             = release
   QMAKE_CFLAGS_RELEASE   = $(EXTRA_CFLAGS)    # override
   QMAKE_CXXFLAGS_RELEASE = $(EXTRA_CXXFLAGS)  # override
}
QMAKE_LFLAGS             += $(EXTRA_LDFLAGS)

# Include parent directory, to allow <gandalf/...> includes
INCLUDEPATH += ..

# Include image libraries
LIBS += -ljpeg -ltiff -lpng

macx {
   # Set correct library path for Mac OS bundle
   QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/

   # Add frameworks on the Mac OS
   macx: LIBS += -framework QtGui -framework OpenGL -framework Accelerate
}

win32 {  # should check for MSVC really
   DEFINES += GANDALF_EXPORTS

   system( copy /y config_msvc7.h config.h )
}
