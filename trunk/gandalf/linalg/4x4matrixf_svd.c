/**
 * File:          $RCSfile: 4x4matrixf_svd.c,v $
 * Module:        4x4 matrix singular value decomposition (single precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.4 $
 * Last edited:   $Date: 2002/04/18 14:56:23 $
 * Author:        $Author: pm $
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

#include <gandalf/linalg/4x4matrixf_svd.h>
#include <gandalf/linalg/matf_gen.h>
#include <gandalf/linalg/matf_square.h>
#include <gandalf/linalg/matf_diagonal.h>
#include <gandalf/linalg/matf_svd.h>
#include <gandalf/common/misc_error.h>

/**
 * \addtogroup LinearAlgebra
 * \{
 */

/**
 * \addtogroup FixedSizeMatVec
 * \{
 */

/**
 * \addtogroup FixedSizeMatrix
 * \{
 */

/**
 * \addtogroup FixedSizeMatrixSVD
 * \{
 */

#define gel(M,i,j)   gan_matf_get_el(M,i,j)
#define sel(M,i,j,v) gan_matf_set_el(M,i,j,v)
#define sgel(M,i,j)   gan_squmatf_get_el(M,i,j)
#define ssel(M,i,j,v) gan_squmatf_set_el(M,i,j,v)

#define WORKSPACE_SIZE 200

/**
 * \brief Singular value decomposition of 4x4 matrix.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Computes the SVD of a 4x4 input matrix \a A as
 * \f[
 *    A = U \: S \: VT
 * \f]
 * where \a U are the left singular vectors, \a S contain the singular
 * values and \a VT are the (transposed) right singular vectors.
 *
 * NOTE: the contents of \a A are NOT destroyed.
 *
 * The singular values \a S are provided in decending order.
 *
 * This function calls the general SVD routine gan_matf_svd().
 */
Gan_Bool
 gan_mat44f_svd ( Gan_Matrix44_f *A,
                  Gan_Matrix44_f *U, Gan_Vector4_f *S, Gan_Matrix44_f *VT )
{
   Gan_Matrix_f    Am, Um, VTm;
   Gan_SquMatrix_f Sm;
   float           Adata[4*4], Udata[4*4], Sdata[4], VTdata[4*4],
                   wkdata[WORKSPACE_SIZE];

   /* convert to general matrices */
   if ( gan_matf_form_data ( &Am, 4, 4, Adata, 4*4 ) == NULL ||
        gan_matf_form_data ( &Um, 4, 4, Udata, 4*4 ) == NULL ||
        gan_diagmatf_form_data ( &Sm, 4, Sdata, 4 ) == NULL ||
        gan_matf_form_data ( &VTm, 4, 4, VTdata, 4*4 ) == NULL )
   {
      gan_err_register ( "gan_mat44_svd", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* copy data from matrix A into general matrix Am */
   if ( gan_matf_fill_va ( &Am, 4, 4,
                  (double)A->xx, (double)A->xy, (double)A->xz, (double)A->xw,
                  (double)A->yx, (double)A->yy, (double)A->yz, (double)A->yw,
                  (double)A->zx, (double)A->zy, (double)A->zz, (double)A->zw,
                  (double)A->wx, (double)A->wy, (double)A->wz, (double)A->ww )
        == NULL )
   {
      gan_err_register ( "gan_mat44_svd", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* perform SVD */
   if ( !gan_matf_svd ( &Am, &Um, &Sm, &VTm, wkdata, WORKSPACE_SIZE ) )
   {
      gan_err_register ( "gan_mat44_svd", GAN_ERROR_FAILURE, "" );
      return GAN_FALSE;
   }

   /* copy singular values and singular vectors */
   gan_mat44f_fill_q ( U,
                       gel(&Um,0,0), gel(&Um,0,1), gel(&Um,0,2), gel(&Um,0,3),
                       gel(&Um,1,0), gel(&Um,1,1), gel(&Um,1,2), gel(&Um,1,3),
                       gel(&Um,2,0), gel(&Um,2,1), gel(&Um,2,2), gel(&Um,2,3),
                       gel(&Um,3,0), gel(&Um,3,1), gel(&Um,3,2), gel(&Um,3,3));
   gan_vec4f_fill_q ( S, sgel(&Sm,0,0), sgel(&Sm,1,1),
                         sgel(&Sm,2,2), sgel(&Sm,3,3) );
   gan_mat44f_fill_q ( VT,
                  gel(&VTm,0,0), gel(&VTm,0,1), gel(&VTm,0,2), gel(&VTm,0,3),
                  gel(&VTm,1,0), gel(&VTm,1,1), gel(&VTm,1,2), gel(&VTm,1,3),
                  gel(&VTm,2,0), gel(&VTm,2,1), gel(&VTm,2,2), gel(&VTm,2,3),
                  gel(&VTm,3,0), gel(&VTm,3,1), gel(&VTm,3,2), gel(&VTm,3,3) );

   /* success */
   return GAN_TRUE;
}

/**
 * \brief Singular value decomposition of the transpose of a 4x4 matrix.
 * \return #GAN_TRUE on success, #GAN_FALSE on failure.
 *
 * Computes the SVD of the transpose of a 4x4 input matrix \a A as
 * \f[
 *    A^{\top} = U \: S \: VT
 * \f]
 * where \a U are the left singular vectors, \a S contain the singular
 * values and \a VT are the (transposed) right singular vectors.
 *
 * NOTE: the contents of \a A are NOT destroyed.
 *
 * The singular values \a S are provided in decending order.
 *
 * This function calls the general SVD routine gan_matf_svd().
 */
Gan_Bool
 gan_mat44Tf_svd ( Gan_Matrix44_f *A,
                   Gan_Matrix44_f *U, Gan_Vector4_f *S, Gan_Matrix44_f *VT )
{
   Gan_Matrix44_f AT;

   /* transpose matrix and apply SVD to A^T */
   (void)gan_mat44f_tpose_q ( A, &AT );
   return gan_mat44f_svd ( &AT, U, S, VT );
}

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */

/**
 * \}
 */
