/**
 * File:          $RCSfile: camera_ST_map.h,v $
 * Module:        Functions for camera with distortion model defined by ST map
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: $
 * Last edited:   $Date: $
 * Author:        $Author: $
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

#ifndef _GAN_CAMERAF_STMAP_H
#define _GAN_CAMERAF_STMAP_H

#include <stdio.h>
#include <gandalf/vision/cameraf.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup Camera
 * \{
 */

struct Gan_Image;

GANDALF_API Gan_Bool gan_cameraf_build_ST_map ( Gan_Camera_f *camera,
                                                float zh,
                                                float fx, float fy,
                                                float x0, float y0,
                                                struct Gan_Image *stmap_dir,
                                                struct Gan_Image *stmap_inv );

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_CAMERAF_STMAP_H */
