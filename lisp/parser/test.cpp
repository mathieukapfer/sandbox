/*
 * Simple program to demonstrate semantic.
 */

#include <stdio.h>

int i_1;

int
main(int argc, char** argv)
{
    printf("Hello world.\n");
}

// (semantic-lex (point-min) (point-max))
// ((spp-system-include "stdio.h" 52 . 70) (INT 72 . 75) (symbol 76 . 79) (punctuation 79 . 80) (INT 82 . 85) (symbol 86 . 90) (semantic-list 90 . 113) (semantic-list 114 . 147))
