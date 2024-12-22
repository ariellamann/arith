/*
 *      testing.c
 *      by akee01 & amann04, 3/7/24
 *      arith
 *
 *      This is a file used to test the functions written in bitpack.c 
 *      This tests them by calling them and making various assertions.  
 *     
 */

#include "bitpack.h"
#include "assert.h"
#include <stdlib.h>
#include <stdio.h>


int main () 
{

    uint64_t gotten = Bitpack_getu(46, 3, 3);
    //fprintf(stderr, "%s %ld\n", "i: ", i);

    assert(gotten == 5);

    assert(Bitpack_fitsu(7, 3));
    assert(!(Bitpack_fitsu(8, 3)));

    assert(!Bitpack_fitss(5, 3));
    assert(Bitpack_fitss(-7, 5));

    uint64_t check = Bitpack_getu(46, 3, 3);
    assert(check == 5);

    int64_t check0 = Bitpack_gets(46, 3, 3);
    assert(check0 == -3);

    uint64_t a = Bitpack_getu(0x3f4, 6, 2);
    assert(a == 61);

    int64_t b = Bitpack_gets(0x3f4, 6, 2);
    assert(b == -3);


    //word, width, lsb, val
    uint64_t check1 = Bitpack_newu(46, 3, 1, 4);
    assert(check1 == 40);

    int64_t check3 = Bitpack_news(40, 3, 1, -3);
    fprintf(stderr, "%s %lu\n", "check3: ", check3);
    assert(check3 == 42);


    uint64_t word = 152;
    unsigned w = 6;
    unsigned lsb = 3;
    uint64_t val = 13;
    unsigned w2 = 3;
    uint64_t lsb2 = 12; 

    uint64_t c = Bitpack_getu((Bitpack_newu(word, w, lsb, val)), w2, lsb2); 
    uint64_t d = Bitpack_getu(word, w2, lsb2);
    assert(c == d);

    uint64_t check2 = Bitpack_getu(Bitpack_newu(46, 3, 1, 4), 3, 1);
    assert(check2 == 4);


    int64_t check4 = Bitpack_gets(Bitpack_news(46, 3, 1, -3), 3, 1);
    assert(check4 == -3);


    return 0;
}