#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//set MagicP to 4 bytes to be detected by parser; MP1 ...MP4
typedef struct
{
    uint8_t MP1;
    uint8_t MP2;
    uint8_t MP3;
    uint8_t MP4;
} MagicP;


//OpenEOF reports back the final byte of a file
long OpenEOF(FILE *InFile);

//imageParse extracts a single jpg, continues to export bytes untils EOF (exit case) or new jpg found.  NOTE* Does not close InFile. can be ran ittiratavely 
int imageParse(FILE *ParseIn, FILE *ParseOut);


//checks for EOF
long OpenEOF(FILE *InFile)
{
    long OriginalP = ftell(InFile);
    fseek(InFile, 0, SEEK_END);
    long EndP = ftell(InFile);
    fseek(InFile, OriginalP, SEEK_SET);
    return EndP;
}



//writes next JPG returns last position in file
int imageParse(FILE *ParseIn, FILE *ParseOut)
{
    //FileSystem Bs
    int FSIO = 512;
    //Set Struct to detect Jpeg
    MagicP Hit = {0xff, 0xd8, 0xff, 0x00};
    MagicP Check = {0};
    while (Check.MP1 != Hit.MP1 &&
           Check.MP2 != Hit.MP2 &&
           Check.MP3 != Hit.MP3 &&
           Check.MP4 > 0xef |
           Check.MP4 < 0xe0)
    {
        fread(&Check, sizeof(MagicP), 1, ParseIn);
    }
    fseek(ParseIn, -(long)sizeof(MagicP), SEEK_CUR);
    do
    {
        for (int i = 0; i < FSIO; i++)
        {
            int Paint = fgetc(ParseIn);
            //Close files and return 0 in case of EOF
            if (feof(ParseIn))
            {
                fclose(ParseIn);
                fclose(ParseOut);
                exit(0);
            }
            putc(Paint, ParseOut);
        }
        fread(&Check, sizeof(MagicP), 1, ParseIn);
        fseek(ParseIn, -(long)sizeof(MagicP), SEEK_CUR);
        
        //*TODO* Check for end of jpg "0xFF && 0xD9"
    }
    while (Check.MP1 != Hit.MP1 |
           Check.MP2 != Hit.MP2 |
           Check.MP3 != Hit.MP3);

    fclose(ParseOut);
    return ftell(ParseIn);
}

