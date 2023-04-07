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

typedef struct Code {
        char ch;
        char code[256];
        int freq; // dla obliczenia
        int length; // for bits operations 
} Code;

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

char code_g[256];  // code_getcode 
int ic = 0; //передвигаться по массиву структру с кодами

void get_code( Node *root , Code *codes , int cur ) {
        if( root->left )
        {
                code_g[cur] = '0';
                get_code( root->left, codes , cur + 1 );
        }

        if( root->right )
        {
                code_g[cur] = '1';
                get_code( root->right, codes , cur + 1 );
        }

        if( !(root->left) && !(root->right) )
        {
                printf( "%c -> ", root->ch );
                codes[ic].ch = root->ch;
                codes[ic].freq = root->freq;
                for( int i = 0; i < cur ; ++i ) {
                        codes[ic].code[i] = code_g[i];
                        if( i+1 == cur )
                                codes[ic].length = i+1;
                } 
                ic++;          
                for( int i = 0; i < cur; ++i ) 
                        printf( "%c", code_g[i] );

                printf( "\n" );
        }
}



//bits 


int BITS_IN_USE( int leaves_count , Code *codes ) {
        int n = 0;
        for( int i = 0 ; i < leaves_count ; i++ ) 
                n += (codes[i].freq) * codes[i].length ;
        return n;
}

// -o1
int BLOCK_O1_COUNT( int bits_in_use ) {
        int n;
        if( ( bits_in_use % 8 ) > 0 ) 
                n = ( bits_in_use / 8 ) + 1;
        else 
                n = bits_in_use / 8 ; 
        return n;
}

void print_buf_o1( unsigned char *buf , int n , char mask) {
        for( int i = 0 ; i < n ; i++ ) {
                printf("buf[%d]: " , i );
                for( int j = 7 ; j >= 0 ; j-- )
                        printf("%d" , ( (buf[i]) >> j ) & mask );
                printf("\n");        
        }
}

// -o2
int BLOCK_O2_COUNT( int bits_in_use ) {
        int n;
        if( ( bits_in_use % 16 ) > 0 ) 
                n = ( bits_in_use / 16 ) + 1;
        else 
                n = bits_in_use / 16 ; 
        return n;
}

void print_buf_o2( unsigned short *buf , int n , char mask) {
        for( int i = 0 ; i < n ; i++ ) {
                printf("buf[%d]: " , i );
                for( int j = 15 ; j >= 0 ; j-- )
                        printf("%d" , ( (buf[i]) >> j ) & mask );
                printf("\n");        
        }
}



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

        int option;
                if( strcmp( "-o1" , argv[3] ) == 0 )
                        option = 1;
                else if( strcmp( "-o2" , argv[3] ) == 0 )
                        option = 2;
                else if( strcmp( "-o3" , argv[3] ) == 0 )
                        option = 3;
                else {
                        fprintf(stderr,"Bledna opcja, podaj opcji -help\n");
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

        //print_heap(heap);


        Node *root = create_tree(heap);

       // print_tree(root);
        
        Code *codes = calloc( leaves_count , sizeof(Code) ); 
        
        printf( "\n\n\nCodes: \n" );
        get_code( root, codes , 0 );
        

        printf("----------");
        printf("\nstruct codes\n");
        for( int i = 0 ; i < leaves_count ; i++ ) {
                printf("%c - %s (length - %d)\n", codes[i].ch , codes[i].code , codes[i].length );
        }


        //now bits-----------------------------------------------------------------------------------------------------------------


        int bits_in_use = BITS_IN_USE( leaves_count , codes );
        printf( "bit in use - %d\n" , bits_in_use );      


        if( option == 1 ) { // opcja 1 - block 8 bitów  

        int block_o1_count = BLOCK_O1_COUNT( bits_in_use );
        printf("blocks count - %d\n", block_o1_count );


        unsigned char *buf = calloc( block_o1_count , sizeof(char) ) ;  
        if( buf == NULL ) {
                fprintf( stderr , "Memory Error\n");
                exit(1);
        }
        
       
        char mask_o1 = 0b1;
       
        printf("******** buf bits before *********\n");
        print_buf_o1( buf , block_o1_count , mask_o1 );
       
        
        //меняю биты на наши коды:

         int buf_index = 0; // dla buffera 
         int bit_position = 0; //dla peredvizenia po bitam 

         fseek( in , 0L , SEEK_SET ) ; // что бы ещё раз прочитать файл
         while ((c = fgetc(in)) != EOF)
        {
                for( int i = 0 ; i < leaves_count ; i++ )
                        {
                                if( (char) c == codes[i].ch ) {
                                        //printf("--%c--\n", c );
                                        for( int k = 0 ; k < codes[i].length ; k++ ) {
                                                if( bit_position == 8 ) { // block zavershen
                                                        buf_index ++ ;
                                                        bit_position = 0;
                                                        //printf("BUFF++\n");
                                                } 


                                                switch( codes[i].code[k] ) {
                                                        case '1':
                                                                //printf("JEDEN\n");
                                                                buf[buf_index] = (buf[buf_index]) | ( mask_o1 << bit_position );
                                                                bit_position++;
                                                                //show_buf_c( buf , block_char_count , mask_c );
                                                                break;

                                                        case '0':
                                                                //printf("ZERO\n");
                                                                buf[buf_index] = (buf[buf_index]) & ~( mask_o1 << bit_position );
                                                                bit_position++;
                                                                //show_buf_c( buf , block_char_count , mask_c );
                                                                break;
                                                }
                                        }
                                }          
                        }       

        } 

        
        printf("******** buf bits after ********\n"); 
        print_buf_o1( buf , block_o1_count , mask_o1 ); 

        fwrite( &buf , sizeof(char) , block_o1_count , out );

        
        free(buf);
        }




        if( option == 2 ) {  // opcja 2 - block 16 bitów
                
                int block_o2_count = BLOCK_O2_COUNT( bits_in_use );
                printf("blocks count - %d\n", block_o2_count );

                unsigned short *buf = calloc( block_o2_count , sizeof(unsigned short) ) ;  
                if( buf == NULL ) {
                        fprintf( stderr , "Memory Error\n");
                exit(1);
                 }
        
       
        short mask_o2 = 0b1;

        printf("\n******** buf bits before *********\n");
        print_buf_o2( buf , block_o2_count , mask_o2 );

        //меняю биты на наши коды:

         int buf_index = 0; // dla buffera 
         int bit_position = 0; //dla peredvizenia po bitam 

         fseek( in , 0L , SEEK_SET ) ; // что бы ещё раз прочитать файл
         while ((c = fgetc(in)) != EOF)
        {
                for( int i = 0 ; i < leaves_count ; i++ )
                        {
                                if( (char) c == codes[i].ch ) {
                                        //printf("--%c--\n", c );
                                        for( int k = 0 ; k < codes[i].length ; k++ ) {
                                                if( bit_position == 16 ) { // block zavershen
                                                        buf_index ++ ;
                                                        bit_position = 0;
                                                        //printf("BUFF++\n");
                                                } 


                                                switch( codes[i].code[k] ) {
                                                        case '1':
                                                                //printf("JEDEN\n");
                                                                buf[buf_index] = (buf[buf_index]) | ( mask_o2 << bit_position );
                                                                bit_position++;
                                                                //show_buf_c( buf , block_char_count , mask_c );
                                                                break;

                                                        case '0':
                                                                //printf("ZERO\n");
                                                                buf[buf_index] = (buf[buf_index]) & ~( mask_o2 << bit_position );
                                                                bit_position++;
                                                                //show_buf_c( buf , block_char_count , mask_c );
                                                                break;
                                                }
                                        }
                                }          
                        }       

        }

        printf("******** buf bits after ********\n"); 
        print_buf_o2( buf , block_o2_count , mask_o2 ); 

        fwrite( &buf , sizeof(short) , block_o2_count , out );
        free(buf);
        }

        free(codes);
        free_heap(heap);
        fclose(in);

        return 0;
}
