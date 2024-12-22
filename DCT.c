#include "DCT.h"
#include "closureStruct.h"
#include "CVStruct.h"

typedef A2Methods_UArray2 A2;


/********** CvToElems ********
 *
 * Use: 
 *      Declares an array for the DCT elements structs to be stored in, 
 *      sets the closure, and calls a mapping function on the array of
 *      component videos
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2
 *      A2 array:      Array of component video structs that is being mapped 
 *                     through to be turned into differnet bit field values
 * Return: 
 *      A2 array holding the DCT element values in structs
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 CvToElems(A2Methods_T methods, A2 array)
{
        A2 elemsArray = methods->new_with_blocksize(
                                                ((methods->width(array)) / 2), 
                                                ((methods->height(array)) / 2), 
                                                sizeof(elements),
                                                1);
        struct closure elCl = {elemsArray, 0, &methods};
        methods->map_default(array, accessBlocks, &elCl);
        
        return elemsArray;
}

//WRITE FUNCTION CONTRACT//
void accessBlocks(int col, int row, A2 array2, void *elem, void *cl)
{
        assert(array2 != NULL);
        assert(cl != NULL);
        struct closure *info = cl; 
        A2Methods_T methods = *(info->methods);

        /* Checks if the cell we are in the is the top left of the block */
        if (((col % 2) == 0) && ((row % 2) == 0)) {
                elements *store = methods->at(info->copyArray, (col / 2), 
                                                                  (row / 2));
                struct compVideo *cv0 = elem;
                struct compVideo *cv1 = methods->at(array2, col + 1, row);
                struct compVideo *cv2 = methods->at(array2, col, row + 1);
                struct compVideo *cv3 = methods->at(array2, col + 1, row + 1);

                double avgPb = getAverage(cv0->Pb, cv1->Pb, cv2->Pb, cv3->Pb);
                double avgPr = getAverage(cv0->Pr, cv1->Pr, cv2->Pr, cv3->Pr);

                *store = DCT(cv0->Y, cv1->Y, cv2->Y, cv3->Y, avgPb, avgPr);
        } 

}

/********** getAverage ********
 *
 * Use: 
 *      Takes the average of four doubles and returns it. 
 * Parameters:
 *      double one: first number
 *      double two: second number
 *      double three: third number
 *      double four: fourth number
 * Return: 
 *      Average of four given doubles.
 * Expects: 
 *
 * Notes: 
 *      
 ************************/
double getAverage(double one, double two, double three, double four)
{
        return ((one + two + three + four) / 4.0);
}

/********** DCT ********
 *
 * Use: 
 *      Calculates the discrete cosine transformation for a 2x2 block 
 *      of pixels. 
 * Parameters:
 *      double Y1: First pixel's component video Y value
 *      double Y2: Second pixel's component video Y value
 *      double Y3: Third pixel's component video Y value
 *      double Y4: Fourth pixel's component video Y value
 *      double avgPb: The average of all four pixels' Pb values. 
 *      double avgPr: The average of all four pixels' Pr values. 
 * Return: 
 *      Struct holding the values from the cosine transformation. 
 * Expects: 
 *
 * Notes: 
 *      
 ************************/
struct elements DCT(double Y1, double Y2, double Y3, double Y4, double avgPb, 
                                                                double avgPr)
{        
        elements newElements;
        
        double a = (Y4 + Y3 + Y2 + Y1) / 4.0;
        double b = (Y4 + Y3 - Y2 - Y1) / 4.0;
        double c = (Y4 - Y3 + Y2 - Y1) / 4.0;
        double d = (Y4 - Y3 - Y2 + Y1) / 4.0;
        
        newElements.a = a;
        newElements.b = b;
        newElements.c = c;
        newElements.d = d;
        newElements.avgPb = avgPb;
        newElements.avgPr = avgPr;

        return newElements;
}

/********** elemsToCv ********
 *
 * Use: 
 *      Declares an array for the component video structs to be stored in, 
 *      sets the closure, and calls a mapping function on the array of
 *      quiantized values to do the reverse DCT 
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2
 *      A2 array:      Array of quantized elements that is being mapped 
 *                     through to be turned into component video
 * Return: 
 *      A2 array holding the component video values
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 elemsToCv(A2Methods_T methods, A2 array)
{
        A2 CVArray = methods->new_with_blocksize(((methods->width(array)) * 2), 
                                        ((methods->height(array)) * 2), 
                                        sizeof(compVideo), 2);

        struct closure CVcl = {CVArray, 0, &methods};
        methods->map_default(array, makeBlocks, &CVcl);

        return CVArray;
}

/********** makeBlocks ********
 *
 * Use: 
 *      This is an apply function called during mapping that converts
 *      reverse discrete cosine transformations 
 * Parameters:
 *      int col:     Integer of the column index of a pixel.
 *      int row:     Integer of a row index of a pixel. 
 *      A2 array2:   A 2D array of quantized elements that is being traversed. 
 *      void *elem:  A pointer that points to the element in the 
 *                   current column and row the mapping function is at.
 *      void *cl:    Void pointer of a closure which holds a closure struct. 
 *                   The closure struct has the array where all the newly 
 *                   created information will be stored (new structs etc.), 
 *                   the denominator of the original image, and the methods.  
 * Return: 
 *      None
 * Expects: 
 *      The closure to not be NULL, it will throw CRE if it is. 
 * Notes: 
 *      
 ************************/
void makeBlocks(int col, int row, A2 array2, void *elem, void *cl)
{
        assert(array2 != NULL);
        assert(cl != NULL);
        struct closure *info = cl; 
        A2Methods_T methods = *(info->methods);

        elements *el = elem;
        
        double Y1 = ((double)el->a) - ((double)el->b) - ((double)el->c) + 
                                                        ((double)el->d); 
        double Y2 = ((double)el->a) - ((double)el->b) + ((double)el->c) - 
                                                        ((double)el->d); 
        double Y3 = ((double)el->a) + ((double)el->b) - ((double)el->c) - 
                                                        ((double)el->d); 
        double Y4 = ((double)el->a) + ((double)el->b) + ((double)el->c) + 
                                                        ((double)el->d);

        compVideo cv1 = {Y1, el->avgPb, el->avgPr};
        compVideo cv2 = {Y2, el->avgPb, el->avgPr};
        compVideo cv3 = {Y3, el->avgPb, el->avgPr};
        compVideo cv4 = {Y4, el->avgPb, el->avgPr};

        int c = col * 2;
        int r = row * 2;
        compVideo *cv1pt = methods->at(info->copyArray, c, r);
        *cv1pt = cv1;
        compVideo *cv2pt = methods->at(info->copyArray, c + 1, r);
        *cv2pt = cv2;
        compVideo *cv3pt = methods->at(info->copyArray, c, r + 1);
        *cv3pt = cv3;
        compVideo *cv4pt = methods->at(info->copyArray, c + 1, r + 1);
        *cv4pt = cv4;
}