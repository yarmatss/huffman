#include "huffman.h"
#include <stdio.h>

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
        printf("%c -> ", root->ch);
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