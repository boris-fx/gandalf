/**
 * File:          $RCSfile: path_builder.h,v $
 * Module:        Path builder module
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.2 $
 * Last edited:   $Date: 2002/05/16 08:43:06 $
 * Author:        $Author: pm $
 * Copyright:     (c) 2000 Imagineer Software Limited
 */

/* This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _PATHBUILDER_H
#define _PATHBUILDER_H

#include <stdlib.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/common/i18n.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Helper function to contruct a path name into a buffer defined in this unit*/
Gan_Char * acBuildPathName(Gan_Char * acBasePath, Gan_Char * acName);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _PATHBUILDER_H */
