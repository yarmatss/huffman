#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "minheap.h"

typedef struct Code
{
        Character ch;
        char code[65536];
        int freq;
        int length;
} Code;

void get_code(Node *root, Code *codes, int cur);
int BITS_IN_USE(int leaves_count, Code *codes);
int BLOCK_COUNT(int block_bits, int bits_in_use);
void zapisz(FILE *in, FILE *out, FILE *table, int option, int bits_in_use, int leaves_count, Code *codes);

#endif