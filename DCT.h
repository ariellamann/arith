/*
 *      DCT.h
 *      by amann04 3/9/24
 *      arith
 *
 *      Discrete cosine transformation and reverse discrete cosine 
 *      transformation.
 *     
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "a2methods.h"
#include "pnm.h"
#include <math.h>
#include "floatsStruct.h"
#include "arith40.h"

A2Methods_UArray2 CvToElems(A2Methods_T methods, A2Methods_UArray2 array);
void accessBlocks(int col, int row, A2Methods_UArray2 array2, 
                                                    void *elem, void *cl);
elements DCT(double Y1, double Y2, double Y3, double Y4, double avgPb, 
                                                                double avgPr);
double getAverage(double one, double two, double three, double four);

A2Methods_UArray2 elemsToCv(A2Methods_T methods, A2Methods_UArray2 array);
void makeBlocks(int col, int row, A2Methods_UArray2 array2, 
                                                    void *elem, void *cl);
