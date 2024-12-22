#include "compvideo.h"
#include "CVStruct.h"
#include "RGBstruct.h"
#include "closureStruct.h"

typedef A2Methods_UArray2 A2;

/********** intToDouble ********
 *
 * Use: 
 *      Declares an array for the RGB structs to be stored in, sets the 
 *      closure, and calls a mapping function on the array of Pnm_rgbs
 * Parameters:
 *      A2Methods_T methods: Methods being used on the A2
 *      Pnm_ppm image: Array of Pnm_rgbs that was read in an is being mapped
 *                     through to be turned to float values 
 * Return: 
 *      A2 array holding the float RGB structs
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 intToDouble(A2Methods_T methods, Pnm_ppm image) 
{
        A2 copyArray = methods->new(image->width, image->height, 
                                                        sizeof(RGB)); 
        struct closure doubleCl = {copyArray, image->denominator, &methods};
        methods->map_default(image->pixels, doubleAndBlock, &doubleCl);

        return copyArray;
}

/********** doubleAndBlock ********
 *
 * Use: 
 *      This is an apply function called during mapping that converts
 *      Pnm_rgb values into doubles, stores them into a struct of doubles. 
 * Parameters:
 *      int col:     Integer of the column index of a pixel.
 *      int row:     Integer of a row index of a pixel.
 *      A2 array2:   A distinct 2D array of memory cells that 
 *                   represents the pixels in an image.
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
void doubleAndBlock(int col, int row, A2 array2, void *elem, void *cl)
{
        (void) array2; 
        assert(cl != NULL);
        struct closure *info = cl; 
        A2Methods_T methods = *(info->methods);

        RGB *pixPointer = methods->at(info->copyArray, col, row);
        RGB newPixel;
        Pnm_rgb pixel = elem;
        
        newPixel.red = ((double)(pixel->red)) / ((double)(info->denom));
        newPixel.green = ((double)(pixel->green)) / ((double)(info->denom));
        newPixel.blue = ((double)(pixel->blue)) / ((double)(info->denom));
        
        *pixPointer = newPixel;
} 


/********** doubleToCv ********
 *
 * Use: 
 *      Declares an array for the component video structs to be stored in, 
 *      sets the closure, and calls a mapping function on the array of RGBs
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2
 *      A2 array:      Array of RGBs that is being mapped through to be
 *                     turned into component video values
 * Return: 
 *      A2 array holding the component video values as structs
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 doubleToCv(A2Methods_T methods, A2 array)
{

        A2 CVArray = methods->new(methods->width(array), 
                                    methods->height(array), sizeof(compVideo));
        struct closure CVcl = {CVArray, 255, &methods};
        methods->map_default(array, makeComponentVideo, &CVcl);

        return CVArray;
}


/********** makeComponentVideo ********
 *
 * Use: 
 *      This is an apply function called during mapping that converts
 *      RGB double values into their component video values. 
 * Parameters:
 *      int col:     Integer of the column index of a pixel.
 *      int row:     Integer of a row index of a pixel.
 *      A2 array2:   A 2D array of RGB structs that is being traversed. 
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
void makeComponentVideo(int col, int row, A2 array2, void *elem, void *cl)
{
        assert(array2 != NULL);
        assert(cl != NULL);
        struct closure *info = cl; 
        A2Methods_T methods = *(info->methods);

        compVideo *cvPointer = methods->at(info->copyArray, col, row);
        RGB *pixel = elem;
        compVideo CV;

        /* Computes component video */
        CV.Y = 0.299 * pixel->red + 0.587 * pixel->green + 0.114 * pixel->blue;
        CV.Pb = -0.168736 * pixel->red - 0.331264 * pixel->green + 0.5 * 
                                                                   pixel->blue;
        CV.Pr = 0.5 * pixel->red - 0.418688 * pixel->green - 0.081312 * 
                                                                   pixel->blue;
        *cvPointer = CV;
}


/********** doubleToInt ********
 *
 * Use: 
 *      Declares an array for the Pnm_rgb structs to be stored in, sets the 
 *      closure, and calls a mapping function on the array of RGBs
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2
 *      A2 array:      Array of RGBs that is being mapped through to be
 *                     turned to int values for Pnm_rgb
 * Return: 
 *      A2 array holding the float Pnm_rgb structs
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 doubleToInt(A2Methods_T methods, A2 array)
{
        A2 copyArray = methods->new_with_blocksize(methods->width(array), 
                               methods->height(array), sizeof(struct Pnm_rgb), 1); 
        struct closure doubleCl2 = {copyArray, 255, &methods};
        methods->map_default(array, reverseDouble, &doubleCl2);

        return copyArray;
}

/********** reverseDouble ********
 *
 * Use: 
 *      This is an apply function called during mapping that converts
 *      float versions of rgb values into ints and stores them into a Pnm_rgb
 *      wh
 * Parameters:
 *      int col:     Integer of the column index of a pixel.
 *      int row:     Integer of a row index of a pixel.
 *      A2 array2:   A 2D array of RGB structs that is being traversed. 
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
void reverseDouble(int col, int row, A2 array2, void *elem, void *cl)
{       
        (void) array2;
        assert(cl != NULL);
        struct closure *info = cl; 
        A2Methods_T methods = *(info->methods);

        Pnm_rgb pixPointer = methods->at(info->copyArray, col, row);
        assert(pixPointer != NULL);
        RGB *pixel = elem;
        struct Pnm_rgb newPixel = { .red = (pixel->red * info->denom), 
                                    .green = (pixel->green * info->denom), 
                                    .blue = (pixel->blue * info->denom)
                                    };        
        *pixPointer = newPixel;
}


/********** CvToDouble ********
 *
 * Use: 
 *      Declares an array for the double RGB structs to be stored in, 
 *      sets the closure, and calls a mapping function on the array of
 *      component videos
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2
 *      A2 array:      Array of component video structs that is being mapped 
 *                     through to be turned into RGB double values
 * Return: 
 *      A2 array holding the RGB double values in structs
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 CvToDouble(A2Methods_T methods, A2 array)
{

        A2 doubleArray = methods->new(methods->width(array), 
                               methods->height(array), sizeof(struct RGB)); 
        struct closure doubleCl = {doubleArray, 255, &methods};
        methods->map_default(array, fromComponentVideo, &doubleCl);

        return doubleArray;

}

/********** fromComponentVideo ********
 *
 * Use: 
 *      This is an apply function called during mapping that converts
 *      component video values into their RGB double values. 
 * Parameters:
 *      int col:     Integer of the column index of a pixel.
 *      int row:     Integer of a row index of a pixel.
 *      A2 array2:   A 2D array of compVideo structs that is being traversed. 
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
void fromComponentVideo(int col, int row, A2 array2, void *elem, void *cl)
{
        assert(array2 != NULL);
        assert(cl != NULL);
        struct closure *info = cl; 
        A2Methods_T methods = *(info->methods);

        RGB *pixel = methods->at(info->copyArray, col, row);
        compVideo *CV =  elem;
        RGB newPixel;

        newPixel.red = 1.0 * CV->Y + 0.0 * CV->Pb + 1.402 * CV->Pr;
        newPixel.green = 1.0 * CV->Y - 0.344136 * CV->Pb - 0.714136 * CV->Pr;
        newPixel.blue = 1.0 * CV->Y + 1.772 * CV->Pb + 0.0 * CV->Pr;

        if (newPixel.red < 0){
                newPixel.red = fabs(newPixel.red);
        } else if (newPixel.red > 1){
                newPixel.red = 1;
        }
        if (newPixel.green < 0){
                newPixel.green = fabs(newPixel.green);
        } else if (newPixel.green > 1){
                newPixel.green = 1;
        }
        if (newPixel.blue < 0 ){
                newPixel.blue = fabs(newPixel.blue);
        } else if (newPixel.blue > 1){
                newPixel.blue = 1;
        }

        *pixel = newPixel;
}
