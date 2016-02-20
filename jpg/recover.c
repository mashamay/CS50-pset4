/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
    // open the card 
    FILE* card = fopen("card.raw", "r");
    if (card == NULL)
    {
        printf("Could not open card.raw\n");
        return 2;
    }
    
    // count found images
    int counter = 0;
    
    // image file
    FILE* image = NULL;
    
    // untill end of file
    while(!feof(card))
    {
        
        // define buffer of 512 bytes
        uint8_t buffer[512];
        
        // create decimal 3 digit number for title
        char filename[8];
        
        // check first four bytes
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] == 0xe0 || buffer[3] == 0xe1)) 
        {
            // check whether image file is open, if it is - close it
            if (image != NULL)
            {
                fclose(image);
            } 
        
            // name the file with three digits
            sprintf(filename, "%03d.jpg", counter);
    
            // open new file for writing
            image = fopen(filename, "w");
        
            // increase the count by 1 
            counter++;
        
            // write bytes to the file
            fwrite(buffer, 512, 1, image);
        }
         
        else if(counter > 0)
        {
            fwrite(buffer, 512, 1, image);
        } 
        // read 512 bytes
        fread(buffer, 512, 1, card);      
    } 
    
    // close files
    fclose(card);
    fclose(image);

    // that's all folks
    return 0;      
}
