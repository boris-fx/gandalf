/**
 * File:          $RCSfile: 4x4symmatf_noc.h,v $
 * Module:        4x4 symmetric matrices (single precision)
 * Part of:       Gandalf Library
 *
 * Revision:      $Revision: 1.23 $
 * Last edited:   $Date: 2005/08/22 08:52:18 $
 * Author:        $Author: jps $
 * Copyright:     (c) 2000 Imagineer Software Limited
 *
 * Notes:         Not to be compiled separately
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

#ifndef _GAN_4X4SYMMATF_H

/**
 * \addtogroup FixedSizeMatrixFill
 * \{
 */

/**
 * \brief Macro: Fill 4x4 symmetric matrix with values.
 *
 * Fill 4x4 symmetric matrix \a A with values:
 * \f[ A = \left(\begin{array}{cccc} XX & YX & ZX & WX \\ YX & YY & ZY & WY \\
 *                                   ZX & ZY & ZZ & ZW \\ WX & WY & WZ & WW
 *         \end{array}\right)
 * \f]
 */
#ifdef GAN_GENERATE_DOCUMENTATION
GANDALF_API Gan_SquMatrix44_f *
    gan_symmat44f_fill_q ( Gan_SquMatrix44_f *A,
                           float XX,
                           float YX, float YY,
                           float ZX, float ZY, float ZZ,
                           float WX, float WY, float WZ, float WW );
#else
#define gan_symmat44f_fill_q(A,XX,YX,YY,ZX,ZY,ZZ,WX,WY,WZ,WW) (GAN_ST44F_FILL(A,GAN_SYMMETRIC_MATRIX44_F,XX,YX,YY,ZX,ZY,ZZ,WX,WY,WZ,WW),A)
#endif

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixExtract
 * \{
 */

/**
 * \brief Macro: Extract parts of 4x4 symmetric matrix.
 *
 * Extract top-left 3x3 part, bottom-left 1x3 part and bottom-right element
 * of 4x4 symmetric matrix \a A into 3x3 lower triangular matrix \a B,
 * row 3-vector \a p and scalar pointer \a s. The arrangement is
 * \f[
 *     A = \left(\begin{array}{cc} B & p \\ p^{\top} & s \end{array}\right)
 * \f]
 *
 * \return No value.
 */
#ifdef GAN_GENERATE_DOCUMENTATION
GANDALF_API void gan_symmat44f_get_parts_q ( const Gan_SquMatrix44_f *A,
                                 Gan_SquMatrix33_f *B, Gan_Vector3_f *p,
                                 float *s);
#else
#define gan_symmat44f_get_parts_q(A,B,p,s)\
           (GAN_TEST_OP3(A,B,p,\
                         Gan_SquMatrix44_f,Gan_SquMatrix33_f,Gan_Vector3_f),\
            assert((A)->type == GAN_SYMMETRIC_MATRIX44_F),\
            gan_eval((B)->type = GAN_SYMMETRIC_MATRIX33_F),\
            (B)->xx=(A)->xx,\
            (B)->yx=(A)->yx,(B)->yy=(A)->yy,\
            (B)->zx=(A)->zx,(B)->zy=(A)->zy,(B)->zz=(A)->zz,\
            (p)->x=(A)->wx,(p)->y=(A)->wy,(p)->z=(A)->wz,*(s)=(A)->ww)
#endif

/**
 * \brief Macro: Extract 2x2 block parts of 4x4 symmetric matrix.
 *
 * Extract top-left 2x2, top-right 2x2 (or transposed bottom-left 2x2) and
 * bottom-right 2x2 blocks of 4x4 symmetric matrix \a A into 2x2 symmetric
 * matrix \a B, 2x2 generic matrix \a C and 2x2 symmetric matrix \a D.
 * The arrangement is
 * \f[
 *     A = \left(\begin{array}{cc} B & C \\ C^{\top} & D \end{array}\right)
 * \f]
 *
 * \return No value.
 */
#ifdef GAN_GENERATE_DOCUMENTATION
GANDALF_API void gan_symmat44f_get_blocks_q ( const Gan_SquMatrix44_f *A,
                                  Gan_SquMatrix22_f *B, Gan_Matrix22_f *C,
                                  Gan_SquMatrix22_f *D );
#else
#define gan_symmat44f_get_blocks_q(A,B,C,D)\
           (GAN_TEST_OP4(A,B,C,D,Gan_SquMatrix44_f,Gan_SquMatrix22_f,\
                                 Gan_Matrix22_f,Gan_SquMatrix22_f),\
            assert((A)->type == GAN_SYMMETRIC_MATRIX44_F),\
            gan_eval((B)->type = GAN_SYMMETRIC_MATRIX22_F),\
            gan_eval((D)->type = GAN_SYMMETRIC_MATRIX22_F),\
            (B)->xx=(A)->xx,\
            (B)->yx=(A)->yx,(B)->yy=(A)->yy,\
            (C)->xx=(A)->zx,(C)->yx=(A)->zy,(D)->xx=(A)->zz,\
            (C)->xy=(A)->wx,(C)->yy=(A)->wy,(D)->yx=(A)->wz,(D)->yy=(A)->ww)
#endif

/**
 * \}
 */

/**
 * \addtogroup FixedSizeMatrixBuild
 * \{
 */

/**
 * \brief Macro: Build a 4x4 symmetric matrix from parts.
 *
 * Build 4x4 symmetric matrix \a A from 3x3 symmetric matrix
 * \a B, 3-vector \a p, and scalar \a s, which are respectively inserted
 * in the top-left, bottom-left/top-right and bottom-right parts of the 4x4
 * matrix. The arrangement of the matrix is
 * \f[
 *    A = \left(\begin{array}{cc} B & p \\ p^{\top} & s \end{array}\right)
 * \f]
 *
 * \return Pointer to filled matrix \a A.
 */
#ifdef GAN_GENERATE_DOCUMENTATION
GANDALF_API Gan_SquMatrix44_f *gan_symmat44f_set_parts_q ( Gan_SquMatrix44_f *A,
                                               const Gan_SquMatrix33_f *B,
                                               const Gan_Vector3_f *p, float s );
#else
#define gan_symmat44f_set_parts_q(A,B,p,s)\
           (GAN_TEST_OP3(A,B,p,Gan_SquMatrix44_f,Gan_SquMatrix33_f,Gan_Vector3_f),\
            assert((B)->type == GAN_SYMMETRIC_MATRIX33_F),\
            gan_eval((A)->type = GAN_SYMMETRIC_MATRIX44_F),\
            (A)->xx=(B)->xx,\
            (A)->yx=(B)->yx,(A)->yy=(B)->yy,\
            (A)->zx=(B)->zx,(A)->zy=(B)->zy,(A)->zz=(B)->zz,\
            (A)->wx=(p)->x,(A)->wy=(p)->y,(A)->wz=(p)->z,(A)->ww=(s),A)
#endif

/**
 * \brief Macro: Build a 4x4 symmetric matrix from 2x2 blocks.
 *
 * Build 4x4 symmetric matrix \a A from 2x2 symmetric matrix \a B, generic
 * 2x2 matrix \a C and 2x2 symmetric matrix \a D, which are respectively
 * inserted in the top-left, top-right (or transposed bottom-left) and
 * bottom-right parts of \a A. The arrangement is
 * \f[
 *   A = \left(\begin{array}{cc} B & C \\ C^{\top} & D \end{array}\right)
 * \f]
 *
 * \return Pointer to filled matrix \a A.
 */
#ifdef GAN_GENERATE_DOCUMENTATION
GANDALF_API Gan_SquMatrix44_f *gan_symmat44f_set_blocks_q ( Gan_SquMatrix44_f *A,
                                                const Gan_SquMatrix22_f *B,
                                                const Gan_Matrix22_f *C,
                                                const Gan_SquMatrix22_f *D );
#else
#define gan_symmat44f_set_blocks_q(A,B,C,D)\
           (GAN_TEST_OP4(A,B,C,D,Gan_SquMatrix44_f,Gan_SquMatrix22_f,\
                                 Gan_Matrix22_f,Gan_SquMatrix22_f),\
            assert((B)->type == GAN_SYMMETRIC_MATRIX22_F),\
            assert((D)->type == GAN_SYMMETRIC_MATRIX22_F),\
            gan_eval((A)->type = GAN_SYMMETRIC_MATRIX44_F),\
            (A)->xx=(B)->xx,\
            (A)->yx=(B)->yx,(A)->yy=(B)->yy,\
            (A)->zx=(C)->xx,(A)->zy=(C)->yx,(A)->zz=(D)->xx,\
            (A)->wx=(C)->xy,(A)->wy=(C)->yy,(A)->wz=(D)->yx,(A)->ww=(D)->yy)
#endif

/**
 * \}
 */

#ifndef GAN_GENERATE_DOCUMENTATION

GANDALF_API Gan_SquMatrix44_f gan_symmat44f_fill_s (
                          float XX,
                          float YX, float YY,
                          float ZX, float ZY, float ZZ,
                          float WX, float WY, float WZ, float WW );
GANDALF_API void gan_symmat44f_get_parts_s ( const Gan_SquMatrix44_f *A,
                                 Gan_SquMatrix33_f *B, Gan_Vector3_f *p,
                                 float *s );
GANDALF_API void gan_symmat44f_get_blocks_s ( const Gan_SquMatrix44_f *A,
                                  Gan_SquMatrix22_f *B, Gan_Matrix22_f *C,
                                  Gan_SquMatrix22_f *D );
GANDALF_API Gan_SquMatrix44_f gan_symmat44f_set_parts_s ( const Gan_SquMatrix33_f *B,
                                              const Gan_Vector3_f *p, float s );
GANDALF_API Gan_SquMatrix44_f gan_symmat44f_set_blocks_s ( const Gan_SquMatrix22_f *B,
                                               const Gan_Matrix22_f *C,
                                               const Gan_SquMatrix22_f *D );
GANDALF_API Gan_Matrix44_f *gan_symmat44f_to_mat44f_q ( const Gan_SquMatrix44_f *A,
                                            Gan_Matrix44_f *B );
GANDALF_API Gan_Matrix44_f  gan_symmat44f_to_mat44f_s ( const Gan_SquMatrix44_f *A );

#endif /* #ifndef GAN_GENERATE_DOCUMENTATION */

#endif /* #ifndef _GAN_4X4SYMMATF_H */

#define GAN_MATTYPE Gan_SquMatrix44_f
#define GAN_MATRIX_TYPE Gan_SquMatrix_f
#define GAN_SQUMATRIX_TYPE Gan_SquMatrix_f
#define GAN_VECTOR_TYPE Gan_Vector_f
#define GAN_MAT_ELEMENT_TYPE GAN_FLOAT
#define GAN_REALTYPE float
#define GAN_FWRITE_LENDIAN gan_fwrite_lendian_f32
#define GAN_FREAD_LENDIAN  gan_fread_lendian_f32
#define GAN_VECTYPE1 Gan_Vector4_f
#define GAN_VECTYPE2 Gan_Vector4_f
#define GAN_FIXED_MATRIX_TYPE GAN_SYMMETRIC_MATRIX44_F
#define GAN_MAT_FPRINT      gan_symmat44f_fprint
#define GAN_MAT_PRINT       gan_symmat44f_print
#define GAN_MAT_FSCANF      gan_symmat44f_fscanf
#define GAN_MAT_FWRITE      gan_symmat44f_fwrite
#define GAN_MAT_FREAD       gan_symmat44f_fread
#define GAN_MAT_ZERO_Q      gan_symmat44f_zero_q
#define GAN_MAT_ZERO_S      gan_symmat44f_zero_s
#define GAN_MAT_COPY_Q      gan_symmat44f_copy_q
#define GAN_MAT_COPY_S      gan_symmat44f_copy_s
#define GAN_MAT_SCALE_Q     gan_symmat44f_scale_q
#define GAN_MAT_SCALE_S     gan_symmat44f_scale_s
#define GAN_MAT_SCALE_I     gan_symmat44f_scale_i
#define GAN_MAT_DIVIDE_Q    gan_symmat44f_divide_q
#define GAN_MAT_DIVIDE_S    gan_symmat44f_divide_s
#define GAN_MAT_DIVIDE_I    gan_symmat44f_divide_i
#define GAN_MAT_NEGATE_Q    gan_symmat44f_negate_q
#define GAN_MAT_NEGATE_S    gan_symmat44f_negate_s
#define GAN_MAT_NEGATE_I    gan_symmat44f_negate_i
#define GAN_MAT_UNIT_Q      gan_symmat44f_unit_q
#define GAN_MAT_UNIT_S      gan_symmat44f_unit_s
#define GAN_MAT_UNIT_I      gan_symmat44f_unit_i
#define GAN_MAT_ADD_Q       gan_symmat44f_add_q
#define GAN_MAT_ADD_I1      gan_symmat44f_add_i1
#define GAN_MAT_ADD_I2      gan_symmat44f_add_i2
#define GAN_MAT_INCREMENT   gan_symmat44f_increment
#define GAN_MAT_ADD_S       gan_symmat44f_add_s
#define GAN_MAT_SUB_Q       gan_symmat44f_sub_q
#define GAN_MAT_SUB_I1      gan_symmat44f_sub_i1
#define GAN_MAT_SUB_I2      gan_symmat44f_sub_i2
#define GAN_MAT_DECREMENT   gan_symmat44f_decrement
#define GAN_MAT_SUB_S       gan_symmat44f_sub_s
#define GAN_MAT_MULTV_Q     gan_symmat44f_multv4_q
#define GAN_MAT_MULTV_S     gan_symmat44f_multv4_s
#define GAN_MAT_SUMSQR_Q    gan_symmat44f_sumsqr_q
#define GAN_MAT_SUMSQR_S    gan_symmat44f_sumsqr_s
#define GAN_MAT_FNORM_Q     gan_symmat44f_Fnorm_q
#define GAN_MAT_FNORM_S     gan_symmat44f_Fnorm_s

#define GAN_SQUMAT_IDENT_Q    gan_symmat44f_ident_q
#define GAN_SQUMAT_IDENT_S    gan_symmat44f_ident_s
#define GAN_SQUMAT_INVERT     gan_symmat44f_invert
#define GAN_SQUMAT_INVERT_Q   gan_symmat44f_invert_q
#define GAN_SQUMAT_INVERT_S   gan_symmat44f_invert_s
#define GAN_SQUMAT_INVERT_I   gan_symmat44f_invert_i
#define GAN_SQUMAT_DET_Q      gan_symmat44f_det_q
#define GAN_SQUMAT_DET_S      gan_symmat44f_det_s
#define GAN_SQUMAT_TRACE_Q    gan_symmat44f_trace_q
#define GAN_SQUMAT_TRACE_S    gan_symmat44f_trace_s

#define GAN_VEC_OUTER_SYM_Q gan_vec44f_outer_sym_q
#define GAN_VEC_OUTER_SYM_S gan_vec44f_outer_sym_s
#define GAN_SYMMAT_CHOLESKY   gan_symmat44f_cholesky
#define GAN_SYMMAT_CHOLESKY_Q gan_symmat44f_cholesky_q
#define GAN_SYMMAT_CHOLESKY_S gan_symmat44f_cholesky_s
#define GAN_SYMMAT_CHOLESKY_I gan_symmat44f_cholesky_i

#ifndef _GAN_4X4SYMMATF_H
#define _GAN_4X4SYMMATF_H

#ifndef GAN_GENERATE_DOCUMENTATION
#define gan_symmat44f_zero_q(A) (GAN_ST44F_ZERO(A,GAN_SYMMETRIC_MATRIX44_F),A)
#define gan_symmat44f_ident_q(A) (GAN_ST44F_IDENT(A,GAN_SYMMETRIC_MATRIX44_F),A)
#define gan_symmat44f_copy_q(A,B) (GAN_ST44F_COPY(A,B,GAN_SYMMETRIC_MATRIX44_F),B)
#define gan_symmat44f_scale_q(A,a,B) (GAN_ST44F_SCALE(A,a,B,GAN_SYMMETRIC_MATRIX44_F),B)
#define gan_symmat44f_scale_i(A,a) gan_symmat44f_scale_q(A,a,A)
#define gan_symmat44f_divide_q(A,a,B) ((a)==0.0 ? NULL :\
                                      (GAN_ST44F_DIVIDE(A,a,B,GAN_SYMMETRIC_MATRIX44_F),B))
#define gan_symmat44f_divide_i(A,a) gan_symmat44f_divide_q(A,a,A)
#define gan_symmat44f_negate_q(A,B) (GAN_ST44F_NEGATE(A,B,GAN_SYMMETRIC_MATRIX44_F),B)
#define gan_symmat44f_negate_i(A) gan_symmat44f_negate_q(A,A)
#define gan_symmat44f_unit_i(A) gan_symmat44f_unit_q(A,A)
#define gan_symmat44f_cholesky_q(A,B) gan_symmat44f_cholesky(A,B,NULL)
#define gan_symmat44f_cholesky_i(A) gan_symmat44f_cholesky_q(A,A)
#define gan_symmat44f_invert_q(A,B) gan_symmat44f_invert(A,B,NULL)
#define gan_symmat44f_invert_i(A)   gan_symmat44f_invert(A,A,NULL)
#define gan_symmat44f_add_q(A,B,C) (GAN_ST44F_ADD(A,B,C,GAN_SYMMETRIC_MATRIX44_F),C)
#define gan_symmat44f_add_i1(A,B) gan_symmat44f_add_q(A,B,A)
#define gan_symmat44f_add_i2(A,B) gan_symmat44f_add_q(A,B,B)
#define gan_symmat44f_increment(A,B) gan_symmat44f_add_i1(A,B)
#define gan_symmat44f_sub_q(A,B,C) (GAN_ST44F_SUB(A,B,C,GAN_SYMMETRIC_MATRIX44_F),C)
#define gan_symmat44f_sub_i1(A,B) gan_symmat44f_sub_q(A,B,A)
#define gan_symmat44f_sub_i2(A,B) gan_symmat44f_sub_q(A,B,B)
#define gan_symmat44f_decrement(A,B) gan_symmat44f_sub_i1(A,B)
#define gan_vec44f_outer_sym_q(p,A)\
 (GAN_TEST_OP2(p,A,Gan_Vector4_f,Gan_SquMatrix44_f),\
  gan_eval((A)->type = GAN_SYMMETRIC_MATRIX44_F),\
  GAN_REP1_ABC((A)->,=(p)->,*(p)->,xx,x,x),\
  GAN_REP2_ABC((A)->,=(p)->,*(p)->,yx,yy,y,y,x,y),\
  GAN_REP3_ABC((A)->,=(p)->,*(p)->,zx,zy,zz,z,z,z,x,y,z),\
  GAN_REP4_ABC((A)->,=(p)->,*(p)->,wx,wy,wz,ww,w,w,w,w,x,y,z,w),A)
#define gan_symmat44f_multv4_q(A,p,q)\
 (GAN_TEST_OP3(A,p,q,Gan_SquMatrix44_f,Gan_Vector4_f,Gan_Vector4_f),\
  assert((p)!=(q) && (A)->type == GAN_SYMMETRIC_MATRIX44_F),\
  GAN_SYM44F_MULT1D((q)->,(A)->,(p)->,x,y,z,w,x,y,z,w),q)
#define gan_symmat44f_det_q(A) (assert(0),0.0F)
#define gan_symmat44f_trace_q(A) (GAN_TEST_OP1(A,Gan_SquMatrix44_f),\
                                 assert((A)->type==GAN_SYMMETRIC_MATRIX44_F),\
                                 ((A)->xx + (A)->yy + (A)->zz + (A)->ww))
#define gan_symmat44f_sumsqr_q(A) (GAN_TEST_OP1(A,Gan_SquMatrix44_f),\
                                  assert((A)->type==GAN_SYMMETRIC_MATRIX44_F),\
 ((A)->xx*(A)->xx + (A)->yy*(A)->yy + (A)->zz*(A)->zz + (A)->ww*(A)->ww +\
  2.0*((A)->yx*(A)->yx + (A)->zx*(A)->zx + (A)->zy*(A)->zy +\
       (A)->wx*(A)->wx + (A)->wy*(A)->wy + (A)->wz*(A)->wz)))
#define gan_symmat44f_Fnorm_q(A) (GAN_TEST_OP1(A,Gan_SquMatrix44_f),\
                                 assert((A)->type==GAN_SYMMETRIC_MATRIX44_F),\
 sqrt((A)->xx*(A)->xx + (A)->yy*(A)->yy + (A)->zz*(A)->zz + (A)->ww*(A)->ww +\
      2.0*((A)->yx*(A)->yx + (A)->zx*(A)->zx + (A)->zy*(A)->zy +\
           (A)->wx*(A)->wx + (A)->wy*(A)->wy + (A)->wz*(A)->wz)))
#endif /* #ifndef GAN_GENERATE_DOCUMENTATION */

#endif /* #ifndef _GAN_4X4SYMMATF_H */
