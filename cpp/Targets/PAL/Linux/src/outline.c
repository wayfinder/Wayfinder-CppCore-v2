/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "outline.h"

/**
 * reverse the direction of the outline so it can be subtracted
 *
 * @param  aOutlineIn  outline to be reversed
 * @param  aOutlineOut reversed ouline
 * @return             none
 */
static void ft_OutlineReverse( FT_Outline *aOutlineIn,
                               FT_Outline *aOutlineOut )
{
   int i;

   /* Reverse the points */
   for ( i = 0 ; i < aOutlineIn->n_points; i++ ) {
      aOutlineOut->points[ i ] = aOutlineIn->points[ aOutlineIn->n_points - i - 1 ];
   }

   /* Reverse the tags */
   for ( i = 0 ; i < aOutlineIn->n_points; i++ ) {
      aOutlineOut->tags[ i ] = aOutlineIn->tags[ aOutlineIn->n_points - i - 1 ];
   }

   /* Reverse the contours */
   for ( i = 0; i < aOutlineIn->n_contours - 1; i++ ) {
      aOutlineOut->contours[ i ] = aOutlineIn->n_points - aOutlineIn->contours[ aOutlineIn->n_contours - i - 2 ] - 2;
   }
}

/**
 * merge the two source outlines into one destination outline
 * destination must be preallocated the right size
 *
 * @param  aOutlineIn  outline to be reversed
 * @param  aOutlineOut reversed ouline
 * @return             none
 */
static void ft_OutlineMerge( FT_Outline *Src1,
                             FT_Outline *Src2,
                             FT_Outline *Dest )
{
   int i;

   /* Copy Src1 points */
   for ( i = 0; i < Src1->n_points; i++ ) {
      Dest->points[ i ] = Src1->points[ i ];
   }

   /* Copy Src2 points */
   for ( i = 0; i < Src2->n_points; i++ ) {
      Dest->points[ Src1->n_points + i ] = Src2->points[ i ];
   }

   /* Copy Src1 tags */
   for ( i = 0; i < Src1->n_points; i++ ) {
      Dest->tags[ i ] = Src1->tags[ i ];
   }

   /* Copy Src2 tags */
   for ( i = 0; i < Src2->n_points; i++ ) {
      Dest->tags[ Src1->n_points + i ] = Src2->tags[ i ];
   }

   /* Copy Src1 contours */
   for ( i = 0; i < Src1->n_contours; i++ ) {
      Dest->contours[ i ] = Src1->contours[ i ];
   }

   /* Copy Src2 contours */
   for ( i = 0; i < Src2->n_contours; i++ ) {
      Dest->contours[ Src1->n_contours + i ] = Src1->n_points + Src2->contours[ i ];
   }

   Dest->n_contours = Src1->n_contours + Src2->n_contours;
   Dest->n_points   = Src1->n_points   + Src2->n_points;
}

FT_Error ft_OutlineStroke( FT_Library   library,
                           FT_Outline  *Outline,
                           int          Thickness )
{
  FT_Error   err = 0;
  FT_Outline OutlineReversed;
  FT_Outline OutlineFattened;
  FT_Outline OutlineStroke;

  if ( Outline == NULL ) {
     goto failure;
  }

  err = FT_Outline_New( library,
                        Outline->n_points,
                        Outline->n_contours,
                        &OutlineReversed );
  if ( err != 0 ) {
     goto failure;
  }

  err = FT_Outline_New( library,
                        Outline->n_points,
                        Outline->n_contours,
                        &OutlineFattened );
  if ( err != 0 ) {
     goto failure;
  }

  err = FT_Outline_Copy( Outline, &OutlineReversed );
  if ( err != 0 ) {
     goto failure;
  }

  err = FT_Outline_Copy( Outline, &OutlineFattened );
  if ( err != 0 ) {
     goto failure;
  }

  err = FT_Outline_New( library,
                        Outline->n_points   * 2,
                        Outline->n_contours * 2,
                        &OutlineStroke );
  if ( err != 0 ) {
     goto failure;
  }

  /* Perform fattening operation */
  err = FT_Outline_Embolden( &OutlineFattened, Thickness << 1 );
  if ( err != 0 ) {
     goto failure;
  }

  /* Perform reversal operation */
  ft_OutlineReverse( Outline,
                     &OutlineReversed );

  FT_Outline_Translate( &OutlineReversed, Thickness, Thickness );

  /* Merge outlines */
  ft_OutlineMerge( &OutlineFattened,
                   &OutlineReversed,
                   &OutlineStroke );

  /* delete temporary and input outline */
  err = FT_Outline_Done( library, &OutlineReversed );
  if ( err != 0 ) {
     goto failure;
  }

  err = FT_Outline_Done( library, &OutlineFattened );
  if ( err != 0 ) {
     goto failure;
  }

  err = FT_Outline_Done( library, Outline );
  if ( err != 0 ) {
     goto failure;
  }

  /* finally copy the outline - its not clear from ft docs if this
     does the right thing but i _think_ its correct */
  memcpy( Outline, &OutlineStroke, sizeof( FT_Outline ) );

  return 0;

failure:

   return err;
}
