/**
 * File:          $RCSfile: camera_ST_map.c,v $
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

#include <gandalf/vision/cameraf_ST_map.h>
#include <gandalf/image/image_defs.h>
#include <gandalf/image/image_rgb_float.h>
#include <gandalf/image/image_rgb_double.h>
#include <gandalf/image/image_rgb_ushort.h>
#include <gandalf/image/image_rgba_float.h>
#include <gandalf/image/image_rgba_double.h>
#include <gandalf/image/image_rgba_ushort.h>
#include <gandalf/image/image_bgr_float.h>
#include <gandalf/image/image_bgr_double.h>
#include <gandalf/image/image_bgr_ushort.h>
#include <gandalf/image/image_bgra_float.h>
#include <gandalf/image/image_bgra_double.h>
#include <gandalf/image/image_bgra_ushort.h>
#include <gandalf/common/numerics.h>

/**
 * \addtogroup Vision
 * \{
 */

/**
 * \addtogroup Camera
 * \{
 */

#define CZH camera->zh
#define CFX camera->fx
#define CFY camera->fy
#define CX0 camera->x0
#define CY0 camera->y0

/////////////////////////////////////////////////////////////////////
// Functions for linear interpolation

static void build_linear_interpolation_neighbours( const Gan_Vector2_f *x,
                                       int neib_x[], int neib_y[] )
{
   // Build neighbour integer pixels
   // Take into account that ST map value is defined for the centre of the pixel
   neib_x[0] = (int)floor(x->x - 0.5f);
   neib_x[1] = neib_x[0] + 1;
   neib_y[0] = (int)floor(x->y - 0.5f);
   neib_y[1] = neib_y[0] + 1;

}

/// Build weights for linear interpolation of ST map
/**
\param x - point at which to perform interpolation
\param a - interpolation coefficients for x
\param b - interpolation coefficients for y
*/
static void build_linear_interpolation_weights( const Gan_Vector2_f *x,
                                                const int neib_x[], const int neib_y[],
                                                float a[], float b[] )
{
   a[0] = neib_x[1] - x->x + 0.5f;
   a[1] = 1.F - a[0];
   b[0] = neib_y[1] - x->y + 0.5f;
   b[1] = 1.F - b[0];
}

/// Build weights for linear interpolation of derivatives of ST map
/**
\param x - point at which to perform interpolation
\param a - interpolation coefficients for x
\param b - interpolation coefficients for y
*/
static void build_linear_interpolation_weightsJ( const Gan_Vector2_f *x,
                                                const int neib_x[], const int neib_y[],
                                                float ax[], float bx[],
                                                float ay[], float by[] )
{
   float a[2], b[2];

   build_linear_interpolation_weights( x, neib_x, neib_y, a, b );

   ax[0] = -1.0f;
   ax[1] = 1.0f;
   bx[0] = b[0];
   bx[1] = b[1];

   ay[0] = a[0];
   ay[1] = a[1];
   by[0] = -1.0f;
   by[1] = 1.0f;
}

/**
 \param st_map - ST map. May have different size than the original image.
 \param neib_x - x-coordinates of neighbour pixels
 \param neib_y - y-coordinates of neighbour pixels
 \param neib   - ST map values in neighbour pixels
 \param width  - width  of the original image
 \param height - height of the original image
 */
static Gan_Bool build_linear_interpolation_values( const struct Gan_Image * st_map,
                                                   const int neib_x[],
                                                   const int neib_y[],
                                                   Gan_Vector2_f neib[][2],
                                                   float width, float height )
{
   unsigned int k,l;
   Gan_Bool result = GAN_TRUE;

   // Get values of ST map in neighbour integer pixels
   for ( k = 0; k < 2; ++k )
   {
      for ( l = 0; l < 2; ++l )
      {
         if ( ( neib_y[k] >= 0 ) && ( neib_y[k] < (int)st_map->height ) &&
              ( neib_x[l] >= 0 ) && ( neib_x[l] < (int)st_map->width  ) )
         {
            switch( st_map->type )
            {
            case GAN_FLOAT:
               {
                  switch ( st_map->format )
                  {
                  case GAN_RGB_COLOUR_IMAGE:
                     {
                        Gan_RGBPixel_f px;
                        px = gan_image_get_pix_rgb_f( st_map, neib_y[k], neib_x[l] );
                        neib[k][l].x = width * px.R;
                        neib[k][l].y = height * px.G;
                     }
                     break;
                  case GAN_RGB_COLOUR_ALPHA_IMAGE:
                     {
                        Gan_RGBAPixel_f px;
                        px = gan_image_get_pix_rgba_f( st_map, neib_y[k], neib_x[l] );
                        neib[k][l].x = width * px.R;
                        neib[k][l].y = height * px.G;
                     }
                     break;
                  case GAN_BGR_COLOUR_IMAGE:
                     {
                        Gan_BGRPixel_f px;
                        px = gan_image_get_pix_bgr_f( st_map, neib_y[k], neib_x[l] );
                        neib[k][l].x = width * px.R;
                        neib[k][l].y = height * px.G;
                     }
                     break;
                  case GAN_BGR_COLOUR_ALPHA_IMAGE:
                     {
                        Gan_BGRAPixel_f px;
                        px = gan_image_get_pix_bgra_f( st_map, neib_y[k], neib_x[l] );
                        neib[k][l].x = width * px.R;
                        neib[k][l].y = height * px.G;
                     }
                     break;
                  default:
                     result = GAN_FALSE;
                  }
                  break;
               }
            case GAN_DOUBLE:
               switch ( st_map->format )
               {
               case GAN_RGB_COLOUR_IMAGE:
                  {
                     Gan_RGBPixel_d px;
                     px = gan_image_get_pix_rgb_d( st_map, neib_y[k], neib_x[l] );
                     neib[k][l].x = width  * (float)px.R;
                     neib[k][l].y = height * (float)px.G;
                     break;
                  }
               default:
                  result = GAN_FALSE;
               }
               break;
            case GAN_USHORT:
              switch ( st_map->format )
               {
               case GAN_RGB_COLOUR_IMAGE:
                  {
                     Gan_RGBPixel_us px;
                     px = gan_image_get_pix_rgb_us( st_map, neib_y[k], neib_x[l] );
                     neib[k][l].x = width * px.R / 65535;
                     neib[k][l].y = height * px.G / 65535;
                     break;
                  }
               default:
                  result = GAN_FALSE;
               }
               break;
            default:
               result = GAN_FALSE;
            }
         }
         else {
            result = GAN_FALSE;
         }
      }
   }

   if ( result == GAN_FALSE )
   {
      for ( k = 0; k < 2; ++k )
      {
         for ( l = 0; l < 2; ++l )
         {
            neib[k][l].x = 0;
            neib[k][l].y = 0;
         }
      }
   }

   return result;
}

static Gan_Vector2_f interpolate_linear( const float a[],
                                       const float b[],
                                       const Gan_Vector2_f neib[][2] )
{
   Gan_Vector2_f v;
   unsigned int k,l;

   // Do linear interpolation
   (void)gan_vec2f_zero_q( &v );
   for ( k = 0; k < 2; ++k )
   {
      for ( l = 0; l < 2; ++l )
      {
         v.x += a[l]*b[k]*neib[k][l].x;
         v.y += a[l]*b[k]*neib[k][l].y;
      }
   }

   return v;
}

/////////////////////////////////////////////////////////////////////
// Utility functions to use in camera interface methods

static Gan_Bool
 point_distort ( const Gan_Camera_f *camera,
                        Gan_Vector3_f *pu, Gan_Vector3_f *p, Gan_PositionState_f *pupprev,
                        int *error_code, Gan_Bool undistort )
{
   Gan_Vector2_f x;
   // Use linear interpolation to get position for pixels with non-integer coordinates
   float a[2], b[2]; // Interpolation coefficients for x and y respectively
   int neib_y[2], neib_x[2]; // Integer neighbours of point
   Gan_Vector2_f neib[2][2];
   Gan_Vector2_f centreST; // Centre of ST map

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_STMAP_CAMERA,
                       "point_distort", GAN_ERROR_INCOMPATIBLE, "" );

#ifndef NDEBUG
   if ( camera->fx == 0.0f || camera->fy == 0.0f || camera->zh == 0.0f )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_distort",
                            GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }
#endif /* #ifndef NDEBUG */

   if ( pu->z == 0.0f )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_distort",
                            GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   /* compute image coordinates */
   x = gan_vec2f_fill_s ( camera->zh*pu->x/pu->z, camera->zh*pu->y/pu->z );

   if ( !( camera->nonlinear.stmap.stmap_inv && camera->nonlinear.stmap.stmap_dir ) &&
         undistort == GAN_TRUE )
   {
      /* no distortion if no st map */
      gan_vec3f_set_parts_q ( p, &x, camera->zh );
      return GAN_TRUE;
   }

   if ( !( camera->nonlinear.stmap.stmap_dir && camera->nonlinear.stmap.stmap_inv ) &&
        undistort == GAN_FALSE )
   {
      /* no distortion if no st map */
      gan_vec3f_set_parts_q ( p, &x, camera->zh );
      return GAN_TRUE;
   }

   /* map image point to a point on ST map */
   if ( undistort == GAN_TRUE )
   {
      centreST.x = 0.5f * camera->nonlinear.stmap.stmap_inv->width;
      centreST.y = 0.5f * camera->nonlinear.stmap.stmap_inv->height;
   }
   else
   {
      centreST.x = 0.5f * camera->nonlinear.stmap.stmap_dir->width;
      centreST.y = 0.5f * camera->nonlinear.stmap.stmap_dir->height;
   }

   x.x += centreST.x - CX0;
   x.y += centreST.y - CY0;

   build_linear_interpolation_neighbours( &x, neib_x, neib_y );

   build_linear_interpolation_weights( &x, neib_x, neib_y, a, b );

   /* emulate linear camera if a point is out of ST map */
   if ( build_linear_interpolation_values(
         undistort == GAN_TRUE ? camera->nonlinear.stmap.stmap_inv
                               : camera->nonlinear.stmap.stmap_dir,
         neib_x, neib_y, neib, 2.0f * CX0, 2.0f * CY0 ) == GAN_TRUE )
   {
      x = interpolate_linear( a, b, neib );
   }
   else
   {
      x = gan_vec2f_fill_s ( camera->zh*pu->x/pu->z, camera->zh*pu->y/pu->z );
   }

   /* fill image point with distortion added */
   gan_vec3f_set_parts_q ( p, &x, camera->zh );

   /* success */
   return GAN_TRUE;
}

/* function to add distortion to a point */
static Gan_Bool
 point_add_distortion ( const Gan_Camera_f *camera,
                        Gan_Vector3_f *pu, Gan_Vector3_f *p, Gan_PositionState_f *pupprev,
                        int *error_code )
{
   return point_distort( camera, pu, p, pupprev, error_code, GAN_FALSE );
}

/* function to remove distortion from a point */
static Gan_Bool
 point_remove_distortion ( const Gan_Camera_f *camera,
                           Gan_Vector3_f *p, Gan_Vector3_f *pu, Gan_PositionState_f *ppuprev,
                           int *error_code)
{
   return point_distort( camera, p, pu, ppuprev, error_code, GAN_TRUE );
}

/////////////////////////////////////////////////////////////////////
//
/* point projection function */
static Gan_Bool
 point_project ( const Gan_Camera_f *camera,
                 Gan_Vector3_f *X, Gan_Vector3_f *p, Gan_PositionState_f *Xpprev,
                 Gan_Matrix22_f *HX, Gan_Camera_f HC[2],
                 int *error_code )
{
   float XZ, YZ;
   Gan_Vector2_f x;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_STMAP_CAMERA, "point_project",
                       GAN_ERROR_INCOMPATIBLE, "" );

   if ( X->z == 0.0f )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_project", GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   if ( X->z == 1.0f )
   {
      XZ = X->x;
      YZ = X->y;
   }
   else
   {
      /* divide through by Z coordinate to normalise it to 1 */
      XZ = X->x/X->z;
      YZ = X->y/X->z;
   }

   /* build homogeneous image coordinates of projected point */
   (void)gan_vec3_fill_q ( p, CX0 + CFX*XZ, CY0 + CFY*YZ, CZH );

   x.x = p->x;
   x.y = p->y;

   /* distort point by ST map */
   if ( point_add_distortion( camera, p, p, Xpprev, error_code ) != GAN_TRUE )
      return GAN_FALSE;

   /* if we are computing Jacobians, the camera coordinates must have Z=1 */
#ifndef NDEBUG
   if ( (HX != NULL || HC != NULL) && X->z != 1.0f )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_project", GAN_ERROR_INCOMPATIBLE, "" );
      }
      else
         *error_code = GAN_ERROR_INCOMPATIBLE;

      return GAN_FALSE;
   }
#endif

   /* compute Jacobians */
   if ( HX || HC )
   {
      if ( camera->nonlinear.stmap.stmap_dir )
      {
         Gan_Matrix22_f H; // derivatives of distorted point with respect to coordinates of undistorted image point

         // Compute derivatives of projected point with respect to coordinates of original point
         // Use linear interpolation
         float ax[2], bx[2], ay[2], by[2]; // Interpolation coefficients for x and y respectively
         int neib_x[2], neib_y[2]; // Integer neighbours of point
         Gan_Vector2_f neib[2][2];
         Gan_Vector2_f q;
         Gan_Vector2_f centreST; // Centre of ST map

         /* map image point to a point on ST map */
         centreST.x = 0.5f * camera->nonlinear.stmap.stmap_dir->width;
         centreST.y = 0.5f * camera->nonlinear.stmap.stmap_dir->height;

         x.x += centreST.x - CX0;
         x.y += centreST.y - CY0;

         build_linear_interpolation_neighbours( &x, neib_x, neib_y );

         build_linear_interpolation_weightsJ( &x, neib_x, neib_y,
                                              ax, bx, ay, by );

         build_linear_interpolation_values(
            camera->nonlinear.stmap.stmap_dir, neib_x, neib_y, neib, 2.0f * CX0, 2.0f * CY0 );

         q = interpolate_linear( ax, bx, neib );
         H.xx = q.x;
         H.yx = q.y;

         q = interpolate_linear( ay, by, neib );
         H.xy = q.x;
         H.yy = q.y;

         if ( HX != NULL )
         {
            // derivatives of distorted point with respect to coordinates of 3d point
            /*
            HX = H * D, where D is a matrix of derivatives of undistorted image point with respect
            to coordinates of 3d point,
            D is a diagonal matrix, Dxx = CFX, Dyy = CFY
            */
            HX->xx = H.xx * CFX;
            HX->yx = H.yx * CFX;

            HX->xy = H.xy * CFY;
            HX->yy = H.yy * CFY;
         }

         if ( HC != NULL )
         {

            // derivatives of distorted point with respect to linear camera parameters
            /*
            HC = H * G, where G is a matrix of derivatives of undistorted image point with respect
            linear camera parameters
            */
            HC[0].zh = 0.f;
            HC[0].fx = H.xx * XZ;
            HC[0].fy = H.xy * YZ;
            HC[0].x0 = 1.0f;
            HC[0].y0 = 0.f;
            HC[0].type = GAN_LINEAR_CAMERA;
            HC[1].zh = 0.f;
            HC[1].fx = H.yx * XZ;
            HC[1].fy = H.yy * YZ;
            HC[1].x0 = 0.f;
            HC[1].y0 = 1.0f;
            HC[1].type = GAN_LINEAR_CAMERA;
         }
      }
      else
      {
         /* compute Jacobians */
         if ( HX != NULL )
            (void)gan_mat22f_fill_q ( HX, CFX, 0.0f,
                                         0.0f, CFY );

         if ( HC != NULL )
         {
            HC[0].zh = 0.0f;
            HC[0].fx = XZ;
            HC[0].fy = 0.0f;
            HC[0].x0 = 1.0f;
            HC[0].y0 = 0.0f;
            HC[0].type = GAN_LINEAR_CAMERA;
            HC[1].zh = 0.0f;
            HC[1].fx = 0.0f;
            HC[1].fy = YZ;
            HC[1].x0 = 0.0f;
            HC[1].y0 = 1.0f;
            HC[1].type = GAN_LINEAR_CAMERA;
         }
      }
   }

   return GAN_TRUE;
}

/* point back-projection function */
static Gan_Bool
 point_backproject ( const Gan_Camera_f *camera,
                     Gan_Vector3_f *p, Gan_Vector3_f *X, Gan_PositionState_f *pXprev,
                     int *error_code )
{
   Gan_Vector3_f pu;
   Gan_Vector2_f Xd;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_STMAP_CAMERA, "point_backproject", GAN_ERROR_INCOMPATIBLE, "" );

   if ( point_remove_distortion( camera, p, &pu, pXprev, error_code ) != GAN_TRUE )
      return GAN_FALSE;

   if ( CFX == 0.0f || CFY == 0.0f || CZH == 0.0f )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_backproject", GAN_ERROR_DIVISION_BY_ZERO, "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   /* can't back-project a point on z=0 plane */
   if ( pu.z == 0.0f )
   {
      if ( error_code == NULL )
      {
         gan_err_flush_trace();
         gan_err_register ( "point_backproject", GAN_ERROR_DIVISION_BY_ZERO,
                            "" );
      }
      else
         *error_code = GAN_ERROR_DIVISION_BY_ZERO;

      return GAN_FALSE;
   }

   /* compute distorted X/Y coordinates of scene point Xd on plane Z=1 */
   (void)gan_vec2f_fill_q ( &Xd, (CZH*pu.x - CX0*pu.z)/(CFX*pu.z),
                                (CZH*pu.y - CY0*pu.z)/(CFY*pu.z) );

   /* build scene point */
   gan_vec3f_set_parts_q ( X, &Xd, 1.0f );

   /* success */
   return GAN_TRUE;
}

/* line projection function */
static Gan_Bool
 line_project ( const Gan_Camera_f *camera,
                Gan_Vector3_f *L, Gan_Vector3_f *l )
{
   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_STMAP_CAMERA, "line_project", GAN_ERROR_INCOMPATIBLE, "" );

   if ( camera->fx == 0.0f || camera->fy == 0.0f || camera->zh == 0.0f )
   {
      gan_err_flush_trace();
      gan_err_register ( "line_project", GAN_ERROR_DIVISION_BY_ZERO, "" );
   }

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "line_project", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;
}

/* line back-projection function */
static Gan_Bool
 line_backproject ( const Gan_Camera_f *camera, Gan_Vector3_f *l, Gan_Vector3_f *L )
{
   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_STMAP_CAMERA,
                       "line_backproject", GAN_ERROR_INCOMPATIBLE, "" );

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "line_backproject", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;
}

Gan_Bool gan_cameraf_build_ST_map ( Gan_Camera_f *camera,
                                    float zh,
                                    float fx, float fy,
                                    float x0, float y0,
                                    struct Gan_Image *stmap_dir,
                                    struct Gan_Image *stmap_inv )
{
   if ( !gan_cameraf_set_common_fields ( camera, GAN_STMAP_CAMERA,
                                         zh, fx, fy, x0, y0 ) )
   {
      gan_err_register ( "gan_camera_build_ST_map",
                         GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* copy non-linear camera parameters */
   camera->nonlinear.stmap.stmap_dir = stmap_dir;
   camera->nonlinear.stmap.stmap_inv = stmap_inv;

   /* add point projection/backprojection/distortion functions */
   camera->pointf.project           = point_project;
   camera->pointf.backproject       = point_backproject;
   camera->pointf.add_distortion    = point_add_distortion;
   camera->pointf.remove_distortion = point_remove_distortion;

   /* add line projection/backprojection/distortion functions */
   camera->linef.project     = line_project;
   camera->linef.backproject = line_backproject;

   /* success */
   return GAN_TRUE;
}

/**
 * \}
 */

/**
 * \}
 */
