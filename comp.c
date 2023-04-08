#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
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

        printf("Codes: \n");
        get_code(root, codes, 0);

        printf("----------");
        printf("\nstruct codes\n");
        for (int i = 0; i < leaves_count; i++)
        {
                printf("%c - %s (length - %d)\n", codes[i].ch, codes[i].code, codes[i].length);
        }

        int bits_in_use = BITS_IN_USE(leaves_count, codes);
        printf("bit in use - %d\n", bits_in_use);

        zapisz(in, out, option, bits_in_use, leaves_count, codes);

        free(codes);
        free_tree(root);
        free(root);
        free_heap(heap);
        fclose(in);
        fclose(out);
        return 0;

}
