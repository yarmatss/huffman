#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
        char ch;
        int freq;
        struct Node *left;
        struct Node *right;
} Node;

typedef struct Heap
{
        Node **array;
        int size;     // current size of heap
        int capacity; // maximum capacity
} minHeap;

Node *create_node(char ch, int fr, Node *left, Node *right)
{
        Node *new_node = malloc(sizeof(*new_node));
        new_node->ch = ch;
        new_node->freq = fr;
        new_node->left = left;
        new_node->right = right;
        return new_node;
}

int parent(int i) // i - index of array
{
        return (i - 1) / 2;
}

int left_child(int i)
{
        return (2 * i + 1);
}

int right_child(int i)
{
        return (2 * i + 2);
}

Node *get_min(minHeap *heap)
{
        return heap->array[0];
}

minHeap *create_min_heap(int capacity)
{
        minHeap *heap = malloc(sizeof(*heap));
        heap->array = malloc(sizeof(Node *) * capacity);
        heap->capacity = capacity;
        heap->size = 0;
        return heap;
}

void swap(Node **elem1, Node **elem2)
{
        Node *tmp = *elem1;
        *elem1 = *elem2;
        *elem2 = tmp;
}

minHeap *insert(minHeap *heap, char ch, int freq, Node *left, Node *right)
{
        if (heap->capacity == heap->size)
        {
                fprintf(stderr, "Error with num of elements\n");
                exit(1);
        }

        heap->size++;
        heap->array[heap->size - 1] = create_node(ch, freq, left, right);

        int cur = heap->size - 1; // current
        while (cur > 0 && heap->array[parent(cur)]->freq > heap->array[cur]->freq)
        {
                // потом свап написать всесто этого говна
                swap(&heap->array[parent(cur)], &heap->array[cur]);
                /*Node *tmp = heap->array[parent(cur)] ;
                heap->array[parent(cur)] = heap->array[cur];
                heap->array[cur] = tmp;*/
                cur = parent(cur); // update current index
        }
        return heap;
}

minHeap *heapify(minHeap *heap, int i)
{ // i - index
        if (heap->size <= 1)
        {
                return heap;
        }
        int left = left_child(i);
        int right = right_child(i);
        int min = i;
        if (left < heap->size && heap->array[left]->freq < heap->array[i]->freq)
                min = left;
        if (right < heap->size && heap->array[right]->freq < heap->array[min]->freq)
                min = right;

        if (min != i)
        {
                swap(&heap->array[i], &heap->array[min]);
                /*Node *tmp = heap->array[i];
                heap->array[i] = heap->array[min];
                heap->array[min] = tmp;*/
                heap = heapify(heap, min);
        }

        return heap;
}


minHeap *delete(minHeap *heap)
{
        if (heap == NULL || heap->size == 0)
        {
                return heap;
        }

        int size = heap->size;
        //Node *last_element = heap->array[size - 1];
        //heap->array[0] = last_element;
        swap( &(heap->array[0]) , &(heap->array[size-1]) );
        heap->size--;
        size--;
        heap = heapify(heap, 0);
        return heap;
}

void free_heap(minHeap *heap)
{
        if (heap == NULL)
                return;
        free(heap->array);
        free(heap);
}

void print_heap(minHeap *heap)
{
        printf("min heap: \n");
        for (int i = 0; i < heap->size; i++)
                printf("%c - %d\n", heap->array[i]->ch, heap->array[i]->freq);
}

Node *copy(Node **elem)
{
        return *elem;
}

Node *create_tree(minHeap *heap )
{
        while ( heap->size != 1)
        {
                Node *a = get_min(heap);
                printf( "%c - %d -\n" , a->ch , a->freq);
                delete(heap);
                
                Node *b = get_min(heap);
                delete(heap);
               
                insert(heap , '\0' , a->freq + b->freq , a , b );
                printf( "---------\n");
        }
        return heap->array[0];
}

void print_tree( Node *root ) {
        if( root == NULL ) {
                printf("----empty----\n");
                return;
        }
        printf("symbol - %c , value - %d\n ", root->ch , root->freq);
        printf("left:\n");
        print_tree(root->left);
        printf("right:\n");
        print_tree(root->right);
        printf("done\n");
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

        minHeap *heap = create_min_heap(2 * leaves_count - 1);

        for (int i = 0; i < 256; i++)
                if (count[i] != 0)
                        insert(heap, (char)i, count[i], NULL, NULL);

        print_heap(heap);


        Node *root = create_tree(heap);

        print_tree(root);
        

        free_heap(heap);
        fclose(in);

        return 0;
}
