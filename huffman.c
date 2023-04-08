#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>

char code_g[256]; // code_getcode
int ic = 0;       // передвигаться по массиву структру с кодами

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
        printf("%c -> ", root->ch.c);
        codes[ic].ch = root->ch;
        codes[ic].freq = root->freq;
        for (int i = 0; i < cur; ++i)
        {
            codes[ic].code[i] = code_g[i];
            if (i + 1 == cur)
                codes[ic].length = i + 1;
        }
        ic++;
        for (int i = 0; i < cur; ++i)
            printf("%c", code_g[i]);

        printf("\n");
    }
}

int BITS_IN_USE(int leaves_count, Code *codes)
{
    int n = 0;
    for (int i = 0; i < leaves_count; i++)
        n += (codes[i].freq) * codes[i].length;
    return n;
}

/////////
void print_buf_o1(unsigned char *buf, int n, char mask)
{
    for (int i = 0; i < n; i++)
    {
        printf("buf[%d]: ", i);
        for (int j = 7; j >= 0; j--)
            printf("%d", ((buf[i]) >> j) & mask);
        printf("\n");
    }
}

void print_buf_o2(unsigned short *buf, int n, char mask)
{
    for (int i = 0; i < n; i++)
    {
        printf("buf[%d]: ", i);
        for (int j = 15; j >= 0; j--)
            printf("%d", ((buf[i]) >> j) & mask);
        printf("\n");
    }
}

void print_buf_o3(unsigned short *buf, int n, char mask)
{
    for (int i = 0; i < n; i++)
    {
        printf("buf[%d]: ", i);
        for (int j = 11; j >= 0; j--)
            printf("%d", ((buf[i]) >> j) & mask);
        printf("\n");
    }
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

void zapisz(FILE *in, FILE *out, int option, int bits_in_use, int leaves_count, Code *codes)
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
    case 3:
        block_bits = 12;
        block_count = BLOCK_COUNT(block_bits, bits_in_use);
        break;
    }

    unsigned char *buf_o1 = calloc(block_count, sizeof *buf_o1);
    if (buf_o1 == NULL)
    {
        fprintf(stderr, "Nie udało się stworzyć buforu\n");
        exit(1);
    }

    short *buf_o2 = calloc(block_count, sizeof *buf_o2);
    if (buf_o2 == NULL)
    {
        fprintf(stderr, "Nie udało się stworzyć buforu\n");
        exit(1);
    }

    short *buf_o3 = calloc(block_count, sizeof *buf_o3);
    if (buf_o3 == NULL)
    {
        fprintf(stderr, "Nie udało się stworzyć buforu\n");
        exit(1);
    }

    printf("blocks count - %d\n", block_count);

    printf("******** buf bits before *********\n");
    switch (option)
    {
    case 1:
        print_buf_o1(buf_o1, block_count, 0b1);
        break;
    case 2:
        print_buf_o2(buf_o2, block_count, 0b1);
        break;
    case 3:
        print_buf_o3(buf_o3, block_count, 0b1);
        break;
    }

    int c;
    fseek(in, 0L, SEEK_SET);
    while (fread( &c, sizeof( short), 1, in))
    {
        for (int i = 0; i < leaves_count; i++)
        {
            if ((short)c == codes[i].ch.s)
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
                        switch (option)
                        {
                        case 1:
                            buf_o1[buf_index] |= (0b1 << bit_position);
                            break;
                        case 2:
                            buf_o2[buf_index] |= (0b1 << bit_position);
                            break;
                        case 3:
                            buf_o3[buf_index] |= (0b1 << bit_position);
                            break;
                        }
                        bit_position++;
                        break;

                    case '0':
                        switch (option)
                        {
                        case 1:
                            buf_o1[buf_index] &= ~(0b1 << bit_position);
                            break;
                        case 2:
                            buf_o2[buf_index] &= ~(0b1 << bit_position);
                            break;
                        case 3:
                            buf_o3[buf_index] &= ~(0b1 << bit_position);
                            break;
                        }

                        bit_position++;
                        break;
                    }
                }
            }
        }
    }

    printf("******** buf bits after ********\n");
    switch (option)
    {
    case 1:
        print_buf_o1(buf_o1, block_count, 0b1);
        fwrite(buf_o1, sizeof(char), block_count, out);
        break;
    case 2:
        print_buf_o2(buf_o2, block_count, 0b1);
        fwrite(buf_o2, sizeof(char), block_count, out);
        break;
    case 3:
        print_buf_o3(buf_o3, block_count, 0b1);
        fwrite(buf_o3, sizeof(char), block_count, out);
        break;
    }
    free(buf_o1);
    free(buf_o2);
    free(buf_o3);
}