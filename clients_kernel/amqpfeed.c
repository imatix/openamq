/*
    Test program

    Creates randomized file with specified size and name.
   */

#include "sfl.h"
#define BUFFER_SIZE  32000

int main (int argc, char *argv [])
{
    FILE
        *output;
    long
        byte_nbr,
        chunk_size,
        file_count,
        remaining;
    char
        filename [LINE_MAX];
    byte
        buffer [BUFFER_SIZE];

    if (argc < 3) {
        coprintf ("Syntax: amqpfeed name size [count]");
        exit (0);
    }
    if (argc == 4)
        file_count = atol (argv [3]);
    else
        file_count = 1;

    strcpy (filename, argv [1]);
    while (file_count > 0) {
        output = fopen (filename, FOPEN_WRITE_BINARY);
        if (output == NULL) {
            coprintf ("E: can't create '%s': %s", filename, strerror (errno));
            exit (0);
        }
        randomize ();
        for (byte_nbr = 0; byte_nbr < BUFFER_SIZE; byte_nbr++)
            buffer [byte_nbr] = random (256);

        remaining = atol (argv [2]);
        while (remaining > 0) {
            chunk_size = min (remaining, BUFFER_SIZE);
            fwrite (buffer, chunk_size, 1, output);
            remaining -= chunk_size;
        }
        fclose (output);
        sprintf (filename, "%s.%ld", argv [1], file_count);
        file_count--;
    }
    return (0);
}
