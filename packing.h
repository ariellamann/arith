/*
 *      packing.h
 *      by amann04 3/9/24
 *      arith
 *
 *      Packing bitwords from different given bitfields. 
 *     
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "a2methods.h"
#include "pnm.h"
#include <math.h>
#include "bitpack.h"

void packWords(int col, int row, A2Methods_UArray2 array2, 
                                                        void *elem, void *cl);
A2Methods_UArray2 traverseToPack(A2Methods_T methods, A2Methods_UArray2 array);


void unpackWords(int col, int row, A2Methods_UArray2 array2, 
                                                        void *elem, void *cl);
A2Methods_UArray2 traverseToUnpack(A2Methods_T methods, 
                                                A2Methods_UArray2 array);
