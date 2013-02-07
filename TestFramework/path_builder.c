/**
 * File:          $RCSfile: path_builder.c,v $
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

#include <string.h>
#include <gandalf/TestFramework/path_builder.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/common/i18n.h>

#define BUILD_PATHNAME_BUF_SIZE 1024

static Gan_Char acTempBuffer[BUILD_PATHNAME_BUF_SIZE];

Gan_Char * acBuildPathName(Gan_Char * acBasePath, Gan_Char * acName)
{
   if ( gan_strlen(acBasePath) == 0 || gan_strlen(acName) == 0 )
   {
      gan_err_flush_trace();
      gan_err_register ( "acBuildPathName", GAN_ERROR_NOT_ENOUGH_DATA, "" );
      return NULL;
   }
                         
   if ( acBasePath[gan_strlen(acBasePath)-1] == GAN_STRING('/') ||
        acBasePath[gan_strlen(acBasePath)-1] == GAN_STRING('\\') )
   {
      /* directory contains a trailing backslash separator */
      if ( gan_strlen(acBasePath) + gan_strlen(acName) >= BUILD_PATHNAME_BUF_SIZE )
      {
         gan_err_flush_trace();
         gan_err_register ( "acBuildPathName", GAN_ERROR_ARRAY_TOO_SMALL, "" );
         return NULL;
      }

	   gan_strcpy ( acTempBuffer, acBasePath );
	   gan_strcat ( acTempBuffer, acName );
   }
   else
   {
      if ( gan_strlen(acBasePath) + gan_strlen(acName) + 1 >= BUILD_PATHNAME_BUF_SIZE )
      {
         gan_err_flush_trace();
         gan_err_register ( "acBuildPathName", GAN_ERROR_ARRAY_TOO_SMALL, "" );
         return NULL;
      }

	   gan_strcpy ( acTempBuffer, acBasePath );
	   gan_strcat ( acTempBuffer, GAN_STRING("/") );
	   gan_strcat ( acTempBuffer, acName );
   }

	return acTempBuffer;
}
