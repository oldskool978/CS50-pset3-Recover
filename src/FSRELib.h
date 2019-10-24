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

//imageParse extracts a single jpg, NOTE* Does not close InFile. can be ran ittiratavely 
int imageParse(FILE *ParseIn, FILE *ParseOut);


//reports location of EOF
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
    int ExitCase = -1;
    do
    {
        for (int i = 0; i < FSIO; i++)
        {
            int Paint = fgetc(ParseIn);
            putc(Paint, ParseOut);
            if (ExitCase == 0)
            {
                if (Paint != 0xD9) 
                    ExitCase--;
                else
                {
                    int SkipSpace = FSIO - ftell(ParseIn) % FSIO;
                    fclose(ParseOut);
                    fseek(ParseIn, SkipSpace, SEEK_CUR);
                    return ftell(ParseIn);
                }
                
            }
            if (Paint == 0xFF)
                ExitCase = 0;      
        }
        fread(&Check, sizeof(MagicP), 1, ParseIn);
        fseek(ParseIn, -(long)sizeof(MagicP), SEEK_CUR);
    }
    while (Check.MP1 != Hit.MP1 |
           Check.MP2 != Hit.MP2 |
           Check.MP3 != Hit.MP3 |
           Check.MP4  > 0xef    |
           Check.MP4 < 0xe0     );

    fclose(ParseOut);
    return ftell(ParseIn);
}

