#include<stdio.h>
#include<stdlib.h>

int main() {
        FILE *f = fopen("govno.bin" , "rb");
        char mask = 0b1;

        if( f == NULL )
        {
                fprintf(stderr,"Error with memory\n");
                exit(1);
        }
        fseek( f , 0 , SEEK_END);      // устанавливаем позицию в конец файла
        long lSize = ftell(f);        // получаем размер в байтах
        rewind(f);

        char *buffer = (char*) malloc(sizeof(char) * lSize);

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
                for( int i = 7 ; i >= 0 ; i-- )
                        printf("%d", ( (buffer[j]) >> i ) & mask );
                printf("\n");
        }

        fclose (f);
        free (buffer);
        return 0;
}
