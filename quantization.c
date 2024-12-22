#include "quantization.h"
#include "closureStruct.h"
#include "quantStruct.h"
#include "floatsStruct.h"

typedef A2Methods_UArray2 A2;

/********** quantize ********
 *
 * Use: 
 *      Declares an array for the quantized element structs to be stored in, 
 *      sets the closure, and calls a mapping function on the array of
 *      unquiantized values
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2
 *      A2 array:      Array of unquantized DCT elements that is being mapped 
 *                     through to be turned into quantized values
 * Return: 
 *      A2 array holding the quantized element values in structs
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 quantize(A2Methods_T methods, A2 array)
{
        A2 quantArray = methods->new_with_blocksize((methods->width(array)), 
                                                (methods->height(array)), 
                                                sizeof(quantElems),
                                                1);
        struct closure quCl = {quantArray, 0, &methods};
        methods->map_default(array, quantizeElems, &quCl);
        
        return quantArray;
}

/********** quantizeElems ********
 *
 * Use: 
 *      This is an apply function called during mapping that converts
 *      values into quantized eqivalents  
 * Parameters:
 *      int col:     Integer of the column index of a pixel.
 *      int row:     Integer of a row index of a pixel. 
 *      A2 array2:   A 2D array of unquantized elements that is being traversed. 
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
void quantizeElems(int col, int row, A2 array2, void *elem, void *cl)
{
        assert(array2 != NULL);
        assert(cl != NULL);
        struct closure *info = cl; 
        A2Methods_T methods = *(info->methods);

        elements *els = elem;
        quantElems *store = methods->at(info->copyArray, col, row);
        quantElems newElements;

        newElements.a = ceil((els->a * 511)); 
        newElements.b = ceil(quantNums(els->b));
        newElements.c = ceil(quantNums(els->c));
        newElements.d = ceil(quantNums(els->d));
        newElements.avgPb = Arith40_index_of_chroma(els->avgPb);
        newElements.avgPr = Arith40_index_of_chroma(els->avgPr);

        *store = newElements;
}

/********** quantNums ********
 *
 * Use: 
 *      Quantizes values by checking if they are between a certain range and
 *      making appropriate calulations. 
 * Parameters:
 *      double num: The number being quantizes 
 * Return: 
 *      Double which holds the quantized value 
 * Expects: 
 *
 * Notes: 
 *      
 ************************/
double quantNums(double num)
{
        if (-0.3 <= num && num <= 0.3 ) {
                return (num * 50);
        } else if (num < -0.3) {
                return -15;
        } else if (num > 0.3) {
                return 15;
        }
        return 0;
}

/********** roundNum ********
 *
 * Use: 
 *      Rounds a number in the correct direction. If it is closer to the
 *      value about, it returns that, and if it is closer to the one below, 
 *      it returns that. 
 * Parameters:
 *      double num: double needing to be rounded
 * Return: 
 *      Double which holds the quantized value 
 * Expects: 
 *
 * Notes: 
 *      
 ************************/
int roundNum(double num)
{
        int rounded = num;
        if ((num - rounded) > 0.5) {
                return rounded + 1;
        }
        return rounded;
}

/********** unquantize ********
 *
 * Use: 
 *      Declares an array for the unquantized elem structs to be stored in, 
 *      sets the closure, and calls a mapping function on the array of
 *      quiantized values to unquantize them 
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2
 *      A2 array:      Array of quantized elements that is being mapped 
 *                     through to be unquantized 
 * Return: 
 *      A2 array holding the unquantized elements in structs
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 unquantize(A2Methods_T methods, A2 array)
{
        A2 elemArray = methods->new_with_blocksize((methods->width(array)), 
                                                (methods->height(array)), 
                                                sizeof(elements),
                                                1);
        struct closure elCl = {elemArray, 0, &methods};
        methods->map_default(array, unquantizeElems, &elCl);
        
        return elemArray;
}

/********** unquantizeElems ********
 *
 * Use: 
 *      This is an apply function called during mapping that converts
 *      quantized values into unquantized element values. 
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
void unquantizeElems(int col, int row, A2 array2, void *elem, void *cl)
{
        assert(array2 != NULL);
        assert(cl != NULL);
        struct closure *info = cl; 
        A2Methods_T methods = *(info->methods);

        quantElems *el = elem;
        elements *store = methods->at(info->copyArray, col, row);
        elements newElements;

        double temp_a, temp_b, temp_c, temp_d;        
        if (-15 <= el->b && el->b <= 15 ) {
                temp_b = ((double)(el->b)) / 50;
        } 
        if (-15 <= el->c && el->c <= 15 ) {
                temp_c = ((double)(el->c)) / 50;
        }
        if (-15 <= el->d && el->d <= 15 ) {
                temp_d = ((double)(el->d)) / 50 ;
        }
        if (el->a <= 511) {
                temp_a = ((double)(el->a)) / 511;
        } 

        newElements.a = temp_a; 
        newElements.b = temp_b;
        newElements.c = temp_c;
        newElements.d = temp_d;
        newElements.avgPb = Arith40_chroma_of_index(el->avgPb);
        newElements.avgPr = Arith40_chroma_of_index(el->avgPr);
        
        *store = newElements;
}
