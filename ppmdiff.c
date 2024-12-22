/*
 *      ppmdiff.c
 *      by akee01 & amann04, 3/7/24
 *      arith
 *
 *      This program compares two images given on the command line and prints
 *      out a float value representing the difference between the two images.
 *      This program was used to test through the 40image program at every
 *      step to ensure the compression and decompression steps were 
 *      maintaining as much memory as possible during each step.  
 *     
 */

#include "pnm.h"
#include "assert.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "a2methods.h"
#include "assert.h"
#include "pnm.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <math.h>

void trim_check_img(Pnm_ppm first, Pnm_ppm second);
int fix_to_even(int dim);
void calculate(Pnm_ppm first, Pnm_ppm second);

int main(int argc, char *argv[])
{
        
        if (argc < 2 || argc > 3) {
                fprintf(stderr, "%s\n", "Wrong number of command line elements");
                return EXIT_FAILURE;
        }

        FILE *before;
        before = fopen(argv[1], "r");
        assert(before != NULL);

        FILE *after;
        after = fopen(argv[2], "r");
        assert(after != NULL);

        A2Methods_T methods = uarray2_methods_plain; 

        /* Read and write the image into a ppm */
        Pnm_ppm imageOne = Pnm_ppmread(before, methods);
        Pnm_ppm imageTwo = Pnm_ppmread(after, methods); 
    
        trim_check_img(imageOne, imageTwo);

        fclose(before);
        fclose(after);
        return 0;
}


/********** trim_check_img ********
 *
 * Use: 
 *      Checks that the difference in size between the images is not too large
 *      and makes the widths and heights even if they are not already. 
 * Parameters:
 *      Pnm_ppm first: The first image given on the command line 
 *      Pnm_ppm second: The first image given on the command line 
 * Return: 
 *      None.
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
void trim_check_img(Pnm_ppm first, Pnm_ppm second)
{
        if (abs((int)first->width - (int)second->width) > 1){
                fprintf(stderr, "%s %d\n", "Difference in Width is too large", 
                                                                           1);
                exit(1);
        } 
        if(abs((int)first->height - (int)second->height) > 1){
                fprintf(stderr, "%s %d\n", "Difference in Height is too large", 
                                                                            1);
                exit(1);
        }
        first->width = fix_to_even(first->width);
        first->height = fix_to_even(first->height);
        second->width = fix_to_even(second->width);
        second->height = fix_to_even(second->height);
        calculate(first, second);
}

/********** fix_to_even ********
 *
 * Use: 
 *      Checks if a number is even, if it is, the number is returned with no
 *      change, but if it is odd, the closest even number below it is returned.
 * Parameters:
 *      int dim: The dimension of the image which is being check for being even
 * Return: 
 *      the even integer 
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
int fix_to_even(int dim)
{
        if ((dim % 2) != 0){
                return dim - 1;
        } else {
            return dim;
        }
}

/********** calculate ********
 *
 * Use: 
 *      Calculates the difference between two images by converting them to 
 *      floats and computing some calculations to compute for E and prints
 *      it out.  
 * Parameters:
 *      Pnm_ppm first: The first image given on the command line 
 *      Pnm_ppm second: The first image given on the command line 
 * Return: 
 *      None
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
void calculate(Pnm_ppm first, Pnm_ppm second)
{       
        double numerator = 0.00;
        
        int height = MIN(first->height, second->height);
        int width = MIN(first->width, second->width);
        assert(height >= 0 && width >= 0);

        Pnm_rgb colors1, colors2;
        double denom1 = (double)first->denominator;
        double denom2 = (double)second->denominator;

        for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                        colors1 = (Pnm_rgb)first->methods->at
                                                        (first->pixels, i, j);
                        colors2 = (Pnm_rgb)second->methods->at
                                                        (second->pixels, i, j);

                        numerator += (((((double)colors1->red)/denom1) - 
                                        (((double)colors2->red)/denom2)) * 
                                        ((((double)colors1->red)/denom1) - 
                                        (((double)colors2->red)/denom2)));
                        numerator += (((((double)colors1->green)/denom1) - 
                                        (((double)colors2->green)/denom2)) * 
                                        ((((double)colors1->green)/denom1) - 
                                        (((double)colors2->green)/denom2)));
                        numerator += (((((double)colors1->blue)/denom1) - 
                                        (((double)colors2->blue)/denom2)) * 
                                        ((((double)colors1->blue)/denom1) - 
                                        (((double)colors2->blue)/denom2)));
                }
        }
        
        double denominator = (3 * width * height);
        float E = sqrt((float)numerator / (float)denominator);
        printf("%s %f\n", "E: ", E);
}

