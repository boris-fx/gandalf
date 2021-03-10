/**
 * File:          $RCSfile: camera_equirectangular.c,v $
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
#include <gandalf/vision/camera_equirectangular.h>
#include <gandalf/image/image_defs.h>
#include <gandalf/common/numerics.h>
#include <gandalf/common/compare.h>

static void gan_camera_equirect_dual_p( const Gan_Camera * camera, Gan_Camera* dualCamera );

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
#define CLONG camera->nonlinear.equirectangular.longitude
#define CLAT camera->nonlinear.equirectangular.latitude
#define CFoV camera->nonlinear.equirectangular.FoV

/////////////////////////////////////////////////////////////////////
// Utility functions to use in camera interface methods

static Gan_Matrix33 const* build_camera_rotation( const Gan_Camera * constCamera )
{
   double sLong = 0, cLong = 0, sLat = 0, cLat = 0;

   Gan_Camera *camera = (Gan_Camera *) constCamera;

   if( CLONG == camera->nonlinear.equirectangular.cache.rot.lon &&
       CLAT  == camera->nonlinear.equirectangular.cache.rot.lat )
      return &camera->nonlinear.equirectangular.cache.rot.m33;

   sLong = sin(CLONG);
   cLong = cos(CLONG);
   sLat = sin(CLAT);
   cLat = cos(CLAT);

   camera->nonlinear.equirectangular.cache.rot.m33.xx = cLong;
   camera->nonlinear.equirectangular.cache.rot.m33.xy = -sLong * sLat;
   camera->nonlinear.equirectangular.cache.rot.m33.xz = sLong * cLat;
   camera->nonlinear.equirectangular.cache.rot.m33.yx = 0;
   camera->nonlinear.equirectangular.cache.rot.m33.yy = cLat;
   camera->nonlinear.equirectangular.cache.rot.m33.yz = sLat;
   camera->nonlinear.equirectangular.cache.rot.m33.zx = -sLong;
   camera->nonlinear.equirectangular.cache.rot.m33.zy = -cLong * sLat;
   camera->nonlinear.equirectangular.cache.rot.m33.zz = cLong * cLat;

   camera->nonlinear.equirectangular.cache.rot.lon = CLONG;
   camera->nonlinear.equirectangular.cache.rot.lat = CLAT;

   return &camera->nonlinear.equirectangular.cache.rot.m33;
}

static double cameraFov_2( Gan_Camera const * camera)
{
   Gan_Camera * pCamera = (Gan_Camera*) camera;
   // cache tanFoV_2
   if( pCamera->nonlinear.equirectangular.cache.fov.fov != pCamera->nonlinear.equirectangular.FoV )
   {
      pCamera->nonlinear.equirectangular.cache.fov.fov = pCamera->nonlinear.equirectangular.FoV;
      pCamera->nonlinear.equirectangular.cache.fov.tanFoV_2 =
         tan( pCamera->nonlinear.equirectangular.FoV * 0.5);
   }
   return pCamera->nonlinear.equirectangular.cache.fov.tanFoV_2;
}

// Normalise longitude to the range [-pi,pi).
static double normalisedLongitude(double longitude)
{
   if (longitude < -M_PI || longitude >= M_PI )
      longitude -= 2*M_PI*floor((longitude+M_PI)/(2*M_PI));
   return longitude;
}

static Gan_Bool point_distort(
   const Gan_Camera *camera,
   Gan_Vector3 *pu, Gan_Vector3 *p, Gan_PositionState *pupprev,
   int *error_code, Gan_Bool undistort )
{
   Gan_Vector2 x;
   double longitude, latitude;
   Gan_Vector3 q;
   Gan_Matrix33 const * pR;
   double tanFoV_2 = cameraFov_2(camera);
   double width = 2*CX0;
   double height = 2*CY0;
   double diag2 = width*width + height*height;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_EQUIRECTANGULAR_CAMERA,
                       "point_distort", GAN_ERROR_INCOMPATIBLE, "" );

#ifndef NDEBUG
   if ( camera->fx == 0.0 || camera->fy == 0.0 || camera->zh == 0.0 )
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

   if ( pu->z == 0.0 )
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
   /* x = gan_vec2_fill_s ( CZH*pu->x/pu->z, CZH*pu->y/pu->z ); */
   x.x = CZH*pu->x/pu->z;
   x.y = CZH*pu->y/pu->z;

   pR = build_camera_rotation( camera );

   if ( undistort == GAN_TRUE )
   {
      double cLat,xq,yq,zq;

      /* transfer point from equirectangular CF to rectilinear CF */
      gan_camera_equirect_xy2latlong( x.x, x.y, width, height, &longitude, &latitude, camera );
      cLat = cos(latitude);

      q.x = sin(longitude)*cLat;
      q.y = sin(latitude);
      q.z = cos(longitude)*cLat;

      // q = gan_mat33T_multv3_s( pR, &q );
      xq = pR->xx*q.x + pR->yx*q.y + pR->zx*q.z;
      yq = pR->xy*q.x + pR->yy*q.y + pR->zy*q.z;
      zq = pR->xz*q.x + pR->yz*q.y + pR->zz*q.z;
      if ( zq != 0 )
      {
         /* q = gan_vec3_scale_s( &q, width/(2*tanFoV_2*q.z) ); */
         double s = width/(2*tanFoV_2*zq);
         q.x = s*xq;
         q.y = s*yq;
      }
      else
         q.x = q.y = sqrt(DBL_MAX)/4;

      x.x = q.x + width/2;
      x.y = q.y + height/2;
   }
   else
   {
      /* transfer point from rectilinear CF to equirectangular CF */
      double d, xq, yq, zq;
      q.x = x.x - width/2;
      q.y = x.y - height/2;
      q.z = width/(2*tanFoV_2);

      /* q = gan_mat33_multv3_s( pR, &q ); */
      xq = pR->xx*q.x + pR->xy*q.y + pR->xz*q.z;
      yq = pR->yx*q.x + pR->yy*q.y + pR->yz*q.z;
      zq = pR->zx*q.x + pR->zy*q.y + pR->zz*q.z;
      longitude = atan2(xq, zq);
      d = yq/sqrt(xq*xq+yq*yq+zq*zq);

      if ( d > 1.0 )
         d = 1.0;
      else if ( d < -1.0)
         d = -1.0;
      latitude = asin(d);

      longitude = normalisedLongitude(longitude);

      gan_camera_equirect_latlong2xy( longitude, latitude, width, height,
         &x.x, &x.y, camera );
   }

   /* fill image point with distortion added */
   gan_vec3_set_parts_q ( p, &x, CZH );

   /* success */
   return GAN_TRUE;
}

/* function to add distortion to a point */
static Gan_Bool
 point_add_distortion ( const Gan_Camera *camera,
                        Gan_Vector3 *pu, Gan_Vector3 *p, Gan_PositionState *pupprev,
                        int *error_code )
{
   return point_distort( camera, pu, p, pupprev, error_code, GAN_FALSE );
}

/* function to remove distortion from a point */
static Gan_Bool
 point_remove_distortion ( const Gan_Camera *camera,
                           Gan_Vector3 *p, Gan_Vector3 *pu, Gan_PositionState *ppuprev,
                           int *error_code)
{
   return point_distort( camera, p, pu, ppuprev, error_code, GAN_TRUE );
}

/////////////////////////////////////////////////////////////////////
//
/* point projection function */
static Gan_Bool point_project_internal(
   const Gan_Camera *camera,
   Gan_Vector3 *X, Gan_Vector3 *p, Gan_PositionState *Xpprev,
   Gan_Matrix22 *HX, Gan_Camera HC[2],
   int *error_code )
{
   double XZ, YZ;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_EQUIRECTANGULAR_CAMERA, "point_project",
                       GAN_ERROR_INCOMPATIBLE, "" );

   if ( X->z == 0.0 )
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

   if ( X->z == 1.0 )
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
   gan_vec3_fill_q( p, CX0 + CFX*XZ, CY0 + CFY*YZ, CZH );

   if ( point_add_distortion( camera, p, p, Xpprev, error_code ) != GAN_TRUE )
      return GAN_FALSE;

   /* if we are computing Jacobians, the camera coordinates must have Z=1 */
#ifndef NDEBUG
   if ( (HX != NULL || HC != NULL) && X->z != 1.0 )
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
      /* derivatives of distorted point w.r.t coordinates of undistorted image point */
      Gan_Matrix22 H;
      Gan_Matrix33 const * pR;
      double tanFoV_2 = cameraFov_2(camera);
      double width = 2*CX0;
      double height = 2*CY0;
      Gan_Matrix23 Q;
      Gan_Vector3 q;
      double longitude, latitude, cosLat, sinLat, cosLong, sinLong;

      gan_camera_equirect_xy2latlong( p->x, p->y, width, height, &longitude, &latitude, camera );

      cosLat = cos( latitude );
      sinLat = sin( latitude );
      cosLong = cos( longitude );
      sinLong = sin( longitude );

      pR = build_camera_rotation( camera );

      q.x = sinLong*cosLat;
      q.y = sinLat;
      q.z = cosLong*cosLat;
      q = gan_mat33T_multv3_s(pR, &q);

      Q.xx = cosLat*cosLong*2.0*M_PI/width;
      Q.xy = 0;
      Q.xz = -cosLat*sinLong*2.0*M_PI/width;

      Q.yx = -sinLong*sinLat*M_PI/height;
      Q.yy = cosLat*M_PI/height;
      Q.yz = -cosLong*sinLat*M_PI/height;

      Q = gan_mat23_rmultm33_s( &Q, pR );

      H.xx = q.z*Q.xx - q.x*Q.xz;
      H.xy = q.z*Q.yx - q.x*Q.yz;
      H.yx = q.z*Q.xy - q.y*Q.xz;
      H.yy = q.z*Q.yy - q.y*Q.yz;
      gan_mat22_scale_i(&H, width/(2*tanFoV_2*q.z*q.z));

      H = gan_mat22_invert_s( &H );

      /* compute derivatives of projected point w.r.t. coordinates of original point */
      if ( HX != NULL )
      {
         /* derivatives of distorted point w.r.t. coordinates of 3d point */
         /* HX = H * D, where D is a matrix of derivatives of undistorted image point w.r.t. 
         coordinates of 3d point, D is a diagonal matrix, Dxx = CFX, Dyy = CFY */
         HX->xx = H.xx * CFX;
         HX->yx = H.yx * CFX;

         HX->xy = H.xy * CFY;
         HX->yy = H.yy * CFY;
      }

      if ( HC != NULL )
      {

         /* derivatives of distorted point w.r.t linear camera parameters */
         /* HC = H * G, where G is a matrix of derivatives of undistorted image point w.r.t.
         linear camera parameters */
         HC[0].zh = 0;
         HC[0].fx = H.xx * XZ;
         HC[0].fy = H.xy * YZ;
         HC[0].x0 = 1.0;
         HC[0].y0 = 0;
         HC[0].type = GAN_LINEAR_CAMERA;
         HC[1].zh = 0;
         HC[1].fx = H.yx * XZ;
         HC[1].fy = H.yy * YZ;
         HC[1].x0 = 0;
         HC[1].y0 = 1.0;
         HC[1].type = GAN_LINEAR_CAMERA;
      }
   }

   return GAN_TRUE;
}

static Gan_Bool point_project(
   const Gan_Camera *camera,
   Gan_Vector3 *X, Gan_Vector3 *p, Gan_PositionState *Xpprev,
   Gan_Matrix22 *HX, Gan_Camera HC[2],
   int *error_code )
{
   if ( X->z < 0 )
   {
      Gan_Vector3 Y = *X;
      Y.y *= -1;

      if(!camera->nonlinear.equirectangular.dualCamera)
      {
         Gan_Camera dualCamera;
         gan_camera_equirect_dual_p(camera, &dualCamera);
         return point_project_internal( &dualCamera, &Y, p, Xpprev, HX, HC, error_code );
      }
      else
         return point_project_internal( camera->nonlinear.equirectangular.dualCamera, &Y, p, Xpprev, HX, HC, error_code );

   }

   return point_project_internal( camera, X, p, Xpprev, HX, HC, error_code );
}

/* point back-projection function */
static Gan_Bool point_backproject_internal(
   const Gan_Camera *camera,
   Gan_Vector3 *p, Gan_Vector3 *X, Gan_PositionState *pXprev,
   int *error_code )
{
   Gan_Vector3 pu;
   Gan_Vector2 Xd;

   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_EQUIRECTANGULAR_CAMERA, "point_backproject", GAN_ERROR_INCOMPATIBLE, "" );

   if ( point_remove_distortion( camera, p, &pu, pXprev, error_code ) != GAN_TRUE )
      return GAN_FALSE;

   if ( CFX == 0.0 || CFY == 0.0 || CZH == 0.0 )
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
   if ( pu.z == 0.0 )
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
   (void)gan_vec2_fill_q ( &Xd, (CZH*pu.x - CX0*pu.z)/(CFX*pu.z),
                                (CZH*pu.y - CY0*pu.z)/(CFY*pu.z) );

   /* build scene point */
   gan_vec3_set_parts_q ( X, &Xd, 1.0 );

   /* success */
   return GAN_TRUE;
}

static Gan_Bool point_backproject(
   const Gan_Camera *camera,
   Gan_Vector3 *p, Gan_Vector3 *X, Gan_PositionState *pXprev,
   int *error_code )
{
   if ( gan_camera_equirect_point_behind( camera, p->x, p->y ) )
   {
      Gan_Vector3 q = *p;
      Gan_Bool result;
      double fullHeight = 2*CY0 +
         camera->nonlinear.equirectangular.margin.top +
         camera->nonlinear.equirectangular.margin.bottom;

      q.y = fullHeight - q.y - camera->nonlinear.equirectangular.margin.top;

      if(!camera->nonlinear.equirectangular.dualCamera)
      {
         Gan_Camera dualCamera;
         gan_camera_equirect_dual_p(camera, &dualCamera);
         result = point_backproject_internal( &dualCamera, &q, X, pXprev, error_code );
      }
      else
         result = point_backproject_internal( camera->nonlinear.equirectangular.dualCamera, &q, X, pXprev, error_code );

      gan_vec3_negate_i( X );
      return result;
   }

   return point_backproject_internal( camera, p, X, pXprev, error_code );
}

/* line projection function */
static Gan_Bool line_project(
   const Gan_Camera *camera, Gan_Vector3 *L, Gan_Vector3 *l )
{
   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_EQUIRECTANGULAR_CAMERA, "line_project", GAN_ERROR_INCOMPATIBLE, "" );

   if ( camera->fx == 0.0 || camera->fy == 0.0 || camera->zh == 0.0 )
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
static Gan_Bool line_backproject(
   const Gan_Camera *camera, Gan_Vector3 *l, Gan_Vector3 *L )
{
   /* consistency check */
   gan_err_test_bool ( camera->type == GAN_EQUIRECTANGULAR_CAMERA,
                       "line_backproject", GAN_ERROR_INCOMPATIBLE, "" );

   /* not implemented */
   gan_err_flush_trace();
   gan_err_register ( "line_backproject", GAN_ERROR_NOT_IMPLEMENTED, "" );
   return GAN_FALSE;
}

Gan_Bool gan_camera_build_equirectangular(
   Gan_Camera *camera,
   double zh,
   double fx, double fy,
   double x0, double y0,
   double longitude, double latitude,
   double FoV )
{
   if( !gan_camera_set_common_fields( camera, GAN_EQUIRECTANGULAR_CAMERA,
                                       zh, fx, fy, x0, y0 ) )
   {
      gan_err_register ( "gan_camera_build_equirectangular",
                         GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* copy non-linear camera parameters */
   camera->nonlinear.equirectangular.longitude = longitude;
   camera->nonlinear.equirectangular.latitude = latitude;
   camera->nonlinear.equirectangular.FoV = FoV;
   camera->nonlinear.equirectangular.dualCamera = NULL;

   // Initialise cache data
   camera->nonlinear.equirectangular.cache.rot.lon = DBL_MAX ;
   camera->nonlinear.equirectangular.cache.rot.lat = DBL_MAX ;
   gan_mat33_ident_q(&camera->nonlinear.equirectangular.cache.rot.m33);
   camera->nonlinear.equirectangular.cache.fov.fov = DBL_MAX;
   camera->nonlinear.equirectangular.cache.fov.tanFoV_2 = DBL_MAX;
   camera->nonlinear.equirectangular.cache.direction.lon = DBL_MAX ;
   camera->nonlinear.equirectangular.cache.direction.lat = DBL_MAX ;

   // Initialise margins
   camera->nonlinear.equirectangular.margin.left   = 0;
   camera->nonlinear.equirectangular.margin.top    = 0;
   camera->nonlinear.equirectangular.margin.right  = 0;
   camera->nonlinear.equirectangular.margin.bottom = 0;

   /* add point projection/backprojection/distortion functions */
   camera->point.project           = point_project;
   camera->point.backproject       = point_backproject;
   camera->point.add_distortion    = point_add_distortion;
   camera->point.remove_distortion = point_remove_distortion;

   /* add line projection/backprojection/distortion functions */
   camera->line.project     = line_project;
   camera->line.backproject = line_backproject;

   /* success */
   return GAN_TRUE;
}

Gan_Camera gan_camera_equirect_dual( const Gan_Camera * camera )
{
   Gan_Camera dualCamera;
   gan_camera_equirect_dual_p( camera, &dualCamera );
   return dualCamera;
}

static void gan_camera_equirect_dual_p( const Gan_Camera * camera, Gan_Camera * dualCamera )
{
   if( !camera )
   {
      gan_err_register( "gan_camera_equirect_dual_p", GAN_ERROR_FAILURE, "" );
   }
   else
   {
      *dualCamera = *camera;

      if( dualCamera->nonlinear.equirectangular.longitude > 0 )
         dualCamera->nonlinear.equirectangular.longitude -= M_PI;
      else
         dualCamera->nonlinear.equirectangular.longitude += M_PI;

      dualCamera->nonlinear.equirectangular.latitude *= -1;
   }
}

Gan_Vector3 gan_camera_equirect_direction( double longitude, double latitude )
{
   Gan_Vector3 dir;
   gan_camera_equirect_direction_p(longitude, latitude, &dir);
   return dir;
}

void gan_camera_equirect_direction_p(double longitude, double latitude, Gan_Vector3 * dir)
{
   double cosLat = cos(latitude);

   dir->x = cos(longitude)*cosLat;
   dir->z = sin(longitude)*cosLat;
   dir->y = sin(latitude);
}

Gan_Bool gan_camera_equirect_point_behind(
   const Gan_Camera * constCamera, double x, double y )
{
   Gan_Camera * camera = (Gan_Camera *)constCamera;

   double longitude;
   double latitude;
   Gan_Vector3 dirPoint;

   gan_camera_equirect_xy2latlong( x, y, 2*CX0, 2*CY0, &longitude, &latitude, constCamera );
   longitude = normalisedLongitude( longitude );
   gan_camera_equirect_direction_p(longitude, latitude, &dirPoint);

   if( CLONG != camera->nonlinear.equirectangular.cache.direction.lon ||
       CLAT  != camera->nonlinear.equirectangular.cache.direction.lat)
   {
      // Calculate and cache camera->nonlinear.equirectangular.cache.direction.vec
      gan_camera_equirect_direction_p( CLONG, CLAT, &camera->nonlinear.equirectangular.cache.direction.vec );
      camera->nonlinear.equirectangular.cache.direction.lon = CLONG;
      camera->nonlinear.equirectangular.cache.direction.lat= CLAT;
   }

   return gan_vec3_dot_q(&dirPoint, &camera->nonlinear.equirectangular.cache.direction.vec) < 0 ? GAN_TRUE : GAN_FALSE;
}

void gan_camera_equirect_xy2latlong(
   double x, double y,
   double width, double height,
   double * longitude, double * latitude, const Gan_Camera * camera )
{
   if(camera)
   {
      double fullWidth = width +
                        camera->nonlinear.equirectangular.margin.left +
                        camera->nonlinear.equirectangular.margin.right;
      double fullHeight = height +
                        camera->nonlinear.equirectangular.margin.top +
                        camera->nonlinear.equirectangular.margin.bottom;
      *longitude = 2*M_PI * ( x - fullWidth/2  + camera->nonlinear.equirectangular.margin.left ) / fullWidth;
      *latitude =    M_PI * ( y - fullHeight/2 + camera->nonlinear.equirectangular.margin.top  ) / fullHeight;
   }
   else
   {
      *longitude = 2*M_PI * ( x - width/2  ) / width;
      *latitude  =   M_PI * ( y - height/2 ) / height;
   }
}

void gan_camera_equirect_latlong2xy(
   double longitude, double latitude,
   double width, double height,
   double * x, double * y, const Gan_Camera * camera )
{
   if(camera)
   {
      double fullWidth = width +
                        camera->nonlinear.equirectangular.margin.left +
                        camera->nonlinear.equirectangular.margin.right;
      double fullHeight = height +
                        camera->nonlinear.equirectangular.margin.top +
                        camera->nonlinear.equirectangular.margin.bottom;

      *x = fullWidth*longitude/2/M_PI + fullWidth/2  - camera->nonlinear.equirectangular.margin.left;
      *y = fullHeight*latitude / M_PI + fullHeight/2 - camera->nonlinear.equirectangular.margin.top;
   }
   else
   {
      *x = width*longitude/2/M_PI + width/2;
      *y = height*latitude / M_PI + height/2;
   }
}

/**
 * \}
 */

/**
 * \}
 */
