#include <stdio.h>
#include <stdlib.h>

#include "parser.tab.h"
#include "codegen.h"

int main(int argc, char **argv)
{
    if (argc != 5 && argc != 2)
    {
        fprintf(stderr, "Incorrect usage, exitting...\n");
        return EXIT_FAILURE;
    }
    if (argc == 5)
    {
        yyin = fopen(argv[2], "r");
        if (yyin == NULL)
        {
            fprintf(stderr, "Unable to open source file, exitting...\n");
            return EXIT_FAILURE;
        }
        outFile = fopen(argv[4], "w");
        if (outFile == NULL)
        {
            fprintf(stderr, "Unable to open output file for writting, exitting...\n");
            fclose(yyin);
        }
    }
    else
    {
        fprintf(stderr, "No output file specified, outputing to STDOUT...\n");
    }



    fclose(yyin);
    if (argc == 5)
    {
        fclose(outFile);
    }

    return EXIT_SUCCESS;
}
