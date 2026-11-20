#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h" // Contains user defined types
#include "common.h"

#define MAX_SECRET_BUF 1
#define MAX_IMAGE_BUF 8
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Decoded data */
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF];
    long size_secret_file;
    long extn_size;
    char image_data[MAX_IMAGE_BUF];
    int width, height;

} DecodeInfo;

/* Decoding function prototype */

/* Check operation type */
// removed duplicate

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extn size */
Status decode_secret_file_extn_size(long *extn_size, DecodeInfo *decInfo);

/* Decode secret file extention */
Status decode_secret_file_extn(const char *file_extn, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function to decode data */
Status decode_data_from_image(char *data, int size, FILE *fptr_stego, FILE *fptr_secret);

/* Decode function to decode size */
Status decode_size_from_image(long *size, FILE *fptr_stego);

/* Decode byte from lsb */
Status decode_byte_from_lsb(char *data, char *image_buffer);

/* Decode size from lsb */
Status decode_size_from_lsb(char *buffer, long *size);

#endif