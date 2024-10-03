/*Tristan Garcia
*This is the main file for the color blindess simulator
*This program reads the file that the user inputs and then 
*converts the colors and then gives an output file of the 
*new changed image
*/

#include "bmp.h"
#include "io.h"

#include <unistd.h>

#define OPTIONS "i:o:h"

int main(int argc, char **argv) {
    int opt = 0;

    //use my own error msg for get opt failures
    opterr = 0;
    bool option_i = false;
    bool option_o = false;
    bool option_h = false;

    FILE *infile = stdin;
    FILE *outfile = stdout;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            infile = fopen(optarg, "rb");
            if (infile == NULL) {
                fprintf(stderr, "colorb:  error reading input file %s\n", optarg);
                option_h = true;
            }
            option_i = true;
            break;
        case 'o':
            outfile = fopen(optarg, "wb");
            if (outfile == NULL) {
                fprintf(stderr, "colorb:  error opening output file %s\n", optarg);
                option_h = true;
            }
            option_o = true;
            break;
        case 'h': option_h = true; break;
        default:
            fprintf(stderr, "colorb:  unknown or poorly formatted option -%c\n", optopt);
            printf("Usage: colorb -i infile -o outfile\n");
            printf("       colorb-h\n");
            option_h = true;
            return 0;
        }
    }

    if (option_h) {
        printf("Usage: colorb -i infile -o outfile\n");
        printf("       colorb-h\n");
        return 0;
    }

    // BMP *pbmp = (BMP *)calloc(1, sizeof(BMP));

    if (option_i && option_o) {
        BMP *image = bmp_create(infile);

        if (image == NULL) {
            fprintf(stderr, "colorb:  error creating BMP structure");
            fclose(infile);
            fclose(outfile);
        }

        bmp_reduce_palette(image);

        bmp_write(image, outfile);

        bmp_free(&image);
        fclose(outfile);
        fclose(infile);
    }
}
