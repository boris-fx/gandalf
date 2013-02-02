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

#ifndef _GAN_I18N_H
#define _GAN_I18N_H

#include <gandalf/common/misc_defs.h>
#include <stdio.h>

#if defined(_MSC_VER) && defined(UNICODE)
#include <wchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup Common
 * \{
 */

/**
 * \addtogroup CommonI18n
 * \{
 */

/**
 * \brief A unicode string type
 * 
 * Gan_UnicodeChar should be used everywhere platform-dependent
 * unicode Gan_UnicodeChar is required
 * for example, to open a file.
 */
#if defined(_MSC_VER) && defined(UNICODE)
typedef wchar_t Gan_UnicodeChar;
#else
typedef char Gan_UnicodeChar;
#endif

/**
 * \brief Macro: Construct a Gan_UnicodeChar *constant.
 *
 * This function should only be used with string constants.
 *
 * For example:
 *
 * const Gan_UnicodeChar *gstr = GAN_STRING("Hello");
 *
 * Not:
 *
 * const char* cstr = "Hello";
 * const Gan_UnicodeChar *gstr = GAN_STRING(cstr);
 *
 */
#if defined(_MSC_VER) && defined(UNICODE)
#define GAN_STRING(x) L ## x
#else
#define GAN_STRING(x) x
#endif

/**
 * \brief Open a file - a version of fopen that uses Gan_Strings
 *
 * This function behaves exactly like fopen, but takes const Gan_UnicodeChar *arguments
 * 
 * \param filename The file top open
 * \param mode The open mode
 * \returns Pointer to opened FILE, NULL is failure.
 */
GANDALF_API FILE* gan_fopen ( const Gan_UnicodeChar *filename, const Gan_UnicodeChar* mode );

/**
 * \brief Measure the length of a unicode string.
 *
 * \param string The string to measure the length of
 * \returns The length of the string
 */
GANDALF_API size_t gan_strlen ( const Gan_UnicodeChar *string );

/**
 * \brief Convert a string of Gan_UnicodeChar to char*
 *
 * The conversion takes place using the UTF-8 code page.
 * To determine the size of output buffer required, call
 * gan_unicode_to_char with n_chars = 0. The function will return
 * the length of buffer required.
 *
 * \param ustring The unicode string
 * \param n_uchars The length of the unicode string
 * \param string The output buffer for the char string
 * \param n_chars The length of the output buffer
 * \returns The required length of the output buffer, or 0 if an error occurred
 *
 */
GANDALF_API size_t gan_unicodechar_to_char( const Gan_UnicodeChar *ustring,
                                            const size_t n_uchars,
                                            char *string,
                                            const size_t n_chars);

/**
 * \brief Gan_UnicodeChar version of strcpy.
 *
 * \param dest The destination string
 * \param src The source string
 * \returns The argument dest
 *
 */
GANDALF_API Gan_UnicodeChar *gan_strcpy( Gan_UnicodeChar *dest,
                                         const Gan_UnicodeChar *src);

/**
 * \brief Gan_UnicodeChar version of strcat.
 *
 * \param dest The destination string
 * \param src The source string
 * \returns The argument dest
 *
 */
GANDALF_API Gan_UnicodeChar *gan_strcat( Gan_UnicodeChar *dest,
                                         const Gan_UnicodeChar *src);


/**
 * \brief Macro: Gan_UnicodeChar version of sprintf.
 *
 * Note that this function is a macro because you can't easily wrap
 * a function taking variable arguments.
 *
 */
#if defined(_MSC_VER) && defined(UNICODE)
#define gan_sprintf swprintf
#else
#define gan_sprintf sprintf
#endif

 /**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_I18N_H */

