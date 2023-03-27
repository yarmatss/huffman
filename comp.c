#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int n = 0;

typedef struct Node
{
        char ch;
        int freq;
        struct Node *left;
        struct Node *right;
} Node;

void free_array(Node **a, int n)
{
        for (int i = 0; i < n; ++i)
                free(a[i]);
        free(a);
}

Node *create_node(char ch, int fr, Node *left, Node *right)
{
        Node *new_node = malloc(sizeof(*new_node));
        new_node->ch = ch;
        new_node->freq = fr;
        new_node->left = left;
        new_node->right = right;
        return new_node;
}

void swap(Node **a, Node **b)
{
        Node *tmp = *a;
        *a = *b;
        *b = tmp;
}

void heapify(Node **a, int n, int i)
{
        int min = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        if (l < n && a[l] < a[min])
                min = l;
        if (r < n && a[r] < a[min])
                min = r;

        if (min != i)
        {
                swap(&a[i], &a[min]);
                heapify(a, n, min);
        }
}

void heap_sort(Node **a, int n)
{
        // Build heap (rearrange array)
        for (int i = n / 2 - 1; i >= 0; i--)
                heapify(a, n, i);

        for (int i = n - 1; i >= 0; i--)
        {
                // Move current root to end
                swap(&a[0], &a[i]);

                // call min heapify on the reduced heap
                heapify(a, i, 0);
        }
}

void insert(Node **a, char ch, int freq, Node *left, Node *right)
{
        a[n++] = create_node( ch, freq, left, right );
        heap_sort( a, n );
}

void print_array(Node **a, int n)
{
        for (int i = 0; i < n; ++i)
                printf("%c -> %d\n", a[i]->ch, a[i]->freq);
}

void delete(Node **a, int v)
{
        swap(&a[v], &a[n - 1]);
        --n;
        heap_sort(a, n);
}

Node *copy(Node **a)
{
        return *a;
}

int main(int argc, char **argv)
{
        FILE *in = argc > 1 ? fopen(argv[1], "rb") : NULL;
        if (in == NULL)
        {
                fprintf(stderr, "Nie udało się odczytać pliku \"%s\"\n", argv[1]);
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

        Node **array = malloc(sizeof(*array) * ( 2 * leaves_count - 1 ) );

        for (int i = 0; i < 256; i++)
                if (count[i] != 0)
                        insert(array, (char)i, count[i], NULL, NULL);

        insert(array, 'y', 9, NULL, NULL);

        print_array(array, n);
        printf("---------------------\n");

        /*while( n > 1 )
        {
                int temp = array[0]->freq + array[1]->freq;
                Node *copyA = copy( &array[1] ), *copyB = copy( &array[0] );
                delete( array, n - 1 );
                delete( array, n - 1 );
                insert( array, 0, temp, copyA, copyB );
                print_array( array, n );
                printf( "---------------------\n" );
        }*/

        free_array(array, leaves_count);
        fclose(in);
        return 0;
}
