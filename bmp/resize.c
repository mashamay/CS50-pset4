/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Enlarges image by factor of n.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize factor infile outfile\n");
        return 1;
    }

    // remember factor
    int factor = atoi(argv[1]);
    
    if (factor < 1 || factor > 101)
    {
        printf("Usage: factor should be from 1 to 100 \n");
        return 1;
    }
    
    // remember filenames
    char* infile = argv[2];
    char* outfile = argv[3];

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
   
    // new file BITMAPFILEHEADER
    BITMAPFILEHEADER bfLarge;
    bfLarge = bf;
    
    // new file BITMAPINFOHEADER
    BITMAPINFOHEADER biLarge;
    biLarge = bi;
    biLarge.biWidth = bi.biWidth * factor;
    biLarge.biHeight = bi.biHeight * factor;  
    
    // determine padding for new file
    int LargePadding =  (4 - (biLarge.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // determine new image size
    biLarge.biSizeImage = (biLarge.biWidth * sizeof(RGBTRIPLE) + LargePadding) * abs(biLarge.biHeight);
    bfLarge.bfSize = biLarge.biSizeImage + 54;
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bfLarge, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&biLarge, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int smallPadding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    RGBTRIPLE* buffer = malloc(sizeof(RGBTRIPLE) * biLarge.biWidth);
    
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        
        int pixel = 0;
        // iterate over pixels in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            
            // repeat pixel 'factor' times
            for (int n = 0; n < factor; n++)
            {
                // add pixel to buffer
                buffer[pixel] = triple;
                pixel++;
            }
            
        }    

        // skip over padding, if any
        fseek(inptr, smallPadding, SEEK_CUR);

        // repeat the line 'factor' times
        for (int m = 0; m < factor; m++)
        {
            // write RGB triple to outfile
            fwrite(buffer, (sizeof(RGBTRIPLE) * biLarge.biWidth), 1, outptr);
            
            // then add it back (to demonstrate how)
            for (int k = 0; k < LargePadding; k++)
            {
                fputc(0x00, outptr);
            }
        }
        
    }

    // free buffer
    free(buffer);
    
    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
