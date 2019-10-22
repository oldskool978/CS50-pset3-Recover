#include "FSRELib.h"


int main(int argc, char *argv[])
{
    //check syntax
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image");
        return 1;
    }
    //open memory card file
    FILE *inptr = fopen(argv[1], "rb");
    while (inptr == NULL)
    {
        fprintf(stderr, "unable to open file");
        return 2;
    }

    //Variable for outfile name
    int VersionControl = 0;

    //Itterate through photo's
    long endFile = OpenEOF(inptr);
    while (ftell(inptr) != endFile)
    {
        char fileName[64] = {0};
        sprintf(fileName, "%03i.jpg", VersionControl);
        FILE *outptr = fopen(fileName, "wb");
        while (outptr == NULL)
        {
            fprintf(stderr, "unable to create file");
            return 2;
        }
        //process image
        imageParse(inptr, outptr);
        VersionControl++;
    }
    fclose(inptr);

    return 0;
}




