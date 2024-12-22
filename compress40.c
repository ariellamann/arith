/*
 *      compress40.c
 *      by amann04 3/7/24
 *      arith
 *
 *      This implementation compresses and decompresses an image. 
 *      Compression and decompression are called from 40image.c and refer
 *      to the compression and decompression implementation as described 
 *      in this file. 
 *     
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "uarray.h"
#include "uarray2b.h"
#include "pnm.h"
#include <math.h>
#include "arith40.h"
#include "compvideo.h"
#include "DCT.h"
#include "quantization.h"
#include "closureStruct.h"
#include "packing.h"

typedef A2Methods_UArray2 A2;

Pnm_ppm trimImg(Pnm_ppm image); 
int makeEven(int dimension);
void printOut(uint32_t elem);
void printCompressed(A2Methods_T methods, A2 array, int width, int height);
A2 readHeader(FILE *input);
A2 readBitwords(A2Methods_T methods, FILE *input, int width, int height);


/********** compress40 ********
 *
 * Use: 
 *      Compresses an image into 32 bit bitwords and prints out the 
 *      compressed image to standard output. 
 * Parameters:
 *      FILE *input: Image to be compressed. 
 * Return: 
 *      None.  
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
void compress40(FILE *input) 
{
        /* Creaat the blocked methods. */
        A2Methods_T methods = uarray2_methods_blocked; 
        assert(methods != NULL);

        /* Read in from file. */
        Pnm_ppm origImage = Pnm_ppmread(input, methods);
        assert(origImage != NULL);
        /* Makes height and width even. */
        Pnm_ppm trimmed = trimImg(origImage);

        /* Compression steps */
        A2 doubleArray = intToDouble(methods, trimmed);
        A2 CVArray = doubleToCv(methods, doubleArray);
        A2 elemsArray = CvToElems(methods, CVArray);
        A2 quantArray = quantize(methods, elemsArray);
        A2 bitwordArray = traverseToPack(methods, quantArray);
        printCompressed(methods, bitwordArray, methods->width(trimmed), 
                                                 methods->height(trimmed));  

        Pnm_ppmfree(&trimmed);
        methods->free(&doubleArray);
        methods->free(&CVArray);
        methods->free(&elemsArray);
        methods->free(&quantArray);
        methods->free(&bitwordArray);
}

/********** decompress40 ********
 *
 * Use: 
 *      Decompresses an compressed image into the original image. 
 * Parameters:
 *      FILE *input: Decompressed version of the image. 
 * Return: 
 *      None.  
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
void decompress40 (FILE *input)
{
        A2Methods_T methods = uarray2_methods_blocked; 
        assert(methods != NULL); 

        /* Reads in header from decompressed file and returns empty Pnm_ppm 
           of correct size for the final decompressed image */
        A2 imageArray = readHeader(input);

        int width = methods->width(imageArray);
        int height = methods->height(imageArray);

        Pnm_ppm pixmap = malloc(sizeof(struct Pnm_ppm));
        pixmap->width = width;
        pixmap->height = height;
        pixmap->denominator = 255;
        pixmap->methods = methods;
        
        A2 bitArray = readBitwords(methods, input, (width / 2), (height / 2));
        A2 quantArray2 = traverseToUnpack(methods, bitArray);
        A2 elemsArray2 = unquantize(methods, quantArray2);
        A2 CVArray2 = elemsToCv(methods, elemsArray2);
        A2 doubleArray2 = CvToDouble(methods, CVArray2);
        A2 finalImg = doubleToInt(methods, doubleArray2);
        pixmap->pixels = finalImg;

        Pnm_ppmwrite(stdout, pixmap);
        Pnm_ppmfree(&pixmap);
        methods->free(&imageArray);
        methods->free(&bitArray);
        methods->free(&quantArray2);
        methods->free(&elemsArray2);
        methods->free(&CVArray2);
        methods->free(&doubleArray2);
}

/********** trimImg ********
 *
 * Use: 
 *      Trims off the last row or col of an image if the height or width
 *      isn't even. 
 * Parameters:
 *      Pnm_ppm image: the image that is being trimmed
 * Return: 
 *      None
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
Pnm_ppm trimImg(Pnm_ppm image) 
{
        Pnm_ppm newImg = malloc(sizeof(struct Pnm_ppm));
        newImg->denominator = image->denominator; 
        newImg->methods = image->methods;
        newImg->height = makeEven(image->height); 
        newImg->width = makeEven(image->width); 
        A2 newPixels = image->methods->new(newImg->width, newImg->height, 
                                                       sizeof(struct Pnm_rgb));
        newImg->pixels = newPixels; 

        for (int i = 0; i < (int)newImg->height; i++){
                for (int j = 0; j < (int)newImg->width; j++){
                        Pnm_rgb pointer = image->methods->at(newPixels, j, i);
                        Pnm_rgb toStore = image->methods->at(image->pixels, 
                                                                        j, i);
                        struct Pnm_rgb newStruct = { .red = toStore->red, 
                                          .green = toStore->green,
                                          .blue = toStore->blue,
                                        };
                        *pointer = newStruct;
                }
        }

        Pnm_ppmfree(&image); 
        return newImg; 
}

/********** makeEven ********
 *
 * Use: 
 *      Checks if a number is even, if it is, the number is returned with no
 *      change, but if it is odd, the closest even number below it is returned.
 * Parameters:
 *      int dimension: Integer being made evem
 * Return: 
 *      the even integer 
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
int makeEven(int dimension)
{
        if ((dimension % 2) != 0){
                return dimension - 1;
        } else {
            return dimension;
        }
}

/********** printCompressed ********
 *
 * Use: 
 *      Prints a header for the decompressed image to standard output and then
 *      maps through the bitwords to print them out to standard output in 
 *      row major order. 
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2 array 
 *      A2 array:               Array of 32-bit bitwords to be printed
 *      int width:              Width of original image
 *      int height:             Height of original image
 * Return: 
 *      None
 * Expects: 
 *      The array given to not be NULL 
 *      The methods to not be NULL 
 * Notes: 
 *      
 ************************/
void printCompressed(A2Methods_T methods, A2 array, int width, int height)
{
        assert(array != NULL);
        assert(methods != NULL);

        printf("COMP40 Compressed image format 2\n%u %u\n", width, height);

        for (int i = 0; i < height / 2; i++) {
                for (int j = 0; j < width / 2; j++) { 
                        uint32_t *element = methods->at(array, j, i);
                        printOut(*element);
                }
        }
}

/********** printOut ********
 *
 * Use: 
 *      This is an apply function called during mapping that converts
 *      prints out every single 32-bit bitword to standard output using
 *      the bitpack.c interface and putchar function. 
 * Parameters:
 *      uint32_t elem:    32-bit bitword to be printed to standard output. 
 * Return: 
 *      None
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
void printOut(uint32_t elem)
{
        char byte1 = (char)Bitpack_getu(elem, sizeof(char) * 8, 24);
        char byte2 = (char)Bitpack_getu(elem, sizeof(char) * 8, 16);
        char byte3 = (char)Bitpack_getu(elem, sizeof(char) * 8, 8);
        char byte4 = (char)Bitpack_getu(elem, sizeof(char) * 8, 0);

        byte1 = putchar(byte1);
        byte2 = putchar(byte2);
        byte3 = putchar(byte3);
        byte4 = putchar(byte4);
}

/********** readHeader ********
 *
 * Use: 
 *      Reads in the header from a file holding a decompressed image and 
 *      allocates space for a Pnm_ppm that will be printed out later when
 *      the image is completely decompressed. 
 * Parameters:
 *      FILE * input:     file pointer to the file being read in. 
 * Return: 
 *      Pnm_ppm of the same height and width of the original image which
 *       will hold the decompressed image. 
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
A2 readHeader(FILE *input)
{
        unsigned width, height;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", 
                                                             &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        A2Methods_T methods = uarray2_methods_blocked;
        assert(methods != NULL);

        A2 array = methods->new_with_blocksize(width, height, 
                                                sizeof(struct Pnm_rgb), 1);

        return array;
}

/********** readBitwords ********
 *
 * Use: 
 *      Reads in the header from a file holding a decompressed image and 
 *      allocates space for a Pnm_ppm that will be printed out later when
 *      the image is completely decompressed. 
 * Parameters:
 *      A2Methods_T methods:    Methods being used on the A2 array. 
 *      FILE * input:           File pointer to the file being read in. 
 *      A2 array:               Array of the original image size so that it is
 *                              clear how much to read in. 
 * Return: 
 *     A2 array holding the 32-bit words that were read in from the file. 
 * Expects: 
 *      Methods to not be NULL, will throw CRE if it is. 
 *      The input file to not be NULL, will throw CRE if it is. 
 *      The array to not be NULL, will throw CRE if it is. 
 * Notes: 
 *      
 ************************/
A2 readBitwords(A2Methods_T methods, FILE *input, int width, int height)
{
        assert(methods != NULL);
        assert(input != NULL);

        A2 bitwordArr = methods->new(width, height, sizeof(uint64_t));
        for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) { 
                        uint64_t *store = methods->at(bitwordArr, j, i);
                        uint64_t bitword = 0;

                        int byte1 = getc(input);
                        int byte2 = getc(input);
                        int byte3 = getc(input);
                        int byte4 = getc(input);

                        bitword = Bitpack_newu(bitword, sizeof(char) * 8, 
                                                                24, byte1);
                        bitword = Bitpack_newu(bitword, sizeof(char) * 8, 
                                                                16, byte2);
                        bitword = Bitpack_newu(bitword, sizeof(char) * 8, 
                                                                8, byte3);
                        bitword = Bitpack_newu(bitword, sizeof(char) * 8, 
                                                                0, byte4);

                        *store = bitword;
                }
        }
        return bitwordArr;
}
