#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>

char code_g[65536];
int ic = 0;

void get_code(Node *root, Code *codes, int cur)
{
    if (root->left)
    {
        code_g[cur] = '0';
        get_code(root->left, codes, cur + 1);
    }

    if (root->right)
    {
        code_g[cur] = '1';
        get_code(root->right, codes, cur + 1);
    }

    if (!(root->left) && !(root->right))
    {
        codes[ic].ch = root->ch;
        codes[ic].freq = root->freq;
        for (int i = 0; i < cur; ++i)
        {
            codes[ic].code[i] = code_g[i];
            if (i + 1 == cur)
                codes[ic].length = i + 1;
        }
        ic++;
    }
}

int BITS_IN_USE(int leaves_count, Code *codes)
{
    int n = 0;
    for (int i = 0; i < leaves_count; i++)
        n += (codes[i].freq) * codes[i].length;
    return n;
}

int BLOCK_COUNT(int block_bits, int bits_in_use)
{
    int n;
    if ((bits_in_use % block_bits) > 0)
        n = (bits_in_use / block_bits) + 1;
    else
        n = bits_in_use / block_bits;
    return n;
}

void zapisz(FILE *in, FILE *out, FILE *table, int option, int bits_in_use, int leaves_count, Code *codes)
{
    int block_count, block_bits;
    int buf_index = 0;
    int bit_position = 0;

    switch (option)
    {
    case 1:
        block_bits = 8;
        block_count = BLOCK_COUNT(block_bits, bits_in_use);
        break;
    case 2:
        block_bits = 16;
        block_count = BLOCK_COUNT(block_bits, bits_in_use);
        break;
    }

    char *buf_o1 = calloc(block_count, sizeof(*buf_o1));
    if (buf_o1 == NULL)
    {
        fprintf(stderr, "Nie udało się stworzyć buforu\n");
        exit(1);
    }

    short *buf_o2 = calloc(block_count, sizeof(*buf_o2));
    if (buf_o2 == NULL)
    {
        fprintf(stderr, "Nie udało się stworzyć buforu\n");
        exit(1);
    }

    fseek(in, 0L, SEEK_SET);
    int c1;
    short c2;
    switch (option)
    {
    case 1:
        while (fread(&c1, sizeof(char), 1, in))
        {
            for (int i = 0; i < leaves_count; i++)
            {
                if ((char)c1 == codes[i].ch.c)
                {
                    for (int k = 0; k < codes[i].length; k++)
                    {
                        if (bit_position == block_bits)
                        {
                            buf_index++;
                            bit_position = 0;
                        }

                        switch (codes[i].code[k])
                        {
                        case '1':
                            buf_o1[buf_index] |= (0b1 << bit_position);
                            bit_position++;
                            break;

                        case '0':
                            buf_o1[buf_index] &= ~(0b1 << bit_position);
                            bit_position++;
                            break;
                        }
                    }
                }
            }
        }
        fwrite(buf_o1, sizeof(char), block_count, out);
        break;

    case 2:
        while (fread(&c2, sizeof(short), 1, in))
        {
            for (int i = 0; i < leaves_count; i++)
            {
                if ((short)c2 == codes[i].ch.s)
                {
                    for (int k = 0; k < codes[i].length; k++)
                    {
                        if (bit_position == block_bits)
                        {
                            buf_index++;
                            bit_position = 0;
                        }

                        switch (codes[i].code[k])
                        {
                        case '1':
                            buf_o2[buf_index] |= (0b1 << bit_position);
                            bit_position++;
                            break;

                        case '0':
                            buf_o2[buf_index] &= ~(0b1 << bit_position);
                            bit_position++;
                            break;
                        }
                    }
                }
            }
        }
        fwrite(buf_o2, sizeof(short), block_count, out);
        break;
    }

    fprintf(table, "%d %d %d\n", block_count * block_bits - bits_in_use, leaves_count, option);

    for (int i = 0; i < leaves_count; ++i)
    {
        switch (option)
        {
        case 1:
            fprintf(table, "%d %s\n", codes[i].ch.c, codes[i].code);
            break;
        case 2:
            fprintf(table, "%d %s\n", codes[i].ch.s, codes[i].code);
            break;
        }
    }

    free(buf_o1);
    free(buf_o2);
}