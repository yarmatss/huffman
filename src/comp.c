#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "minheap.h"
#include "huffman.h"

#define MAX1 256
#define MAX2 65536

int main(int argc, char **argv)
{
        int option = 0, opt;
        while ((opt = getopt(argc, argv, "o:h")) != -1)
        {
                switch (opt)
                {
                case 'h':
                        printf("Usage:\ncomp <nazwa pliku do kompresji> <nazwa pliku skompresowanego> [-o stopień kompresji]\n");
                        exit(1);
                        break;
                case 'o':
                        option = atoi(optarg);
                        break;
                }
        }

        if (option < 1 || option > 2)
        {
                fprintf(stderr, "Błędny lub niepodany stopień kompresji!\nUżyj -h dla wyświetlenia menu help\n");
                exit(1);
        }

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

        int count[MAX2] = {0};

        int leaves_count = 0;
        char c1;
        unsigned short c2;

        switch (option)
        {
        case 1:
                while (fread(&c1, sizeof(char), 1, in))
                {
                        count[c1]++;
                        if (count[c1] == 1)
                                leaves_count++;
                }
                break;
        case 2:
                while (fread(&c2, sizeof(short), 1, in))
                {
                        count[c2]++;
                        if (count[c2] == 1)
                                leaves_count++;
                }
                break;
        }

        if (leaves_count == 0)
        {
                fprintf(stderr, "Pusty plik \"%s\"\n", argv[1]);
                fclose(in);
                fclose(out);
                exit(1);
        }

        minHeap *heap = create_min_heap(2 * leaves_count - 1);
        if (!heap)
        {
                fprintf(stderr, "Nie udało się stworzyć sterty\n");
                exit(1);
        }

        switch (option)
        {
        case 1:
                for (int i = 0; i < MAX1; i++)
                        if (count[i] != 0)
                        {
                                Character tmp;
                                tmp.c = (char)i;
                                insert(heap, tmp, count[i], NULL, NULL);
                        }
                break;
        case 2:
                for (int i = 0; i < MAX2; i++)
                        if (count[i] != 0)
                        {
                                Character tmp;
                                tmp.s = (short)i;
                                insert(heap, tmp, count[i], NULL, NULL);
                        }
                break;
        }

        Node *root = create_tree(heap);
        if (!root)
        {
                fprintf(stderr, "Nie udało się stworzyć drzewo\n");
                exit(1);
        }
        Code *codes = calloc(leaves_count, sizeof(Code));
        if (!codes)
        {
                fprintf(stderr, "Nie udało się stworzyć tablicy kodów\n");
                exit(1);
        }

        get_code(root, codes, 0);

        int bits_in_use = BITS_IN_USE(leaves_count, codes);

        FILE *table = fopen("kody", "w");
        if (table == NULL)
        {
                fprintf(stderr, "Nie udało sie stworzyć pliku \"%s\"\n", argv[2]);
                exit(1);
        }

        zapisz(in, out, table, option, bits_in_use, leaves_count, codes);

        free(codes);
        free_tree(root);
        free(root);
        free_heap(heap);
        fclose(in);
        fclose(out);
        fclose(table);
        return 0;
}
