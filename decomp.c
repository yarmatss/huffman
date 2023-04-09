//известен алфавит + колво неиспольщуемых битов

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Code
{
        char ch;
        char code[256];
        int length;
} Code;

typedef struct string {
        char *block;
} string; 

void print_codes( Code *codes , int leaves_count ) {
        for( int i = 0 ; i < leaves_count ; i++ ) 
                printf("'%c' - %s (length = %d)\n", codes[i].ch , codes[i].code , codes[i].length );
}

void reverse( string *blocks , int lSize ) {
        char tmp;
        for( int i = 0 ; i < lSize ; i++ ) {
                for(int j = 0 ; j < 8 / 2 ; j++ ){
                tmp = blocks[i].block[j];
                blocks[i].block[j] = blocks[i].block[8-j-1];
                blocks[i].block[8-j-1] = tmp ;
                }
        }
}

void zeruj( char *w_arr ) {
        for( int i = 0 ; i < 256 ; i++ )
                w_arr[i] = '\0';
}

int main( int argc , char *argv[] ) {

        int excess_bits, leaves_count;
        FILE *kody = fopen( "kody", "r" );
        fscanf(kody, "%d %d", &excess_bits, &leaves_count);
        printf( "%d %d\n", excess_bits, leaves_count);

        Code *codes = malloc( leaves_count * sizeof *codes);

        for( int i = 0 ; i < leaves_count ; i++ )
                for( int j = 0 ; j < 256 ; j++ )
                        codes[i].code[j] = '\0';

        for( int i = 0; i < leaves_count; ++i )
        {
                int c;
                fscanf(kody, "%d %256s", &c, codes[i].code);
                codes[i].ch = (char) c;
                codes[i].length = strlen(codes[i].code);
        }

        print_codes( codes , leaves_count );

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

        int block_bits = 8;


        fseek( f , 0 , SEEK_END);      // устанавливаем позицию в конец файла
        long lSize = ftell(f);        // получаем размер в байтах
        rewind(f);                      //возвращаем в начало файла

        //printf("Size in bytes - %ld \n", lSize );
        char mask = 0b1;
        char *buffer = (char*) malloc(sizeof(char) * lSize);

        string *blocks = malloc( sizeof(string) * lSize ); 
                for( int i = 0 ; i < lSize ; i++ )
                        blocks[i].block = malloc(sizeof(char) * 8 );
        for( int i = 0 ; i < lSize ; i++ )
                for( int j = 0 ; j < 8 ; j++ ) 
                        blocks[i].block[j] = '\0';

        for( int i = 0 ; i < lSize ; i++ ) 
                printf("arr[%d]: %s\n", i , blocks[i].block);        
                                  
         if( buffer == NULL ) {
                fprintf(stderr, "ERROR WITH MEMORY\n");
                exit(1);
        }

               
        size_t result = fread(buffer, 1, lSize, f );       // считываем файл в буфер
                if (result != lSize)
                {
                fprintf(stderr,"Error with memory");
                exit (3);
                }

        printf("Soderzimoe faila teper nahoditsa v buffere\n");

        for( int j = 0 ; j < lSize ; j++ ){
                printf("buffer[%d]: ", j );
                for( int i = 7 ; i >= 0 ; i-- ) {
                        printf("%d", ( (buffer[j]) >> i ) & mask );
                        switch(( (buffer[j]) >> i ) & mask) {
                                case 0:
                                        strcat( blocks[j].block , "0" );
                                        break;
                                case 1: 
                                        strcat( blocks[j].block , "1" );
                                        break;        
                        }        
        }
                printf("\n");
        }
        
        reverse( blocks , lSize );
        printf("Perevernutye: \n");
           
        for( int i = 0 ; i < lSize ; i++ ) 
                printf("arr[%d]: %s\n", i , blocks[i].block); 

        int bits_in_use = 8 * lSize - excess_bits ;

        char w_arr[256]; //work_array 
        for( int i = 0 ; i < 256 ; i++ )
                w_arr[i] = '\0';       



        int code_i = 0;
        int code_i_w = 0;
        char c;


        for( int i = 0 ; i < lSize ;  ) {
                if( code_i == 8 ) {
                        i++;
                        code_i = 0;
                }        
                w_arr[code_i_w] = blocks[i].block[code_i];
                code_i_w++;
                code_i++;
                        for( int j = 0 ; j < leaves_count ; j++ )
                                if( strcmp( w_arr , codes[j].code ) == 0 ) {
                                        c = codes[j].ch;
                                        fwrite( &c , sizeof(char) , 1 , in );
                                        zeruj(w_arr);
                                        code_i_w = 0;
                                }
                
        }









/*

        int n = 1; //сколько записываем 
        char c;
        for( int i = 0 ; i < lSize ; i++ ) {
                strncat( w_arr , n , blocks[i].block );   
                for( int j = 0 ; j < leaves_count ; j++ ) {

                        if( strcmp(codes[j].code , w_arr) == 0 ) {
                                c = codes[j].ch;
                                fwrite( &c , sizeof(char) , 1 , in );
                        }        
                        zeruj( w_arr );
                        n++;
                }
        }

*/        



        fclose (f);
        free (buffer);
        return 0; 
}
