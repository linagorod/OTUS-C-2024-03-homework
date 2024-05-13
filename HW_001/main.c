#include <stdio.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <string.h>

#define STR_LENGTH      4
#define ORIGIN_ZIP_FL   24 
#define ORIGIN_ZIP_FN   15 

void findFileName(FILE *_File)
{
    char buf[3];
    
    fseek(_File, ORIGIN_ZIP_FL, SEEK_CUR);
    fread(buf, sizeof(char), 3, _File);

    unsigned int filelen = (buf[0]|(buf[1]<<8))&0xffff;

    char filename[filelen+1];

    fseek(_File, ORIGIN_ZIP_FN, SEEK_CUR);
    fread(filename, sizeof(char), filelen, _File);
    filename[filelen] = 0;

    printf("Найден файл архива: %s\n", filename);
}

int main(int argc, char *argv[])
{
    const char zip_signature[]= "\x50\x4b\x01\x02";
    const char zipend_signature[]= "\x50\x4b\x05\x06";
    FILE *fptr;
    char databuf2[STR_LENGTH+1];
    struct stat statbuf;
    long int result, files_found;
    char zip_found = 0; 

    for (int ii = 1; ii<argc; ii++)
    {
            fptr = fopen(argv[ii], "rb");
            result = fstat(fileno (fptr), &statbuf);

            if (result != 0)
            {
                printf("Ошибка ввода! Некорректный путь! \n");
            }
            else
            {
                printf("\nОткрыт файл: %s \n", argv[ii]);
                zip_found=0; files_found=0;

                while (fread(databuf2, sizeof(char), STR_LENGTH, fptr) && (ftell(fptr)<statbuf.st_size))
                {
                    databuf2[STR_LENGTH] = 0;
                    if (strcmp(databuf2, zip_signature) == 0)
                    {
                        findFileName(fptr);
                        files_found++;
                    }

                    if (strcmp(databuf2, zipend_signature) == 0)
                    { 
                        zip_found = 1;
                        printf("Это архив! \n");
                    }

                    fseek(fptr, -STR_LENGTH+1, SEEK_CUR);
                }

                if (!zip_found) printf("Файл не является архивом!\n");
                else printf("Количество объектов в архиве: %ld\n", files_found);

                fclose(fptr);
            }
    }
    return 0;
}