//известен алфавит + колво неиспольщуемых битов

#include<stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include<string.h>

#define MAX1 256 
#define MAX2 65536

typedef union Character {
        short s;
        char c;
} Character;

typedef struct Code
{
        Character ch;
        char code[65536];
        int length;
} Code;

/*typedef struct Code
{
        char ch;
        char code[256];
        int length;
} Code;*/



typedef struct string {
        char *block;
} block_str; 


void print_codes( Code *codes , int leaves_count , int option ) {
        switch(option) {
        case 1:
        for( int i = 0 ; i < leaves_count ; i++ ) 
                printf("'%c' - %s (length = %d)\n", codes[i].ch.c , codes[i].code , codes[i].length );
                break;
        case 2:
        for( int i = 0 ; i < leaves_count ; i++ ) 
                printf("'%c' - %s (length = %d)\n", codes[i].ch.s , codes[i].code , codes[i].length );
                break;
        }
}


void reverse( block_str *blocks , int lSize , int block_bits ) {
                char tmp;
                for( int i = 0 ; i < lSize ; i++ ) {
                        for(int j = 0 ; j < block_bits / 2 ; j++ ){
                        tmp = blocks[i].block[j];
                        blocks[i].block[j] = blocks[i].block[block_bits-j-1];
                        blocks[i].block[block_bits-j-1] = tmp ;
                        }
                }
}


void zeruj( char *w_arr , int option ) {
        switch(option) {
        case 1:
        for( int i = 0 ; i < MAX1 ; i++ )
                w_arr[i] = '\0';
        break;
        case 2:
        for( int i = 0 ; i < MAX2 ; i++ )
                w_arr[i] = '\0';
        break;        
        }
}

void delete( char* single_str , int excess_bits ) {
        int len = strlen(single_str);
        for( int i = len - excess_bits ; i < len ; i++ )
                single_str[i] = '\0';
}

int main( int argc , char *argv[] ) {

        int excess_bits, leaves_count, option;
        FILE *kody = fopen( "kody", "r" );
        fscanf(kody, "%d %d %d", &excess_bits, &leaves_count, &option);
        //printf( "%d %d\n", excess_bits, leaves_count);

        Code *codes = malloc( leaves_count * sizeof(*codes));

        switch(option) {
                case 1:
                for( int i = 0 ; i < leaves_count ; i++ )
                        for( int j = 0 ; j < MAX1 ; j++ )
                                codes[i].code[j] = '\0';
                break;
                case 2:
                for( int i = 0 ; i < leaves_count ; i++ )
                        for( int j = 0 ; j < MAX2 ; j++ )
                                codes[i].code[j] = '\0';
                break;                     
        }

        switch(option) {
                case 1:
                for( int i = 0; i < leaves_count; ++i )
                {
                        int c;
                        fscanf(kody, "%d %256s", &c, codes[i].code);
                        codes[i].ch.c = (char) c;
                        codes[i].length = strlen(codes[i].code);
                }
                break;
                
                case 2:
                for( int i = 0; i < leaves_count; ++i )
                {
                        int c;
                        fscanf(kody, "%d %65536s", &c, codes[i].code);
                        codes[i].ch.s = (short) c;
                        codes[i].length = strlen(codes[i].code);
                }
                break;
        }

        print_codes( codes , leaves_count , option );

        FILE *f = argc > 1 ? fopen( argv[1] , "rb") : NULL ;
        FILE *in = argc > 2 ? fopen( argv[2] , "wb") : NULL ;
       
                if( f == NULL )
                {
                fprintf(stderr,"Empty file\n");
                exit(1);
                }

                if( in == NULL )
                {
                fprintf(stderr,"Empty file\n");
                exit(1);
                }

        int block_bits;
        switch(option) {
                case 1:
                block_bits = 8;
                break;

                case 2:
                block_bits = 16;
                break;
        } 

        fseek( f , 0 , SEEK_END);      // устанавливаем позицию в конец файла
        long lSize = ftell(f);        // получаем размер в байтах
        rewind(f);                      //возвращаем в начало файла

        //printf("Size in bytes - %ld \n", lSize );
        char mask = 0b1;
        short mask_s = 0b1;

        char *buffer = (char*) malloc(sizeof(char) * lSize);
        short *buffer_s = (short*) malloc(sizeof(short) * lSize);

        int bits_at_all = lSize * block_bits ;  // для единой строки 
        char *single_str = malloc( sizeof(char) * bits_at_all );

        block_str *blocks = malloc( sizeof(*blocks) * lSize ); 

                for( int i = 0 ; i < lSize ; i++ )
                blocks[i].block = /* malloc(sizeof(char) * 8 ) */ calloc( block_bits , sizeof(char) );    //op

        /*for( int i = 0 ; i < lSize ; i++ )
                for( int j = 0 ; j < 8 ; j++ ) 
                        blocks[i].block[j] = '\0';*/

        //for( int i = 0 ; i < lSize ; i++ ) 
                //printf("arr[%d]: %s\n", i , blocks[i].block);        



         if( buffer == NULL ) {
                fprintf(stderr, "ERROR WITH MEMORY\n");
                exit(1);
        }

        size_t result;   
        switch( option ) {
                case 1:     
                        result = fread(buffer, sizeof(char) , lSize, f );       // считываем файл в буфер
                      /*  if (result != lSize) {
                                fprintf(stderr,"Error with memory");
                        exit (3);
                        } */
                        break;
                case 2:
                        result = fread(buffer_s, sizeof(short) , lSize / 2 + 1 , f );       // считываем файл в буфер
                        break;           
        } 
                

        //printf("Soderzimoe faila teper nahoditsa v buffere\n");

        switch(option) {
                case 1:
        for( int j = 0 ; j < lSize ; j++ ){
                //printf("buffer[%d]: ", j );
                for( int i = block_bits-1 ; i >= 0 ; i-- ) {
                        //printf("%d", ( (buffer[j]) >> i ) & mask );
                        switch(( (buffer[j]) >> i ) & mask) {
                                case 0:
                                        strcat( blocks[j].block , "0" );
                                        break;
                                case 1: 
                                        strcat( blocks[j].block , "1" );
                                        break;        
                                }        
                        }
                //printf("\n");
                }
                break;

                case 2:
        for( int j = 0 ; j < lSize / 2 ; j++ ){
                //printf("buffer[%d]: ", j );
                for( int i = block_bits-1 ; i >= 0 ; i-- ) {
                        //printf("%d", ( (buffer[j]) >> i ) & mask );
                        switch(( (buffer_s[j]) >> i ) & mask_s) {
                                case 0:
                                        strcat( blocks[j].block , "0" );
                                        break;
                                case 1: 
                                        strcat( blocks[j].block , "1" );
                                        break;        
                                }        
                        }
                //printf("\n");
                }
                break;
        }
        
        reverse( blocks , lSize , block_bits );
        //printf("Reversed: \n");
        //for( int i = 0 ; i < lSize ; i++ ) 
                //printf("arr[%d]: %s\n", i , blocks[i].block); 
        
        for( int i = 0 ; i < lSize ; i++ )
                strcat( single_str , blocks[i].block);
        //printf("%s\n", single_str );


        delete( single_str , excess_bits );
        //printf("%s\n", single_str );
        



        
        char w_arr[65536]; //work_array
                switch(option) { 
                case 1:
                for( int i = 0 ; i < MAX1 ; i++ )
                        w_arr[i] = '\0'; 
                break;

                case 2:
                for( int i = 0 ; i < MAX2 ; i++ )
                        w_arr[i] = '\0'; 
                }

        //int n = lSize * block_bits - excess_bits ;
        int n = strlen(single_str);                     // в делит бахнуть
        
        char c;
        short s;
        int wai = 0; //work_array_index 
        switch(option) {
                case 1:
                        for( int i = 0 ; i < n ; i++ ) {
                        w_arr[wai++] = single_str[i];
                        for( int j = 0 ; j < leaves_count ; j++ ) 
                                if( strcmp( w_arr , codes[j].code ) == 0 ) {
                                        c = codes[j].ch.c;
                                        fwrite( &c , sizeof(char) , 1 , in );
                                        zeruj(w_arr, option);
                                        wai = 0;            
                                } 
                        }
                break;

                case 2:
                        for( int i = 0 ; i < n ; i++ ) {
                        w_arr[wai++] = single_str[i];
                        for( int j = 0 ; j < leaves_count ; j++ ) 
                                if( strcmp( w_arr , codes[j].code ) == 0 ) {
                                        s = codes[j].ch.s;
                                        fwrite( &s , sizeof(short) , 1 , in );
                                        zeruj(w_arr, option);
                                        wai = 0;            
                                } 
                        }

        }


        fclose (f);
        free (buffer);
        return 0; 
}
