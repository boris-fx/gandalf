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

#include "i18n.h"

FILE* gan_fopen ( const Gan_UnicodeChar *filename, const Gan_UnicodeChar* mode )
{
#if defined(_MSC_VER) && defined(UNICODE)
   return _wfopen(filename, mode);
#else
   return fopen(filename, mode);
#endif
}

size_t gan_strlen ( const Gan_UnicodeChar *string )
{
#if defined(_MSC_VER) && defined(UNICODE)
   return wcslen(string);
#else
   return strlen(string);
#endif
}

size_t gan_unicodechar_to_char( const Gan_UnicodeChar *ustring,
                                const size_t n_uchars,
                                char* mbstring,
                                const size_t n_chars)
{
#if defined(_MSC_VER) && defined(UNICODE)
   return WideCharToMultiByte(CP_UTF8, 0, ustring, n_uchars, mbstring, n_chars, NULL, NULL);
#else
   /* emulate the behaviour of WideCharToMultiByte using memcpy and strlen */
   if (0 == n_chars)
      return strlen(ustring);
   else
      if (NULL == memcpy(mbstring, ustring, n_chars))
         return 0;
      else
         return n_chars;
#endif
}

Gan_UnicodeChar *gan_strcpy( Gan_UnicodeChar *dest,
                             const Gan_UnicodeChar *src)
{
#if defined(_MSC_VER) && defined(UNICODE)
   return wcscpy(dest, src);
#else
   return strcpy(dest, src);
#endif
}

Gan_UnicodeChar *gan_strcat( Gan_UnicodeChar *dest,
                             const Gan_UnicodeChar *src)
{
#if defined(_MSC_VER) && defined(UNICODE)
   return wcscat(dest, src);
#else
   return strcat(dest, src);
#endif
}

