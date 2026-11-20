/*
* Name        : Gangarapu VeeraShankar
* Batch ID    : 26018F
* Project     : Image Steganography (LSB Encoding and Decoding)
* File        : decode.c
* Description : Decoding part of the Image Steganography project written in C.
*               It reads the LSB of the image bytes of a stego .bmp image to get back the hidden secret file.
*               The magic string is checked first, then the extension, file size and file data are decoded.
*/
// Standard input/output functions (fopen, fread, fwrite)
#include <stdio.h>
// String functions (strlen, strcmp, strcpy)
#include <string.h>
// Standard library functions
#include <stdlib.h>
// User defined types (Status)
#include "types.h"
// Magic string definition
#include "common.h"
// Decoding structure and function declarations
#include "decode.h"

// Checks the command line arguments and stores the file names
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // argv[2] = stego.bmp , argv[3] = output file (optional)
    // Stego image name is mandatory
    if (argv[2] == NULL)
        return e_failure;

    // Store the stego image name
    decInfo->stego_image_fname = argv[2];

    // Output file name is optional
    if (argv[3] != NULL)
        // Use the name given by the user
        decInfo->secret_fname = argv[3];
    else
        // Use the default output name
        decInfo->secret_fname = "decoded_output"; // default

    return e_success;
}

// Opens the stego image and the output file
Status open_decode_files(DecodeInfo *decInfo)
{
    // Open the stego image for reading
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    // Open the output file for writing
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "wb");
    if (decInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
        return e_failure;
    }
    return e_success;
}

// Collects the LSBs of 8 image bytes into one byte
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    // Start with zero
    *data = 0;
    // Take the LSB of each of the 8 bytes
    for (int i = 0; i < 8; i++)
    {
        // Shift left and add the new bit
        *data = (*data << 1) | (image_buffer[i] & 1);
    }
    return e_success;
}

// Collects the LSBs of 32 image bytes into a number
Status decode_size_from_lsb(char *buffer, long *size)
{
    // Not used in your flow, but implement for completeness
    // Start with zero
    *size = 0;
    // Take the LSB of each of the 32 bytes
    for (int i = 0; i < 32; i++)
    {
        // Shift left and add the new bit
        *size = (*size << 1) | (buffer[i] & 1);
    }
    return e_success;
}

// Decodes the given number of bytes from the stego image
Status decode_data_from_image(char *data, int size, FILE *fptr_stego, FILE *fptr_secret)
{
    // Generic decoder: decode 'size' bytes
    // Buffer for 8 image bytes
    char image_buffer[8];
    // Decode the required number of bytes
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from the stego image
        fread(image_buffer, 1, 8, fptr_stego);
        // Get one byte from them
        decode_byte_from_lsb(&data[i], image_buffer);
    }
    // If fptr_secret provided, write
    if (fptr_secret != NULL)
    {
        // Write the decoded data to the output file
        fwrite(data, 1, size, fptr_secret);
    }
    return e_success;
}

// Reads 32 bytes from the stego image and decodes a number
Status decode_size_from_image(long *size, FILE *fptr_stego)
{
    // Buffer for 32 image bytes
    char image_buffer[32];
    // Read 32 bytes from the stego image
    fread(image_buffer, 1, 32, fptr_stego);
    // Start with zero
    *size = 0;
    // Take the LSB of each of the 32 bytes
    for (int i = 0; i < 32; i++)
    {
        // Shift left and add the new bit
        *size = (*size << 1) | (image_buffer[i] & 1);
    }
    return e_success;
}

// Checks that the hidden magic string matches the expected one
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    // Skip the 54 byte BMP header
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    // Holds the decoded magic string
    char decoded[3];
    // Buffer for 8 image bytes
    char image_buffer[8];
    // Decode every character of the magic string
    for (int i = 0; i < strlen(magic_string); i++)
    {
        // Read 8 bytes from the stego image
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        // Get one character from them
        decode_byte_from_lsb(&decoded[i], image_buffer);
    }
    // End the string with a null character
    decoded[strlen(magic_string)] = '\0';
    // Success only if the decoded string matches the magic string
    if (strcmp(decoded, magic_string) == 0)
        return e_success;
    else
        return e_failure;
}

// Decodes the length of the file extension
Status decode_secret_file_extn_size(long *extn_size, DecodeInfo *decInfo)
{
    // Holds the decoded size
    long size;
    // Decode the extension size
    decode_size_from_image(&size, decInfo->fptr_stego_image);
    // Return the size to the caller
    *extn_size = size;
    // Save the size in the structure
    decInfo->extn_size = size;
    return e_success;
}

// Decodes the file extension (for example .txt)
Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo)
{
    // file_extn param is unused, we decode directly into struct
    // Buffer to hold the decoded extension
    char extn[MAX_FILE_SUFFIX + 10];
    // Buffer for 8 image bytes
    char image_buffer[8];
    // Decode every character of the extension
    for (int i = 0; i < decInfo->extn_size; i++)
    {
        // Read 8 bytes from the stego image
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        // Get one character from them
        decode_byte_from_lsb(&extn[i], image_buffer);
    }
    // End the string with a null character
    extn[decInfo->extn_size] = '\0';
    // Save the extension in the structure
    strcpy(decInfo->extn_secret_file, extn);
    return e_success;
}

// Decodes the size of the secret file
Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo)
{
    // Holds the decoded size
    long size;
    // Decode the secret file size
    decode_size_from_image(&size, decInfo->fptr_stego_image);
    // Return the size to the caller
    *file_size = size;
    // Save the size in the structure
    decInfo->size_secret_file = size;
    return e_success;
}

// Decodes the secret file content and writes it to the output file
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    // Buffer for 8 image bytes
    char image_buffer[8];
    // Holds one decoded byte
    char ch;
    // Decode every byte of the secret file
    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        // Read 8 bytes from the stego image
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        // Get one byte from them
        decode_byte_from_lsb(&ch, image_buffer);
        // Write the byte to the output file
        fputc(ch, decInfo->fptr_secret);
    }
    return e_success;
}

// Runs all the decoding steps in order
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: Decoding operation selected\n");

    // Open the stego image and the output file
    if (open_decode_files(decInfo) == e_failure)
        return e_failure;

    printf("INFO: Opened %s\n", decInfo->stego_image_fname);

    // Check the magic string to confirm the image has hidden data
    if (decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
    {
        fprintf(stderr, "ERROR: Magic string mismatch\n");
        return e_failure;
    }
    printf("INFO: Magic string matched %s\n", MAGIC_STRING);

    // Decode the extension size
    long extn_size;
    decode_secret_file_extn_size(&extn_size, decInfo);
    printf("INFO: Extn size %ld\n", extn_size);

    // Decode the extension
    decode_secret_file_extn(NULL, decInfo);
    printf("INFO: Extn is %s\n", decInfo->extn_secret_file);

    // Decode the secret file size
    long file_size;
    decode_secret_file_size(&file_size, decInfo);
    printf("INFO: Secret file size %ld\n", file_size);

    // Decode the secret file data
    decode_secret_file_data(decInfo);
    printf("INFO: Decoding done, check %s\n", decInfo->secret_fname);

    // Close both files
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_secret);
    return e_success;
}
