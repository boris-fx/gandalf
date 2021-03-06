/**
 * File:          $RCSfile: vision_test.c,v $
 * Module:        Vision package test program
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.39 $
 * Last edited:   $Date: 2006/04/20 16:17:04 $
 * Author:        $Author: nicolas $
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

#include <gandalf/TestFramework/cUnit.h>
#include <gandalf/vision/vision_test.h>

#include <gandalf/image/io/image_io.h>
#include <gandalf/vision/mask1D.h>
#include <gandalf/vision/harris_corner.h>
#include <gandalf/vision/canny_edge.h>
#include <gandalf/vision/segment_strings.h>
#include <gandalf/vision/orthog_line.h>
#include <gandalf/vision/lev_marq.h>
#include <gandalf/vision/essential.h>
#include <gandalf/vision/fundamental.h>
#include <gandalf/vision/camera_linear.h>
#include <gandalf/linalg/mat_symmetric.h>
#include <gandalf/linalg/3x3matrix.h>
#include <gandalf/linalg/3vector.h>
#include <gandalf/common/numerics.h>
#include <gandalf/common/misc_error.h>
#include <gandalf/image/image_convert.h>

#ifdef WIN32
        #include <windows.h>
#endif

#ifdef VISION_TEST_MAIN
#include <gandalf/image/image_display.h>
#include <gandalf/vision/corner_disp.h>
#include <gandalf/vision/edge_disp.h>
#include <gandalf/vision/line_disp.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

static int window_id;

/* display colours */
static Gan_RGBPixel_f blue = {0.0F, 0.0F, 1.0F}, green = {0.0F, 1.0F, 0.0F},
                      yellow = {1.0F, 1.0F, 0.0F}, red = {1.0F, 0.0F, 0.0F};

#define SHOW_CORNERS   0
#define SHOW_EDGES     1
#define SHOW_LINES     2
#define QUIT          99

/* defines currently displayed type of features */
static int mode = -1;
#endif /* #ifdef VISION_TEST_MAIN */

/* grey-level and RGB versions of image */
static Gan_Image *img_grey = NULL, *img_rgb = NULL;

/* feature map pointers */
static Gan_CornerFeatureMap *cmap = NULL;
static Gan_EdgeFeatureMap *emap = NULL;
static Gan_LineFeatureMap *lmap = NULL;

#ifdef VISION_TEST_MAIN
/* Displays the image on the screen */
static void display_image(void)
{
   /* display image */
   glRasterPos2i ( 0, 0 );
   gan_image_display ( img_grey );
   switch ( mode )
   {
      case -1:
        /* no features yet */
        break;

      case SHOW_CORNERS:
        gan_corner_feature_map_display ( cmap, 2.0F, NULL, &blue, &blue  );
        break;

      case SHOW_EDGES:
        gan_edge_feature_map_display ( emap, 0.0F, NULL, &red, &green,
                                       &blue, &yellow, &green );
        break;

      case SHOW_LINES:
        gan_line_feature_map_display ( lmap, 1.0F, NULL,
                                       &green, &blue, &yellow, &blue, &green );
        break;

      default:
        fprintf ( stderr, "illegal display mode\n" );
        exit(EXIT_FAILURE);
        break;
   }

   glFlush();
}

/* Free up resources and exit */
static void cleanup(void)
{
   glutDestroyWindow ( window_id );
        
   if ( cmap != NULL ) gan_corner_feature_map_free ( cmap );
   if ( emap != NULL ) gan_edge_feature_map_free ( emap );
   if ( lmap != NULL ) gan_line_feature_map_free ( lmap );
   gan_image_free_va ( img_rgb, img_grey, NULL );
   exit(EXIT_SUCCESS);
}

/* Determines the rpessed touch of the keyboard*/


static void affiche(unsigned char key,int x,int y)
{
 printf(" la touche pressee est :s \n");
 glutPostRedisplay();
}



/* Determines and processes the option the user has selected from the menu */
static void ModeMenu ( int entry )
{
   switch ( entry )
   {
      case SHOW_CORNERS:
        fprintf ( stderr, "displaying corners\n" );
        break;
        
      case SHOW_EDGES:
        fprintf ( stderr, "displaying edges\n" );
        break;
        
      case SHOW_LINES:
        fprintf ( stderr, "displaying lines\n" );
        break;
        
      case QUIT:
        cleanup();
        exit(EXIT_SUCCESS);
        break;
        
      default:
        cleanup();
        fprintf ( stderr, "illegal menu entry %d\n", entry );
        exit(EXIT_FAILURE);
        break;
   }
   
   /* store latest display mode for redisplay events */
   mode = entry;
   
   /* display image and features */
   display_image();
}

static void reshape ( int window_width, int window_height )
{
   glViewport ( 0, 0, window_width, window_height );

   /* set coordinate frame for graphics in window */
   glMatrixMode ( GL_PROJECTION );
   glLoadIdentity();

   gluOrtho2D ( 0, img_grey->width, img_grey->height, 0 );

   glMatrixMode ( GL_MODELVIEW );
   glLoadIdentity();

   /* set zoom factors for image display */
   glPixelZoom ( ((float)window_width)/((float)img_grey->width),
                 -((float)window_height)/((float)img_grey->height) );
}
#endif /* #ifdef VISION_TEST_MAIN */

/* Code to fit Quadratic by least squares */

/* number of points */
#define NPOINTS 100

/* ground-truth quadratic coefficients a,b,c */
#define A_TRUE 2.0
#define B_TRUE 3.0
#define C_TRUE 4.0

/* noise standard deviation */
#define SIGMA 1.0

#define vgel(x,i)   gan_vec_get_el(x,i)
#define vsel(x,i,v) gan_vec_set_el(x,i,v)

/* observation callback function for single point */
static Gan_Bool
 quadratic_h ( Gan_Vector *x,  /* state vector */
               Gan_Vector *z,  /* observation vector */
               void *zdata,    /* user pointer attached to z */
               Gan_Vector *h,  /* vector h(x) to be evaluated */
               Gan_Matrix *H ) /* matrix dh/dx to be evaulated or NULL */
{
   double a, b, c;

   /* read x-coordinate from user-defined data pointer */
   double xj = *((double *) zdata);

   /* read quadratic parameters from state vector x=(a b c)^T*/
   if ( !gan_vec_read_va ( x, 3, &a, &b, &c ) )
   {
      gan_err_register ( "quadratic_h", GAN_ERROR_FAILURE, NULL );
      return GAN_FALSE;
   }

   /* evaluate h(x) = h(a,b,c) = y = a*x*x + b*x + c */
   if ( gan_vec_fill_va ( h, 1, a*xj*xj + b*xj + c ) == NULL )
   {
      gan_err_register ( "quadratic_h", GAN_ERROR_FAILURE, NULL );
      return GAN_FALSE;
   }

   /* if Jacobian matrix is passed as non-NULL, fill it with the Jacobian
      matrix (dh/da dh/db dh/dc) = (x*x x 1) */
   if ( H != NULL &&
        gan_mat_fill_va ( H, 1, 3, xj*xj, xj, 1.0 ) == NULL )
   {
      gan_err_register ( "quadratic_h", GAN_ERROR_FAILURE, NULL );
      return GAN_FALSE;
   }

   /* success */
   return GAN_TRUE;
}

/* initialisation function for state vector */
static Gan_Bool
 quadratic_init ( Gan_Vector *x0,     /* state vector to be initialised */
                  Gan_List *obs_list, /* list of observations */
                  void *data )        /* user data pointer */
{
   int list_size = gan_list_get_size(obs_list);
   Gan_LevMarqObs *obs;
   Gan_Matrix33 A;
   Gan_Vector3 b;
   double xj, y;

   /* we need at least three points to fit a quadratic */
   if ( list_size < 3 ) return GAN_FALSE;

   /* initialise quadratic by interpolating three points: the first, middle and
      last point in the list of point observations. We construct equations

         (y1)   (x1*x1 x1 1) (a)
         (y2) = (x2*x2 x2 1) (b) = A * b for 3x3 matrix A and 3-vector b
         (y3)   (x3*x3 x3 1) (c)

      and solve the equations by direct matrix inversion (not pretty...) to
      obtain our first estimate of a, b, c given points (x1,y1), (x2,y2) and
      (x3,y3).
   */

   /* first point */
   gan_list_goto_pos ( obs_list, 0 );
   obs = gan_list_get_current ( obs_list, Gan_LevMarqObs );
   xj = *((double *) obs->details.h.zdata); /* read x-coordinate */
   A.xx = xj*xj; A.xy = xj; A.xz = 1.0; /* fill first row of equations in A */
   gan_vec_read_va ( &obs->details.h.z, 1, &y );
   b.x = y; /* fill first entry in b vector */
    
   /* middle point */
   gan_list_goto_pos ( obs_list, list_size/2 );
   obs = gan_list_get_current ( obs_list, Gan_LevMarqObs );
   xj = *((double *) obs->details.h.zdata); /* read x-coordinate */
   A.yx = xj*xj; A.yy = xj; A.yz = 1.0; /* fill first row of equations in A */
   gan_vec_read_va ( &obs->details.h.z, 1, &y );
   b.y = y; /* fill second entry in b vector */
    
   /* last point */
   gan_list_goto_pos ( obs_list, list_size-1 );
   obs = gan_list_get_current ( obs_list, Gan_LevMarqObs );
   xj = *((double *) obs->details.h.zdata); /* read x-coordinate */
   A.zx = xj*xj; A.zy = xj; A.zz = 1.0; /* fill first row of equations in A */
   gan_vec_read_va ( &obs->details.h.z, 1, &y );
   b.z = y; /* fill second entry in b vector */

   /* invert matrix and solve (don't do this at home) */
   A = gan_mat33_invert_s(&A);
   b = gan_mat33_multv3_s ( &A, &b );

   /* fill state vector x0 with our initial values for a,b,c */
   gan_vec_fill_va ( x0, 3, b.x, b.y, b.z );
   return GAN_TRUE;
}

/* test function for Levenberg-Marquardt */
static Gan_Bool lev_marq_test(void)
{
   Gan_LevMarqStruct *lm;
   int i;

   /* arrays of x & y coordinates */
   double xcoord[NPOINTS], ycoord[NPOINTS];

   Gan_Vector *z; /* define observation vector */
   Gan_SquMatrix *Ni; /* define observation inverse covariance */
   Gan_Vector *x; /* solution state vector */
   double residual, new_residual;
   double lambda = 0.1; /* damping factor */

   /* initialise Levenberg-Marquardt algorithm */
   lm = gan_lev_marq_alloc();
   cu_assert ( lm != NULL );

   /* build arrays of x & y coordinates */
   for ( i = NPOINTS-1; i >= 0; i-- )
   {
      /* x-coordinates evenly spaced */
      xcoord[i] = (double) i;

      /* construct y = a*x^2 + b*y + c + w with added Gaussian noise w */
      ycoord[i] = A_TRUE*xcoord[i]*xcoord[i] + B_TRUE*xcoord[i]
                  + C_TRUE + gan_normal_sample(0.0, SIGMA);
   }

   /* allocate observation vector z and inverse covariance Ni */
   z = gan_vec_alloc(1);
   Ni = gan_symmat_fill_va ( NULL, 1, 1.0/(SIGMA*SIGMA) );
   cu_assert ( z != NULL && Ni != NULL );

   for ( i = NPOINTS-1; i >= 0; i-- )
   {
      /* construct point observation */
      z = gan_vec_fill_va ( z, 1, ycoord[i] );
      cu_assert ( z != NULL );
      cu_assert ( gan_lev_marq_obs_h ( lm, z, &xcoord[i], Ni, quadratic_h )
                  != NULL );
   }

   /* initialise Levenberg-Marquardt algorithm */
   cu_assert ( gan_lev_marq_init ( lm, quadratic_init, NULL, &residual ) );

   /* apply iterations */
   for ( i = 0; i < 10; i++ )
   {
      cu_assert ( gan_lev_marq_iteration ( lm, lambda, &new_residual ) );

      if ( new_residual < residual )
      {
         /* iteration succeeded in reducing the residual */
         lambda *= 0.1;
         residual = new_residual;
      }
      else
         /* iteration failed to reduce the residual */
         lambda *= 10.0;
   }

   /* get optimised solution */
   x = gan_lev_marq_get_x ( lm );
   cu_assert ( x != NULL );

   /* compute solution using a linear method (pseudo-inverse) and compare */
   {
      Gan_Matrix A;
      Gan_SquMatrix *ATA, *L;
      Gan_Vector b, *xlin;

      /* construct equations A*x = b */
      cu_assert ( gan_mat_form ( &A, NPOINTS, 3 ) != NULL &&
                  gan_vec_form ( &b, NPOINTS ) != NULL );
      for ( i = NPOINTS-1; i >= 0; i-- )
      {
         gan_mat_set_el ( &A, i, 0, xcoord[i]*xcoord[i] );
         gan_mat_set_el ( &A, i, 1, xcoord[i] );
         gan_mat_set_el ( &A, i, 2, 1.0 );
         gan_vec_set_el ( &b, i, ycoord[i] );
      }

      /* compute pseudo-inverse solution x = (A^T*A)^-1 * A^T * b. Rather
         than using explicit inverse we apply Cholesky factorisation to
         A^T*T, computing lower triangular L such that L*L^T = A^T*A, and
         then solving the triangular sets of equations that result */
      ATA = gan_mat_slmultT_s ( &A );
      L = gan_squmat_cholesky_s(ATA);
      xlin = gan_matT_multv_s ( &A, &b );
      gan_squmatI_multv_i ( L, xlin );
      gan_squmatIT_multv_i ( L, xlin );

      /* compare Levenberg-Marquardt computed solution with linearly computed
         solution */
      cu_assert ( gan_sqr_d(vgel(x,0)-vgel(xlin,0)) +
                  gan_sqr_d(vgel(x,1)-vgel(xlin,1)) +
                  gan_sqr_d(vgel(x,2)-vgel(xlin,2)) < 1.0e-6 );

      /* success */
      gan_mat_free(&A);
      gan_vec_free_va ( xlin, &b, NULL );
      gan_squmat_free_va ( L, ATA, NULL );
   }
   
   /* Free up resources */
   gan_squmat_free ( Ni );
   gan_vec_free ( z );
   gan_lev_marq_free ( lm );

   return GAN_TRUE;
}

#define ESSENTIAL_TEST_ANGLE  0.1
#define ESSENTIAL_TEST_AXIS_X 0.0
#define ESSENTIAL_TEST_AXIS_Y 1.0
#define ESSENTIAL_TEST_AXIS_Z 0.0
#define ESSENTIAL_TEST_TX     3.0
#define ESSENTIAL_TEST_TY     0.0
#define ESSENTIAL_TEST_TZ     0.0
#define ESSENTIAL_TEST_NO_POINTS 100

/* test essential matrix algorithm */
static Gan_Bool essential_matrix_test()
{
   Gan_Euclid3D pose, result;
   Gan_Camera camera1, camera2;
   int i;
   Gan_Vector3 X[ESSENTIAL_TEST_NO_POINTS];
   Gan_Vector3 p1[ESSENTIAL_TEST_NO_POINTS], p2[ESSENTIAL_TEST_NO_POINTS];

   /* set up ground truth pose */
   cu_assert(gan_rot3D_build_angle_axis(&pose.rot,
                                        ESSENTIAL_TEST_ANGLE,
                                        ESSENTIAL_TEST_AXIS_X,
                                        ESSENTIAL_TEST_AXIS_Y,
                                        ESSENTIAL_TEST_AXIS_Z));
   (void)gan_vec3_fill_q(&pose.trans,
                         ESSENTIAL_TEST_TX,
                         ESSENTIAL_TEST_TY,
                         ESSENTIAL_TEST_TZ);

   /* now build cameras */
   cu_assert(gan_camera_build_linear(&camera1, 100.0, 50.0, 50.0, 10.0, 10.0));
   cu_assert(gan_camera_build_linear(&camera2, 150.0, 10.0, 100.0, 20.0, -20.0));

   /* now build 3D points */
   for(i=ESSENTIAL_TEST_NO_POINTS-1; i >= 0; i--)
      (void) gan_vec3_fill_q(&X[i],
                             20.0*gan_random_m11(),
                             20.0*gan_random_m11(),
                             50.0 + 20.0*gan_random_01());

   /* project the points onto the images */
   for(i=ESSENTIAL_TEST_NO_POINTS-1; i >= 0; i--)
   {
      /* first camera */
      cu_assert(gan_camera_project_point_q(&camera1, &X[i], &p1[i]));

      /* second camera */
      p2[i] = gan_vec3_sub_s(&X[i], &pose.trans);
      p2[i] = gan_rot3D_apply_v3_s(&pose.rot, &p2[i]);
      if(p2[i].z <= 0.0)
         printf("Here\n");

      cu_assert(gan_camera_project_point_i(&camera2, &p2[i]));
   }

   /* now compute essential matrix */
   cu_assert(gan_essential_matrix_fit(p1, p2, ESSENTIAL_TEST_NO_POINTS, &camera1, &camera2, NULL, &result));

   result.rot = gan_rot3D_convert_s(&result.rot, GAN_ROT3D_ANGLE_AXIS);

   return GAN_TRUE;
}

/* test fundamental matrix algorithm */
static Gan_Bool fundamental_matrix_test()
{
   return GAN_TRUE;
}

static Gan_Bool setup_test(void)
{
   /* set default Gandalf error handler without trace handling */
   printf("\nSetup for vision_test completed!\n\n");
   return GAN_TRUE;
}

static Gan_Bool teardown_test(void)
{
   printf("\nTeardown for vision_test completed!\n\n");
   return GAN_TRUE;
}

/* Tests the vision functions */
static Gan_Bool run_test(void)
{
   Gan_Char *image_file = acBuildPathName(TEST_INPUT_PATH,GAN_STRING("brussels.pgm"));
   Gan_ImageFileFormat file_format = GAN_PGM_FORMAT;

   Gan_Mask1D *mask;
   Gan_LocalFeatureMapParams lpms = { 20, 20, 3 };

   /* test Levenberg-Marquardt algorithm */
   cu_assert ( lev_marq_test() );

   /* test essential matrix algorithm */
   cu_assert ( essential_matrix_test() );

   /* test fundamental matrix algorithm */
   cu_assert ( fundamental_matrix_test() );

   /* read image from file */
   img_rgb = gan_image_read ( image_file, file_format, NULL, NULL, NULL );
   cu_assert(img_rgb != NULL);

   /* Convert image to grey-level */
   img_grey = gan_image_convert_s ( img_rgb, GAN_GREY_LEVEL_IMAGE, img_rgb->type );
   cu_assert ( img_grey != NULL );

   /* build convolution mask */
   mask = gan_gauss_mask_new ( GAN_FLOAT, 1.0, 9, 1.0, NULL );
   cu_assert ( mask != NULL );

   /* compute edges of the image */
   emap = gan_canny_edge_s ( img_grey, NULL, mask, mask, NULL, NULL,
                             0.008, 0.024, 10, NULL, NULL, &lpms, NULL, NULL );
   cu_assert ( emap != NULL );

   /* compute corners of the image */
   cmap = gan_harris_corner_s ( img_grey, NULL, NULL, NULL, mask, mask, 0.04F,
                                0.04F, NULL, 0, NULL, &lpms );
   cu_assert ( cmap != NULL );

   /* compute lines of the image */
   lmap = gan_orthog_line_s ( emap, 10, 2, 1.0, &lpms, GAN_TRUE, NULL, NULL );
   cu_assert ( lmap != NULL );

#if 0
   /* segment the edge strings */
   cu_assert ( gan_segment_strings_q ( emap, 9, 0.5F, 10 ) );
#endif

   /* free convolution mask */
   gan_mask1D_free ( mask );

   /* number of edges is 22389 on all architectures tested so far */
   cu_assert ( emap->nedges >= 22384 && emap->nedges <= 22394 );

   /* number of corners is 1546 on all architectures tested so far */
   cu_assert ( cmap->ncorners >= 1534 && cmap->ncorners <= 1540 );

   /* number of lines found is 679 */
   cu_assert ( lmap->nlines >= 676 && lmap->nlines <= 682 );

#ifdef VISION_TEST_MAIN
   /* convert the Unicode string to a UTF-8 charcter array */
   char *image_file_ascii = NULL; /* UTF-8 converted image_file */
   size_t          n_uchars; /* size of source buffer */
   size_t          n_chars; /* size of destination buffer */

   n_uchars = gan_strlen(image_file);
   n_chars = gan_unicodechar_to_char( image_file, n_uchars, NULL, 0 );
   image_file_ascii = gan_malloc_array(char, n_chars);

   if( !image_file_ascii )
   {
      gan_err_flush_trace();
      gan_err_register_with_number( "run_test", GAN_ERROR_MALLOC_FAILED, "char[]", n_chars );
      return GAN_ERROR_MALLOC_FAILED;
   }

   if ( gan_unicodechar_to_char( image_file, n_uchars, image_file_ascii, n_chars ) == 0 )
   {
      gan_err_flush_trace();
      gan_err_register("run_test",
                       GAN_EC_FAIL,
                       "Failed to convert error message from unicode to char.");
   }

   if ( !gan_display_new_window ( img_grey->height, img_grey->width, 1.0,
                                  image_file_ascii, 0, 0, &window_id ) )
   {
      fprintf ( stderr, "cannot open create OpenGL window\n" );
      gan_free_va(image_file_ascii, NULL);
      return GAN_FALSE;
   }
   gan_free_va(image_file_ascii, NULL);

   glutDisplayFunc ( display_image );
   glutReshapeFunc ( reshape );
   glutKeyboardFunc(affiche);

   
   /* Setup the menu, which is available by right clicking on the image */
   glutCreateMenu( ModeMenu );
   glutAddMenuEntry ( "Show corners", SHOW_CORNERS );
   glutAddMenuEntry ( "Show edges",   SHOW_EDGES );
   glutAddMenuEntry ( "Show lines",   SHOW_LINES );
   glutAddMenuEntry ( "Quit",         QUIT );
   glutAttachMenu(GLUT_RIGHT_BUTTON);

   
   /* enter event handling loop */       

   printf ( "Right-click on the image to bring up the feature display menu\n");
   
   /* transfer control of flow to OpenGL event handler */
   glutMainLoop();

   /* shouldn't get here */
   return GAN_FALSE;
#else
   /* Free up resources */
   gan_line_feature_map_free ( lmap );
   gan_edge_feature_map_free ( emap );
   gan_corner_feature_map_free ( cmap );
   gan_image_free_va ( img_rgb, img_grey, NULL );

   /* success */
   return GAN_TRUE;
#endif /* #ifdef VISION_TEST_MAIN */   
}

#ifdef VISION_TEST_MAIN

int main ( int argc, char *argv[] )
{
   /* turn on error tracing */
   gan_err_set_trace ( GAN_ERR_TRACE_ON );

   glutInit(&argc, argv);
      
  
   setup_test();
   if ( run_test() )
      {
         printf ( "Tests ran successfully!\n" );
          
      }
   
   else
   {
      printf ( "At least one test failed\n" );
      printf ( "Gandalf errors:\n" );
      gan_err_default_reporter();
   }
   
   teardown_test();
   gan_heap_report(NULL);
   return 0;
}

#else

/* This function registers the unit tests to a cUnit_test_suite. */
cUnit_test_suite *vision_test_build_suite(void)
{
   cUnit_test_suite *sp;

   /* Get a new test session */
   sp = cUnit_new_test_suite("vision_test suite");

   cUnit_add_test(sp, "vision_test", run_test);

   /* register a setup and teardown on the test suite 'vision_test' */
   if (cUnit_register_setup(sp, setup_test) != GAN_TRUE)
      printf("Error while setting up test suite vision_test");

   if (cUnit_register_teardown(sp, teardown_test) != GAN_TRUE)
      printf("Error while tearing down test suite vision_test");

   return( sp );
}

#endif /* #ifdef VISION_TEST_MAIN */
