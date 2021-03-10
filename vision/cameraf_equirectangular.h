/**
 * File:          $RCSfile: cameraf_equirectangular.h,v $
 * Module:        Functions for equirectangular camera
 * Part of:       Gandalf Library

 * Copyright:     (c) 2015 Imagineer Software Limited
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
#ifndef _GAN_CAMERAF_EQUIRECTANGULAR_H
#define _GAN_CAMERAF_EQUIRECTANGULAR_H

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

GANDALF_API Gan_Bool gan_cameraf_build_equirectangular(
   Gan_Camera_f *camera,
   float zh,
   float fx, float fy,
   float x0, float y0,
   float longitude, float latitude,
   float FoV );

GANDALF_API Gan_Camera_f gan_cameraf_equirect_dual( const Gan_Camera_f * camera );


GANDALF_API void gan_cameraf_equirect_direction_p(double longitude, double latitude, Gan_Vector3_f * dir);

// Less efficient version of gan_camera_equirect_direction_p
GANDALF_API Gan_Vector3_f gan_cameraf_equirect_direction( float longitude, float latitude );

GANDALF_API Gan_Bool gan_cameraf_equirect_point_behind( const Gan_Camera_f * camera,
   float x, float y );

GANDALF_API void gan_cameraf_equirect_xy2latlong( float x, float y,
   float width, float height,
   float * longitude, float * latitude, const Gan_Camera_f * );

GANDALF_API void gan_cameraf_equirect_latlong2xy( float longitude, float latitude,
   float width, float height,
   float * x, float * y, const Gan_Camera_f * );

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_CAMERAF_EQUIRECTANGULAR_H */
