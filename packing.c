#include "packing.h"
#include "closureStruct.h"
#include "quantStruct.h"

typedef A2Methods_UArray2 A2;

/********** traverseToPack ********
 *
 * Use: 
 *      Declares an array for bitpacked words to be stored in, 
 *      sets the closure, and calls a mapping function on the array of
 *      quiantized values to pack them into bitwords.  
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2
 *      A2 array:      Array of quantized elements that is being mapped 
 *                     through to be packed.  
 * Return: 
 *      A2 array holding the bitpacked words. 
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 traverseToPack(A2Methods_T methods, A2 array)
{ 
        A2 bitwordArr = methods->new_with_blocksize((methods->width(array)), 
                                     (methods->height(array)), 
                                     sizeof(uint32_t), 1);
        struct closure bitpackCl = {bitwordArr, 0, &methods};
        methods->map_default(array, packWords, &bitpackCl);
        
        return bitwordArr;
}

/********** packWords ********
 *
 * Use: 
 *      This is an apply function called during mapping that bitpacks 
 *      bitfields into one 32-bit bitword. 
 * Parameters:
 *      int col:     Integer of the column index of a pixel.
 *      int row:     Integer of a row index of a pixel. 
 *      A2 array2:   A 2D array of quantized elements that is being packed. 
 *      void *elem:  A pointer that points to the element in the 
 *                   current column and row the mapping function is at.
 *      void *cl:    Void pointer of a closure which holds a closure struct. 
 *                   The closure struct has the array where all the newly 
 *                   created information will be stored (new structs etc.) 
 *                   and the methods being used.   
 * Return: 
 *      None
 * Expects: 
 *      The closure to not be NULL, it will throw CRE if it is. 
 * Notes: 
 *      
 ************************/
void packWords(int col, int row, A2 array2, void *elem, void *cl)
{
        assert(array2 != NULL);
        assert(cl != NULL);
        struct closure *info = cl; 
        A2Methods_T methods = *(info->methods);

        uint32_t *store = methods->at(info->copyArray, col, row);
        quantElems *fields = elem;
        uint64_t bitword = 0;

        bitword = Bitpack_newu(bitword, 9, 23, fields->a);
        bitword = Bitpack_news(bitword, 5, 18, fields->b);
        bitword = Bitpack_news(bitword, 5, 13, fields->c);
        bitword = Bitpack_news(bitword, 5, 8, fields->d);
        bitword = Bitpack_newu(bitword, 4, 4, fields->avgPb);
        bitword = Bitpack_newu(bitword, 4, 0, fields->avgPr);

        *store = (uint32_t)bitword;
}

/********** traverseToUnpack ********
 *
 * Use: 
 *      Declares an array for quantized elements to be stored in, 
 *      sets the closure, and calls a mapping function on the array of
 *      bitpacked words to get the different bitfields. 
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2
 *      A2 array:      Array of bitpacked words that will be traversed and  
 *                     unpacked. 
 * Return: 
 *      A2 array holding the quantized elements (unpacked bitfields) 
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 traverseToUnpack(A2Methods_T methods, A2 array)
{
        A2 quantArray = methods->new_with_blocksize((methods->width(array)), 
                                     (methods->height(array)), 
                                     sizeof(quantElems), 1);
        struct closure unpackCl = {quantArray, 0, &methods};
        methods->map_default(array, unpackWords, &unpackCl);
        
        return quantArray;
}

/********** unpackWords ********
 *
 * Use: 
 *      This is an apply function called during mapping that unpacks 32-bit 
 *      bitwords into six different bitfields and stores that information 
 *      in the quanitzed elements struct. 
 * Parameters:
 *      int col:     Integer of the column index of a pixel.
 *      int row:     Integer of a row index of a pixel. 
 *      A2 array2:   A 2D array of bitpacked words
 *      void *elem:  A pointer that points to the element in the 
 *                   current column and row the mapping function is at.
 *      void *cl:    Void pointer of a closure which holds a closure struct. 
 *                   The closure struct has the array where all the newly 
 *                   created information will be stored (new structs etc.) 
 *                   and the methods being used.   
 * Return: 
 *      None
 * Expects: 
 *      The closure to not be NULL, it will throw CRE if it is. 
 *      The array to not be NULL, it will throw CRE if it is.  
 * Notes: 
 *      
 ************************/
void unpackWords(int col, int row, A2 array2, void *elem, void *cl)
{
        assert(array2 != NULL);
        assert(cl != NULL);
        struct closure *info = cl; 
        A2Methods_T methods = *(info->methods);

        uint32_t *bitword = elem;
        quantElems *store = methods->at(info->copyArray, col, row);
        quantElems fields;

        fields.a = Bitpack_getu(*bitword, 9, 23);
        fields.b = Bitpack_gets(*bitword, 5, 18);
        fields.c = Bitpack_gets(*bitword, 5, 13);
        fields.d = Bitpack_gets(*bitword, 5, 8);
        fields.avgPb = Bitpack_getu(*bitword, 4, 4);
        fields.avgPr = Bitpack_getu(*bitword, 4, 0);

        *store = fields;
}