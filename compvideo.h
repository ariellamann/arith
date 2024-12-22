/*
 *      compvideo.h
 *      by amann04 3/9/24
 *      arith
 *
 *      Creates and undoes component video information from RGB pixels.  
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


A2Methods_UArray2 intToDouble(A2Methods_T methods, Pnm_ppm image);
void doubleAndBlock(int col, int row, A2Methods_UArray2 array2, 
                                        void *elem, void *cl);
A2Methods_UArray2 doubleToCv(A2Methods_T methods, A2Methods_UArray2 array);
void makeComponentVideo(int col, int row, A2Methods_UArray2 array2, 
                                        void *elem, void *cl);

A2Methods_UArray2 doubleToInt(A2Methods_T methods, A2Methods_UArray2 array);
void reverseDouble(int col, int row, A2Methods_UArray2 array2, 
                                                void *elem, void *cl);
A2Methods_UArray2 CvToDouble(A2Methods_T methods, A2Methods_UArray2 array);
void fromComponentVideo(int col, int row, A2Methods_UArray2 array2, 
                                                void *elem, void *cl);
