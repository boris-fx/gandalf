/**
 * File:          $RCSfile: camera_equirectangular.h,v $
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
#ifndef _GAN_CAMERA_EQUIRECTANGULAR_H
#define _GAN_CAMERA_EQUIRECTANGULAR_H

#include <stdio.h>
#include <gandalf/vision/camera.h>

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

GANDALF_API Gan_Bool gan_camera_build_equirectangular(
   Gan_Camera *camera,
   double zh,
   double fx, double fy,
   double x0, double y0,
   double longitude, double latitude,
   double FoV );

GANDALF_API Gan_Camera gan_camera_equirect_dual( const Gan_Camera * camera );


GANDALF_API void gan_camera_equirect_direction_p(double longitude, double latitude, Gan_Vector3 * dir);

// Less efficient version of gan_camera_equirect_direction_p
GANDALF_API Gan_Vector3 gan_camera_equirect_direction( double longitude, double latitude );

GANDALF_API Gan_Bool gan_camera_equirect_point_behind( const Gan_Camera * camera,
   double x, double y );

GANDALF_API void gan_camera_equirect_xy2latlong( double x, double y,
   double width, double height,
   double * longitude, double * latitude, const Gan_Camera * );

GANDALF_API void gan_camera_equirect_latlong2xy( double longitude, double latitude,
   double width, double height,
   double * x, double * y, const Gan_Camera * );

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GAN_CAMERA_EQUIRECTANGULAR_H */
