/**
 * Module:        Internationalisation (i18n) functions
 * Part of:       Gandalf Library
 *
 * Author:        \$Author $
 * Copyright:     (c) 2008 Imagineer Software Limited
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

#if defined(_MSC_VER) && defined(UNICODE)
#include <windows.h>
#endif

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "i18n.h"

FILE* gan_fopen ( const Gan_Char *filename, const Gan_Char* mode )
{
#if defined(_MSC_VER) && defined(UNICODE)
   return _wfopen(filename, mode);
#else
   return fopen(filename, mode);
#endif
}

size_t gan_strlen ( const Gan_Char *string )
{
#if defined(_MSC_VER) && defined(UNICODE)
   return wcslen(string);
#else
   return strlen(string);
#endif
}

size_t gan_unicodechar_to_char( const Gan_Char *ustring,
                                const size_t n_uchars,
                                char* mbstring,
                                const size_t n_chars )
{
   size_t size;

   if( ! ustring )
      return 0;
#if defined(_MSC_VER) && defined(UNICODE)
   size = WideCharToMultiByte(CP_UTF8, 0, ustring, n_uchars, mbstring, n_chars, NULL, NULL);
   if( *ustring && size == 0 )
      return 0;  /* error */

   if( n_chars == 0 )
      return size + 1;

   if( ! mbstring )
      return 0;

   assert( size <= n_chars );
   if( size == n_chars )
      -- size;
#else
   /* emulate the behaviour of WideCharToMultiByte using memcpy and strlen */
   if( 0 == n_chars )
      return strlen( ustring ) + 1;

   if( n_uchars >= n_chars )
      size = n_chars - 1;
   else // n_uchars < n_chars
      size = n_uchars;

   if( ! mbstring || memcpy( mbstring, ustring, size ) == NULL )
      return 0;
#endif
   mbstring[ size ] = '\0';
   return size;
}

Gan_Char *gan_strcpy( Gan_Char *dest, const Gan_Char *src)
{
#if defined(_MSC_VER) && defined(UNICODE)
   return wcscpy(dest, src);
#else
   return strcpy(dest, src);
#endif
}

Gan_Char *gan_strcat( Gan_Char *dest, const Gan_Char *src )
{
#if defined(_MSC_VER) && defined(UNICODE)
   return wcscat(dest, src);
#else
   return strcat(dest, src);
#endif
}

