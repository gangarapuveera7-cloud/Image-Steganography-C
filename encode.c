/*
* Name        : Gangarapu VeeraShankar
* Batch ID    : 26018F
* Project     : Image Steganography (LSB Encoding and Decoding)
* File        : encode.c
* Description : Encoding part of the Image Steganography project written in C.
*               It hides a secret file inside a 24-bit .bmp image by storing its bits in the LSB of the image bytes.
*               Magic string, extension, file size and file data are hidden in that order after the 54 byte BMP header.
*/
// Standard input/output functions (fopen, fread, fwrite)
#include <stdio.h>
// String functions (strlen, strcpy, strrchr)
#include <string.h>
// Standard library functions
#include <stdlib.h>
// Encoding structure and function declarations
#include "encode.h"
// User defined types (Status, uint)
#include "types.h"
// Magic string definition
#include "common.h"

// Returns the number of image data bytes (width x height x 3)
uint get_image_size_for_bmp(FILE *fptr_image)
{
    // Variables to hold the image width and height
    uint width, height;
    // Width is stored at byte 18 of the BMP header
    fseek(fptr_image, 18, SEEK_SET);
    // Read the width
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);
    // Read the height (stored right after the width)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    // Every pixel has 3 bytes (RGB)
    return width * height * 3;
}

// Returns the size of a file in bytes
uint get_file_size(FILE *fptr)
{
    // Go to the end of the file
    fseek(fptr, 0, SEEK_END);
    // The current position is the file size
    uint size = ftell(fptr);
    // Go back to the start of the file
    rewind(fptr);
    return size;
}

// Checks the command line arguments and stores the file names
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // step 1 -> check argv[2] is having .bmp or not
    if (argv[2] == NULL || strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR: Source file should be .bmp\n");
        return e_failure;
    }
    // Store the source image name
    encInfo->src_image_fname = argv[2];

    // step 2 -> check argv[3] is having extn there or not
    if (argv[3] == NULL || strchr(argv[3], '.') == NULL)
    {
        printf("ERROR: Secret file should have extension\n");
        return e_failure;
    }
    // Store the secret file name
    encInfo->secret_fname = argv[3];
    // Store extension
    strcpy(encInfo->extn_secret_file, strrchr(argv[3], '.'));

    // step 3 -> check argv[4] is NULL or not
    if (argv[4] != NULL)
    {
        if (strstr(argv[4], ".bmp") == NULL)
        {
            printf("ERROR: Output file should be .bmp\n");
            return e_failure;
        }
        // Use the output name given by the user
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        // Use the default output name
        encInfo->stego_image_fname = "stego.bmp";
        printf("INFO: Output file not mentioned, defaulting to stego.bmp\n");
    }

    return e_success;
}

// Opens the source image, secret file and output image
Status open_files(EncodeInfo *encInfo)
{
    // Open the source image
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    // Open the secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    // Open the output (stego) image
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

// Checks whether the image is big enough to hide the secret file
Status check_capacity(EncodeInfo *encInfo)
{
    // Number of bytes available in the image
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    // Size of the secret file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    // Bits needed: magic string + extn size + extn + file size + file data
    uint required = 16 + 32 + (strlen(encInfo->extn_secret_file) * 8) + 32 + (encInfo->size_secret_file * 8);

    // Image must be bigger than the bits required
    if (encInfo->image_capacity > required)
        return e_success;
    else
        return e_failure;
}

// Copies the 54 byte BMP header to the output image
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // Go to the start of the source image
    rewind(fptr_src_image);
    // BMP header is 54 bytes
    char buffer[54];
    // Read the header
    fread(buffer, 54, 1, fptr_src_image);
    // Write the header unchanged to the output image
    fwrite(buffer, 54, 1, fptr_dest_image);
    return e_success;
}

// Hides one byte in the LSBs of 8 image bytes
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Handle each of the 8 bits, MSB first
    for (int i = 0; i < 8; i++)
    {
        // Extract bit
        char bit = (data >> (7 - i)) & 1;
        // Clear LSB and set
        image_buffer[i] = (image_buffer[i] & 0xFE) | bit;
    }
    return e_success;
}

// Hides a 32 bit number in the LSBs of 32 image bytes
Status encode_size_to_lsb(int size, char *image_buffer)
{
    // Handle each of the 32 bits, MSB first
    for (int i = 0; i < 32; i++)
    {
        // Take the next bit of the number
        char bit = (size >> (31 - i)) & 1;
        // Replace the LSB of the image byte with the bit
        image_buffer[i] = (image_buffer[i] & 0xFE) | bit;
    }
    return e_success;
}

// Hides the magic string in the image
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // Buffer for 8 image bytes (1 bit goes into each)
    char buffer[8];
    // Encode every character of the magic string
    for (uint i = 0; i < strlen(magic_string); i++)
    {
        // Read 8 bytes from the source image
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        // Hide the character in those 8 bytes
        encode_byte_to_lsb(magic_string[i], buffer);
        // Write the changed bytes to the output image
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

// Hides the length of the file extension in the image
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    // Buffer for 32 image bytes (a 32 bit number)
    char buffer[32];
    // Read 32 bytes from the source image
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    // Hide the extension size in those 32 bytes
    encode_size_to_lsb(size, buffer);
    // Write the changed bytes to the output image
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

// Hides the file extension (for example .txt) in the image
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    // Buffer for 8 image bytes
    char buffer[8];
    // Encode every character of the extension
    for (uint i = 0; i < strlen(file_extn); i++)
    {
        // Read 8 bytes from the source image
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        // Hide the character in those 8 bytes
        encode_byte_to_lsb(file_extn[i], buffer);
        // Write the changed bytes to the output image
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

// Hides the size of the secret file in the image
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // Buffer for 32 image bytes
    char buffer[32];
    // Read 32 bytes from the source image
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    // Hide the file size in those 32 bytes
    encode_size_to_lsb(file_size, buffer);
    // Write the changed bytes to the output image
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

// Hides the content of the secret file in the image
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // Start reading the secret file from the beginning
    rewind(encInfo->fptr_secret);
    // Buffer for 8 image bytes
    char buffer[8];
    // Holds one byte of the secret file
    char data;
    // Encode every byte of the secret file
    for (uint i = 0; i < encInfo->size_secret_file; i++)
    {
        // Read one byte from the secret file
        fread(&data, 1, 1, encInfo->fptr_secret);
        // Read 8 bytes from the source image
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        // Hide the byte in those 8 bytes
        encode_byte_to_lsb(data, buffer);
        // Write the changed bytes to the output image
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

// Copies the rest of the image unchanged to the output image
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    // Holds one byte of the image
    char ch;
    // Copy byte by byte until the end of the source image
    while (fread(&ch, 1, 1, fptr_src) == 1)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    // Both files should now be at the same position
    if (ftell(fptr_src) == ftell(fptr_dest))
        return e_success;
    return e_failure;
}

// Runs all the encoding steps in order
Status do_encoding(EncodeInfo *encInfo)
{
    // Open all the files
    if (open_files(encInfo) == e_failure)
    {
        printf("ERROR: open_files failed\n");
        return e_failure;
    }
    printf("INFO: Files opened successfully\n");

    // Check the image can hold the secret file
    if (check_capacity(encInfo) == e_failure)
    {
        printf("ERROR: check_capacity failed - Not enough capacity\n");
        return e_failure;
    }
    printf("INFO: Capacity check passed\n");

    // Copy the 54 byte BMP header
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: copy_bmp_header failed\n");
        return e_failure;
    }
    printf("INFO: BMP header copied\n");

    // Hide the magic string
    encode_magic_string(MAGIC_STRING, encInfo);
    printf("INFO: Magic string encoded\n");

    // Hide the extension size
    encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo);
    printf("INFO: Extn size encoded\n");

    // Hide the extension
    encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
    printf("INFO: Extn encoded\n");

    // Hide the secret file size
    encode_secret_file_size(encInfo->size_secret_file, encInfo);
    printf("INFO: File size encoded\n");

    // Hide the secret file data
    encode_secret_file_data(encInfo);
    printf("INFO: File data encoded\n");

    // Copy the rest of the image unchanged
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: copy_remaining_img_data failed\n");
        return e_failure;
    }
    printf("INFO: Remaining data copied\n");

    return e_success;
}
