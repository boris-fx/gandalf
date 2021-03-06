/**
 * File:          $RCSfile: image_colour_noc.h,v $
 * Module:        Colour image macros & definitions
 * Part of:       Gandalf Library 
 *
 * Revision:      $Revision: 1.25 $
 * Last edited:   $Date: 2005/08/22 08:52:19 $
 * Author:        $Author: jps $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Not to be complled separately
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

#include <stdlib.h>
#include <gandalf/common/misc_defs.h>
#include <gandalf/image/image_defs.h>

/**
 * \addtogroup ImagePackage
 * \{
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GAN_IMAGE_STRUCT_DEFINED
struct Gan_Image;
GAN_PIXEL;
#endif

/**
 * \addtogroup ImageSet
 * \{
 */

/**
 * \brief Not a user function
 */
GANDALF_API struct Gan_Image *GAN_IMAGE_SET_GEN(
   Gan_Image *img,
   unsigned long height, unsigned long width, unsigned long stride,
   Gan_Bool alloc_pix_data );

/**
 * \}
 */

/**
 * \addtogroup ImageAllocate
 * \{
 */

/**
 * \brief Not a user function
 */
GANDALF_API struct Gan_Image *GAN_IMAGE_FORM_GEN(
   Gan_Image *img,
   unsigned long height, unsigned long width, unsigned long stride,
   Gan_Bool alloc_pix_data,
   GAN_PIXEL  *pix_data, size_t pix_data_size,
   GAN_PIXEL **row_data, size_t row_data_size );
/**
 * \}
 */

/**
 * \addtogroup ImageExtract
 * \{
 */

/**
 * \brief Computes bounding box of non-zero pixels in grey-level image.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Computes image window which covers all pixels in given grey-level \a image
 * which are non-zero. If \a image is zero everywhere the fields of the
 * \a subwin structure will be set to zero.
 *
 */
GANDALF_API Gan_Bool GAN_IMAGE_GET_ACTIVE_SUBWINDOW ( const Gan_Image *image,
                                          Gan_ImageWindow *subwin );
/**
 * \}
 */

#ifndef NDEBUG
/**
 * \addtogroup ImageAccessPixel
 * \{
 */

/**
 * \brief Return a pointer to a pixel from a GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image.
 *
 * Returns the pointer to the pixel at position \a row, \a col in
 * GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image \a img.
 */
GANDALF_API GAN_PIXEL *GAN_IMAGE_GET_PIXPTR ( const Gan_Image *img,
                                  unsigned row, unsigned col );

/**
 * \brief Return the pixel array from a GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image.
 *
 * Returns the pixel array for the GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image
 * \a img.
 */
GANDALF_API GAN_PIXEL **GAN_IMAGE_GET_PIXARR ( const Gan_Image *img );

/**
 * \}
 */
#endif /* #ifndef NDEBUG */

/**
 * \addtogroup ImageFill
 * \{
 */

/**
 * \brief Clear binary image except in specified rectangular region.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
GANDALF_API Gan_Bool GAN_IMAGE_MASK_WINDOW ( Gan_Image *pImage,
                                 unsigned r0,     unsigned c0,
                                 unsigned height, unsigned width );

/**
 * \brief Clear binary image in specified rectangular region.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 */
GANDALF_API Gan_Bool GAN_IMAGE_CLEAR_WINDOW ( Gan_Image *pImage,
                                  unsigned r0,     unsigned c0,
                                  unsigned height, unsigned width );
/**
 * \}
 */

#if defined(GAN_IMAGE_GET_MINIMUM_PIXEL)
/**
 * \addtogroup ImageMinMax
 * \{
 */
GANDALF_API Gan_Bool GAN_IMAGE_GET_MINIMUM_PIXEL ( const Gan_Image *pImage,
                                       const Gan_Image *pMask,
                                       GAN_PIXEL_TYPE *minval );
GANDALF_API Gan_Bool GAN_IMAGE_GET_MAXIMUM_PIXEL ( const Gan_Image *pImage,
                                       const Gan_Image *pMask,
                                       GAN_PIXEL_TYPE *maxval );
/**
 * \}
 */
#endif /* defined(GAN_IMAGE_GET_MINIMUM_PIXEL) */

/* declarations of macros defined in individual header files */

#ifdef GAN_GENERATE_DOCUMENTATION

/**
 * \addtogroup ImageAllocate
 * \{
 */

/**
 * \fn Gan_Image *GAN_IMAGE_ALLOC ( unsigned long height, unsigned long width )
 * \brief Macro: Allocate a new GAN_PIXEL_FORMAT image with type \c GAN_PIXEL_TYPE.
 * \return The new image, or \c NULL on failure.
 *
 * Allocate and return a new GAN_PIXEL_FORMAT image of type \c GAN_PIXEL_TYPE
 * and given dimensions \a height and \a width.
 *
 * Implemented as a macro.
 */
GANDALF_API Gan_Image *GAN_IMAGE_ALLOC ( unsigned long height, unsigned long width );

/**
 * \fn Gan_Image *GAN_IMAGE_FORM ( Gan_Image *img, unsigned long height, unsigned long width )
 * \brief Macro: Forms and returns a new GAN_PIXEL_FORMAT image with type \c GAN_PIXEL_TYPE.
 * \return The formed image \a img, or \c NULL on failure.
 *
 * Forms and returns a new GAN_PIXEL_FORMAT image of type \c GAN_PIXEL_TYPE and
 * given dimensions \a height and \a width. If the \a img image pointer is
 * not \c NULL, the given image pointer is formed; otherwise \a img is passed
 * as \c NULL and the image is dynamically allocated.
 *
 * Implemented as a macro.
 */
GANDALF_API Gan_Image *GAN_IMAGE_FORM ( Gan_Image *img,
                            unsigned long height, unsigned long width );

/**
 * \brief Macro: Allocates and returns a new GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image.
 * \param height Height of image
 * \param width Width of image
 * \param stride Stride of image
 * \param pix_data Pixel data as a one-dimensional array of pixels
 * \param pix_data_size Number of pixels in pix_data array
 * \param row_data Pointers to be set to the start of each row
 * \param row_data_size Size of row_data array, at least image height
 * \return Pointer to formed image, or \c NULL on failure.
 *
 * Forms and returns a new #GAN_IMAGE_FORMAT image of type #GAN_IMAGE_TYPE and
 * given dimensions \a height, \a width and given \a stride
 * \f$ (stride \geq width) \f$, using the provided pixel data and row data
 * array.
 */
GANDALF_API Gan_Image *GAN_IMAGE_ALLOC_DATA ( unsigned long height,
                                              unsigned long width,
                                              unsigned long stride,
                                              GAN_PIXEL  *pix_data, size_t pix_data_size,
                                              GAN_PIXEL **row_data, size_t row_data_size );

/**
 * \brief Macro: Forms and returns a new GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image.
 * \param img Address of current image to be modified.
 * \param height Height of image
 * \param width Width of image
 * \param stride Stride of image
 * \param pix_data Pixel data as a one-dimensional array of pixels
 * \param pix_data_size Number of pixels in pix_data array
 * \param row_data Pointers to be set to the start of each row
 * \param row_data_size Size of row_data array, at least image height
 * \return Pointer to formed image, or \c NULL on failure.
 *
 * Forms and returns a new #GAN_IMAGE_FORMAT image of type #GAN_IMAGE_TYPE and
 * given dimensions \a height, \a width and given \a stride
 * \f$ (stride \geq width) \f$, using the provided pixel data and row data
 * array.
 */
GANDALF_API Gan_Image *GAN_IMAGE_FORM_DATA ( Gan_Image *img,
                                 unsigned long height,
                                 unsigned long width,
                                 unsigned long stride,
                                 GAN_PIXEL  *pix_data, size_t pix_data_size,
                                 GAN_PIXEL **row_data, size_t row_data_size );

/**
 * \}
 */

/**
 * \addtogroup ImageSet
 * \{
 */

/**
 * \brief Macro: Set an existing image to be GAN_PIXEL_FORMAT with type \c GAN_PIXEL_TYPE.
 * \return The result image \a img.
 *
 * Set up an existing image \a img as GAN_PIXEL_FORMAT format with type
 * \c GAN_PIXEL_TYPE and dimensions \a height, \a width and given \a stride
 * \f$ (stride \geq width) \f$.
 *
 * Implemented as a macro.
 */
GANDALF_API Gan_Image *GAN_IMAGE_SET ( Gan_Image *img,
                           unsigned long height, unsigned long width );

/**
 * \}
 */

/**
 * \addtogroup ImageAccessPixel
 * \{
 */

/**
 * \brief Macro: Set a pixel in a GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Set the pixel at position \a row, \a col in GAN_PIXEL_FORMAT
 * \c GAN_PIXEL_TYPE image \a img to \a pix.
 */
GANDALF_API Gan_Bool GAN_IMAGE_SET_PIX ( Gan_Image *img, unsigned row, unsigned col,
                             GAN_PIXEL *pix );

/**
 * \brief Macro: Return a pixel from a GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image.
 *
 * Returns the value of the pixel at position \a row, \a col in
 * GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image \a img.
 */
GANDALF_API GAN_PIXEL GAN_IMAGE_GET_PIX ( const Gan_Image *img, unsigned row, unsigned col );

/**
 * \}
 */

/**
 * \addtogroup ImageFill
 * \{
 */

/**
 * \brief Macro: Fills all the pixels in a GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Fills all the pixels in GAN_PIXEL_FORMAT \c GAN_PIXEL_TYPE image \a img
 * to the value \a pix.
 */
GANDALF_API Gan_Bool GAN_IMAGE_FILL_CONST ( Gan_Image *img, GAN_PIXEL *pix );

/**
 * \}
 */

#endif /* #ifdef GAN_GENERATE_DOCUMENTATION */

#ifdef __cplusplus
}
#endif

#undef GAN_PIXEL
#undef GAN_PIXEL_FORMAT
#undef GAN_PIXEL_TYPE
#undef GAN_IMTYPE
#undef GAN_IMAGE_FORM_GEN
#undef GAN_IMAGE_SET_GEN
#undef GAN_IMAGE_ALLOC
#undef GAN_IMAGE_ALLOC_DATA
#undef GAN_IMAGE_FORM
#undef GAN_IMAGE_FORM_DATA
#undef GAN_IMAGE_SET
#undef GAN_IMAGE_SET_PIX
#undef GAN_IMAGE_GET_PIX
#undef GAN_IMAGE_GET_PIXPTR
#undef GAN_IMAGE_GET_PIXARR
#undef GAN_IMAGE_FILL_CONST
#undef GAN_IMAGE_GET_ACTIVE_SUBWINDOW
#undef GAN_IMAGE_MASK_WINDOW
#undef GAN_IMAGE_CLEAR_WINDOW
#undef GAN_IMAGE_GET_MINIMUM_PIXEL
#undef GAN_IMAGE_GET_MAXIMUM_PIXEL
#undef GAN_IMAGE_PIXEL_MIN_VAL
#undef GAN_IMAGE_PIXEL_MAX_VAL
#undef GAN_IMAGE_PIXEL_ZERO_VAL

/**
 * \}
 */
