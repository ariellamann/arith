/*
 *      bitpack.c
 *      by akee01 & amann04, 3/7/24
 *      arith
 *
 *      This implements the program to pack bits into a word.
 *     
 */


#include "bitpack.h"
#include "assert.h"
#include <stdlib.h>
#include <stdio.h>
#include "math.h"

Except_T Bitpack_Overflow = { "Overflow packing bits" };
const int numBits = sizeof(uint64_t) * 8;


/********** Bitpack_fitsu ********
 *
 * Use: 
 *      Checks if a given unsigned integer can fit within a certain number of
 *      bits. 
 * Parameters:
 *      uint64_t n: Unsigned integer that is being checked 
 *      unsigned width: The number of bits
 * Return: 
 *      A bool of whether or not the uint can fit it the number of bits. 
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        uint64_t maxvalue = (1 << width);

        if (n < maxvalue) {
                return true;
        }
        else {
                return false;
        }
}

/********** Bitpack_fitss ********
 *
 * Use: 
 *      Checks if a given signed integer can fit within a certain number
 *      of bits. 
 * Parameters:
 *      int64_t n: Signed integer that is being checked 
 *      unsigned width: The number of bits
 * Return: 
 *      A bool of whether or not the signed int can fit it the number of bits 
 * Expects: 
 *      
 * Notes: 
 *      
 ************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(0 < (int)width && (int)width <= 64);

        int64_t maxvalue = (1 << (width - 1)) - 1; 
        int64_t minvalue = - (1 << (width));

        if (n < maxvalue && n > minvalue) {
                return true;
        } else {
                return false;
        }
}

/********** Bitpack_getu ********
 *
 * Use: 
 *      Gets the bits from a bit word at a specifc offset and length and 
*       returns it as an unsigned integer
 * Parameters:
 *      uint64_t word: bit word that it will get a field from
 *      unsigned width: The number of bits in the field wanted
 *      unsigned lsb: the least significant bit of the wanted field
 * Return: 
 *      The uint64_t which stores the integer retrieved from the bit field. 
 * Expects: 
 *      Width of the bit field to be greater than 0 and less than 64
 *      The least significant bit plus the width is less than 64
 * Notes: 
 *      
 ************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(0 < (int)width && (int)width <= 64);
        assert((lsb + width) <= 64);

        /* Makes the mask all 1s. Prints out a -1 (all 1s binary) */
        uint64_t mask = ~0;

        /* Shift right, then left to get the mask we want */
        mask = mask >> (numBits - width) << lsb;

        uint64_t extracted = (word & mask) >> lsb;
        return extracted;
}

/********** Bitpack_gets ********
 *
 * Use: 
 *      Gets the bits from a bit word at a specifc offset and length and 
 *      returns it as signed integer. 
 * Parameters:
 *      uint64_t word: bit word that it will get a field from
 *      unsigned width: The number of bits in the field wanted
 *      unsigned lsb: the least significant bit of the wanted field
 * Return: 
 *      The int64_t which stores the integer retrieved from the bit field. 
 * Expects: 
 *      Width of the bit field to be greater than 0 and less than 64
 *      The least significant bit plus the width is less than 64
 * Notes: 
 *      
 ************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(0 < (int)width && (int)width <= 64);
        assert((lsb + width) <= 64);

        /* Makes the mask all 1s. Prints out a -1 (all 1s binary) */
        uint64_t mask = ~0;

        /* Shift right, then left to get the mask we want */
        mask = mask >> (numBits - width) << lsb;
        int64_t maskedWord = (word & mask);

        int64_t toReturn = (maskedWord << (numBits - (width + lsb))) >> 
                                                        (numBits - width);

        return toReturn;
}


/********** Bitpack_newu ********
 *
 * Use: 
 *      Creates a new integer from a given uint by swapping out a specific
 *      bitfield with a new given value. 
 * Parameters:
 *      uint64_t word: Bit word of which a field will be replaced
 *      unsigned width: The number of bits in the field wanted
 *      unsigned lsb: the least significant bit of the wanted field
 *      uint64_t value: Value wanted to be stored
 * Return: 
 *      The newly created uint64_t with the newly swapped out bitfield
 *      with the value
 * Expects: 
 *      Width of the bitfield to be greater than 0 and less than 64
 *      That the least significant bit plus the width is less than 64
 *      The value can fit within the given width
 * Notes: 
 *      
 ************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                                                                uint64_t value)
{
        assert(0 < (int)width && (int)width <= 64);
        assert((lsb + width) <= 64);
        
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        /* Makes the mask all 1s */
        uint64_t mask = ~0;
        
        /* Shift right, then left, and flip bits to get the mask we want */
        mask = ~(mask >> (numBits - width) << lsb);
        
        uint64_t newWord = ((word & mask) | (value << lsb));

        return newWord;
}

/********** Bitpack_news ********
 *
 * Use: 
 *      Creates a new integer from a given signed int by swapping out a 
 *      specific bitfield with a new given value. 
 * Parameters:
 *      uint64_t word: Bit word of which a field will be replaced
 *      unsigned width: The number of bits in the field wanted
 *      unsigned lsb: the least significant bit of the wanted field
 *      int64_t value: Value wanted to be stored
 * Return: 
 *      The newly created uint64_t with the newly swapped out bitfield
 *      with the value 
 * Expects: 
 *      Width of the bitfield to be greater than 0 and less than 64
 *      That the least significant bit plus the width is less than 64
 *      The value can fit within the given width
 * Notes: 
 *      
 ************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                                                                int64_t value)
{
        assert(0 < (int)width && (int)width <= 64);
        assert((lsb + width) <= 64);
        uint64_t newVal;
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        } else {
                newVal = (uint64_t)value;
        }

        return Bitpack_newu(word, width, lsb, Bitpack_getu(newVal, width, 0));
}

