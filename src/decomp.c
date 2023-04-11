#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define MAX1 256
#define MAX2 65536

typedef union Character
{
        short s;
        char c;
} Character;

typedef struct Code
{
        Character ch;
        char code[65536];
        int length;
} Code;

typedef struct string
{
        char *block;
} block_str;

void reverse(block_str *blocks, int lSize, int block_bits)
{
        char tmp;
        for (int i = 0; i < lSize; i++)
        {
                for (int j = 0; j < block_bits / 2; j++)
                {
                        tmp = blocks[i].block[j];
                        blocks[i].block[j] = blocks[i].block[block_bits - j - 1];
                        blocks[i].block[block_bits - j - 1] = tmp;
                }
        }
}

void zeruj(char *w_arr, int option)
{
        switch (option)
        {
        case 1:
                for (int i = 0; i < MAX1; i++)
                        w_arr[i] = '\0';
                break;
        case 2:
                for (int i = 0; i < MAX2; i++)
                        w_arr[i] = '\0';
                break;
        }
}

void free_blocks(block_str *blocks, int n)
{
        for (int i = 0; i < n; ++i)
                free(blocks[i].block);
        free(blocks);
}

int main(int argc, char *argv[])
{
        int opt;
        while ((opt = getopt(argc, argv, ":h")) != -1)
        {
                switch (opt)
                {
                case 'h':
                        printf("Usage:\ndecomp <nazwa pliku skompresowanego> <nazwa pliku po dekomresji>\n");
                        exit(1);
                        break;
                }
        }

        int excess_bits = 0, leaves_count = 0, option = 0;
        FILE *kody = fopen("kody", "r");
        if(!kody)
        {
                fprintf(stderr, "Nie udało się odczytać pliku z kodami\nUżyj -h dla wyświetlenia menu help\n");
                exit(1);
        }
        fscanf(kody, "%d %d %d", &excess_bits, &leaves_count, &option);

        Code *codes = malloc(leaves_count * sizeof(*codes));

        switch (option)
        {
        case 1:
                for (int i = 0; i < leaves_count; i++)
                        for (int j = 0; j < MAX1; j++)
                                codes[i].code[j] = '\0';
                break;
        case 2:
                for (int i = 0; i < leaves_count; i++)
                        for (int j = 0; j < MAX2; j++)
                                codes[i].code[j] = '\0';
                break;
        }

        switch (option)
        {
        case 1:
                for (int i = 0; i < leaves_count; ++i)
                {
                        int c;
                        fscanf(kody, "%d %256s", &c, codes[i].code);
                        codes[i].ch.c = (char)c;
                        codes[i].length = strlen(codes[i].code);
                }
                break;

        case 2:
                for (int i = 0; i < leaves_count; ++i)
                {
                        int c;
                        fscanf(kody, "%d %65536s", &c, codes[i].code);
                        codes[i].ch.s = (short)c;
                        codes[i].length = strlen(codes[i].code);
                }
                break;
        }

        FILE *f = argc > 1 ? fopen(argv[1], "rb") : NULL;
        FILE *in = argc > 2 ? fopen(argv[2], "wb") : NULL;

        if (f == NULL)
        {
                fprintf(stderr, "Nie udało się odczytać pliku skompresowanego\nUżyj -h dla wyświetlenia menu help\n");
                exit(1);
        }

        if (in == NULL)
        {
                fprintf(stderr, "Nie udało się stworzyć pliku do dekompresji\nUżyj -h dla wyświetlenia menu help\n");
                exit(1);
        }

        int block_bits = 0;
        switch (option)
        {
        case 1:
                block_bits = 8;
                break;

        case 2:
                block_bits = 16;
                break;
        }

        fseek(f, 0, SEEK_END); // устанавливаем позицию в конец файла
        long lSize = 0;
        lSize = ftell(f); // получаем размер в байтах
        rewind(f);        // возвращаем в начало файла

        char mask = 0b1;
        short mask_s = 0b1;
        int lSize_s = (lSize + 1) / 2;

        char *buffer = (char *)malloc(sizeof(char) * lSize);
        short *buffer_s = (short *)malloc(sizeof(short) * lSize_s);

        int bits_at_all = lSize * block_bits; // для единой строки
        char *single_str = calloc(bits_at_all + 1, sizeof(char));
        if (!single_str)
        {
                fprintf(stderr, "Kuarro\n");
                exit(1);
        }

        block_str *blocks;

        switch (option)
        {
        case 1:
                blocks = (block_str *)malloc(sizeof(*blocks) * lSize);
                break;
        case 2:
                blocks = (block_str *)malloc(sizeof(*blocks) * lSize_s);
                break;
        }

        switch (option)
        {
        case 1:
                for (int i = 0; i < lSize; i++)
                        blocks[i].block = calloc(block_bits + 1, sizeof(char));
                break;
        case 2:
                for (int i = 0; i < lSize_s; i++)
                        blocks[i].block = calloc(block_bits + 1, sizeof(char));
                break;
        }

        if (buffer == NULL)
        {
                fprintf(stderr, "ERROR WITH MEMORY\n");
                exit(1);
        }

        size_t result;
        switch (option)
        {
        case 1:
                result = fread(buffer, sizeof(char), lSize, f); // считываем файл в буфер
                if(result != lSize)
                {
                        fprintf(stderr, "Błąd w odczytaniu pliku\n");
                        exit(1);
                }
                break;
        case 2:
                result = fread(buffer_s, sizeof(short), lSize_s, f); // считываем файл в буфер
                if(result != lSize_s)
                {
                        fprintf(stderr, "Błąd w odczytaniu pliku\n");
                        exit(1);
                }
                break;
        }

        switch (option)
        {
        case 1:
                for (int j = 0; j < lSize; j++)
                {
                        for (int i = block_bits - 1; i >= 0; i--)
                        {
                                switch (((buffer[j]) >> i) & mask)
                                {
                                case 0:
                                        strcat(blocks[j].block, "0");

                                        break;
                                case 1:
                                        strcat(blocks[j].block, "1");
                                        break;
                                }
                        }
                }
                break;

        case 2:
                for (int j = 0; j < lSize_s; j++)
                {
                        for (int i = block_bits - 1; i >= 0; i--)
                        {
                                switch (((buffer_s[j]) >> i) & mask_s)
                                {
                                case 0:
                                        strcat(blocks[j].block, "0");
                                        break;
                                case 1:
                                        strcat(blocks[j].block, "1");
                                        break;
                                }
                        }
                }
                break;
        }

        switch (option)
        {
        case 1:
                reverse(blocks, lSize, block_bits);
                break;

        case 2:
                reverse(blocks, lSize_s, block_bits);
                break;
        }

        switch (option)
        {
        case 1:
                for (int i = 0; i < lSize; i++)
                        strcat(single_str, blocks[i].block);
                break;

        case 2:
                for (int i = 0; i < lSize_s; i++)
                        strcat(single_str, blocks[i].block);
                break;
        }

        single_str[strlen(single_str) - excess_bits] = '\0';

        char w_arr[65536];
        switch (option)
        {
        case 1:
                for (int i = 0; i < MAX1; i++)
                        w_arr[i] = '\0';
                break;

        case 2:
                for (int i = 0; i < MAX2; i++)
                        w_arr[i] = '\0';
        }

        int n = strlen(single_str);

        char c;
        short s;
        int wai = 0;
        switch (option)
        {
        case 1:
                for (int i = 0; i < n; i++)
                {
                        w_arr[wai++] = single_str[i];
                        for (int j = 0; j < leaves_count; j++)
                                if (strcmp(w_arr, codes[j].code) == 0)
                                {
                                        c = codes[j].ch.c;
                                        fwrite(&c, sizeof(char), 1, in);
                                        zeruj(w_arr, option);
                                        wai = 0;
                                }
                }
                break;

        case 2:
                for (int i = 0; i < n; i++)
                {
                        w_arr[wai++] = single_str[i];
                        for (int j = 0; j < leaves_count; j++)
                                if (strcmp(w_arr, codes[j].code) == 0)
                                {
                                        s = codes[j].ch.s;
                                        fwrite(&s, sizeof(short), 1, in);
                                        zeruj(w_arr, option);
                                        wai = 0;
                                }
                }
        }

        switch (option)
        {
        case 1:
                free_blocks(blocks, lSize);
                break;
        case 2:
                free_blocks(blocks, lSize_s);
                break;
        }

        free(codes);
        free(single_str);
        free(buffer_s);
        fclose(f);
        fclose(in);
        fclose(kody);
        free(buffer);
        return 0;
}