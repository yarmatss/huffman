#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "minheap.h"
#include "huffman.h"

int main(int argc, char **argv)
{

        FILE *in = argc > 1 ? fopen(argv[1], "rb") : NULL;
        FILE *out = argc > 2 ? fopen(argv[2], "wb") : NULL;
        if (in == NULL)
        {
                fprintf(stderr, "Nie udało się odczytać pliku \"%s\"\n", argv[1]);
                exit(1);
        }

        if (out == NULL)
        {
                fprintf(stderr, "Nie udało sie stworzyć pliku \"%s\"\n", argv[2]);
                exit(1);
        }

        int option = 0, opt;

        while( (opt = getopt( argc, argv, "o:")) != -1)
        {
                switch(opt)
                {
                        case 'o':
                                option = atoi(optarg);
                                break;
                }
        }

        if( option < 1 || option > 3 )
        {
                fprintf(stderr, "Błędny lub niepodany stopień kompresji!\nUżyj -h dla wyświetlenia menu help\n");
                exit(1);
        }

        int count[256] = {0};

        int c, leaves_count = 0;
        while ((c = fgetc(in)) != EOF)
        {
                count[c]++;
                if (count[c] == 1)
                        leaves_count++;
        }

        if (leaves_count == 0)
        {
                fprintf(stderr, "Pusty plik \"%s\"\n", argv[1]);
                fclose(in);
                fclose(out);
                exit(1);
        }

        minHeap *heap = create_min_heap(2 * leaves_count - 1);

        for (int i = 0; i < 256; i++)
                if (count[i] != 0)
                        insert(heap, (char)i, count[i], NULL, NULL);

        Node *root = create_tree(heap);

        Code *codes = calloc(leaves_count, sizeof(Code));

        printf("\n\n\nCodes: \n");
        get_code(root, codes, 0);

        printf("----------");
        printf("\nstruct codes\n");
        for (int i = 0; i < leaves_count; i++)
        {
                printf("%c - %s (length - %d)\n", codes[i].ch, codes[i].code, codes[i].length);
        }

        // now bits----------------------------------------------------------------------------------------------------------------
        

        int bits_in_use = BITS_IN_USE(leaves_count, codes);
        printf("bit in use - %d\n", bits_in_use);

        int block_bits; 

        if (option == 1)
        { // opcja 1 - block 8 bitów
                block_bits = 8;
                int block_o1_count = BLOCK_COUNT( block_bits , bits_in_use);
                printf("blocks count - %d\n", block_o1_count);

                unsigned char *buf = calloc(block_o1_count, sizeof(char));
                if (buf == NULL)
                {
                        fprintf(stderr, "Memory Error\n");
                        exit(1);
                }

                char mask_o1 = 0b1;

                printf("******** buf bits before *********\n");
                print_buf_o1(buf, block_o1_count, mask_o1);

                // меняю биты на наши коды:

                int buf_index = 0;    // dla buffera
                int bit_position = 0; // dla peredvizenia po bitam

                fseek(in, 0L, SEEK_SET); // что бы ещё раз прочитать файл
                while ((c = fgetc(in)) != EOF)
                {
                        for (int i = 0; i < leaves_count; i++)
                        {
                                if ((char)c == codes[i].ch)
                                {
                                        // printf("--%c--\n", c );
                                        for (int k = 0; k < codes[i].length; k++)
                                        {
                                                if (bit_position == block_bits)
                                                { // block zavershen
                                                        buf_index++;
                                                        bit_position = 0;
                                                        // printf("BUFF++\n");
                                                }

                                                switch (codes[i].code[k])
                                                {
                                                case '1':
                                                        // printf("JEDEN\n");
                                                        buf[buf_index] = (buf[buf_index]) | (mask_o1 << bit_position);
                                                        bit_position++;
                                                        // show_buf_c( buf , block_char_count , mask_c );
                                                        break;

                                                case '0':
                                                        // printf("ZERO\n");
                                                        buf[buf_index] = (buf[buf_index]) & ~(mask_o1 << bit_position);
                                                        bit_position++;
                                                        // show_buf_c( buf , block_char_count , mask_c );
                                                        break;
                                                }
                                        }
                                }
                        }
                }

                printf("******** buf bits after ********\n");
                print_buf_o1(buf, block_o1_count, mask_o1);

                fwrite(buf, sizeof(char), block_o1_count, out);

                free(buf);
        }

        if (option == 2)
        { // opcja 2 - block 16 bitów
                block_bits = 16;
                int block_o2_count = BLOCK_COUNT( block_bits ,bits_in_use );
                printf("blocks count - %d\n", block_o2_count);

                unsigned short *buf = calloc(block_o2_count, sizeof(unsigned short));
                if (buf == NULL)
                {
                        fprintf(stderr, "Memory Error\n");
                        exit(1);
                }

                short mask_o2 = 0b1;

                printf("\n******** buf bits before *********\n");
                print_buf_o2(buf, block_o2_count, mask_o2);

                // меняю биты на наши коды:

                int buf_index = 0;    // dla buffera
                int bit_position = 0; // dla peredvizenia po bitam

                fseek(in, 0L, SEEK_SET); // что бы ещё раз прочитать файл
                while ((c = fgetc(in)) != EOF)
                {
                        for (int i = 0; i < leaves_count; i++)
                        {
                                if ((char)c == codes[i].ch)
                                {
                                        // printf("--%c--\n", c );
                                        for (int k = 0; k < codes[i].length; k++)
                                        {
                                                if (bit_position == block_bits)
                                                { // block zavershen
                                                        buf_index++;
                                                        bit_position = 0;
                                                        // printf("BUFF++\n");
                                                }

                                                switch (codes[i].code[k])
                                                {
                                                case '1':
                                                        // printf("JEDEN\n");
                                                        buf[buf_index] = (buf[buf_index]) | (mask_o2 << bit_position);
                                                        bit_position++;
                                                        // show_buf_c( buf , block_char_count , mask_c );
                                                        break;

                                                case '0':
                                                        // printf("ZERO\n");
                                                        buf[buf_index] = (buf[buf_index]) & ~(mask_o2 << bit_position);
                                                        bit_position++;
                                                        // show_buf_c( buf , block_char_count , mask_c );
                                                        break;
                                                }
                                        }
                                }
                        }
                }

                printf("******** buf bits after ********\n");
                print_buf_o2(buf, block_o2_count, mask_o2);

                fwrite(buf, sizeof(short), block_o2_count, out);
                free(buf);
        }

        if (option == 3)
        { // opcja 3 - block 12 bitów
                block_bits = 12;
                int block_o3_count = BLOCK_COUNT( block_bits , bits_in_use);
                printf("blocks count - %d\n", block_o3_count);

                unsigned short *buf = calloc(block_o3_count, sizeof(short) );
                if (buf == NULL)
                {
                        fprintf(stderr, "Memory Error\n");
                        exit(1);
                }

                short mask_o3 = 0b1;

                printf("******** buf bits before *********\n");
                print_buf_o3(buf, block_o3_count, mask_o3);

                // меняю биты на наши коды:

                int buf_index = 0;    // dla buffera
                int bit_position = 0; // dla peredvizenia po bitam

                fseek(in, 0L, SEEK_SET); // что бы ещё раз прочитать файл
                while ((c = fgetc(in)) != EOF)
                {
                        for (int i = 0; i < leaves_count; i++)
                        {
                                if ((char)c == codes[i].ch)
                                {
                                        // printf("--%c--\n", c );
                                        for (int k = 0; k < codes[i].length; k++)
                                        {
                                                if (bit_position == block_bits)
                                                { // block zavershen
                                                        buf_index++;
                                                        bit_position = 0;
                                                        // printf("BUFF++\n");
                                                }

                                                switch (codes[i].code[k])
                                                {
                                                case '1':
                                                        // printf("JEDEN\n");
                                                        buf[buf_index] = (buf[buf_index]) | (mask_o3 << bit_position);
                                                        bit_position++;
                                                        // show_buf_c( buf , block_char_count , mask_c );
                                                        break;

                                                case '0':
                                                        // printf("ZERO\n");
                                                        buf[buf_index] = (buf[buf_index]) & ~(mask_o3 << bit_position);
                                                        bit_position++;
                                                        // show_buf_c( buf , block_char_count , mask_c );
                                                        break;
                                                }
                                        }
                                }
                        }
                }

                printf("******** buf bits after ********\n");
                print_buf_o3(buf, block_o3_count, mask_o3);

                fwrite(buf, sizeof(short), block_o3_count, out);

                free(buf);
        }

        free(codes);
        free_tree(root);
        free(root);
        free_heap(heap);
        fclose(in);
        fclose(out);
        return 0;

}
