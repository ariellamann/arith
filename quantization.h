/*
 *      quantization.h
 *      by amann04 3/9/24
 *      arith
 *
 *      Quantizes elements in order to be bitpacked. 
 *     
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "a2methods.h"
#include "pnm.h"
#include <math.h>
#include "arith40.h"

A2Methods_UArray2 quantize(A2Methods_T methods, A2Methods_UArray2 array);
void quantizeElems(int col, int row, A2Methods_UArray2 array2, 
                                                        void *elem, void *cl);
double quantNums(double num);
int roundNum(double num);

A2Methods_UArray2 unquantize(A2Methods_T methods, A2Methods_UArray2 array);
void unquantizeElems(int col, int row, A2Methods_UArray2 array2, 
                                                        void *elem, void *cl);