#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cs50.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./recover image");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    int cnt = 0;            // count number of image
    char filename[10];      // filename
    BYTE buffer[512 + 5];   // buffer to restored data 512 Byte

    // find the fisrt context of JPG format form
    while (true)
    {
        fread(buffer, 512, 1, file);
        bool isJPG = buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0;
        if (isJPG)
        {
            break;
        }
    }

    // After loop while above we found first position in buffer
    // So, let create first image
    bool newJPG = true;
    FILE *img;
    while (true)
    {
        // If we found first image
        if (newJPG)
        {
            // Create them by new name from count image
            sprintf(filename, "%03i.jpg", cnt++);

            // Make new image follow by filename from above
            img = fopen(filename, "w");
            fwrite(buffer, 512, 1, img);

            // Set new state of newJPG is not first block of image
            newJPG = false;
        }
        else
        {
            // If new data from file is empty or not full 1 item
            if (fread(buffer, 512, 1, file) != 1)
            {
                break; // End of process
            }

            // Let isJPG is state of current buffer that is new header of JPG image
            bool isJPG = buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0;

            // If not new image
            if (!isJPG)
            {
                // Write back-to-back the image
                fwrite(buffer, 512, 1, img);
            }
            else // If new image
            {
                // Close old one
                fclose(img);

                // And set next time is new image with data in cuurect buffer
                newJPG = true;
            }
        }
    }
    fclose(file);
}
