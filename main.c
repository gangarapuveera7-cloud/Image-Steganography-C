/*
* Name        : Gangarapu VeeraShankar
* Batch ID    : 26018F
* Project     : Image Steganography (LSB Encoding and Decoding)
* File        : main.c
* Description : Main driver of the Image Steganography project written in C.
*               -e hides a secret file inside a .bmp image and -d extracts it back from the stego image.
*               It checks the arguments and calls the encoding or decoding functions.
*/
// Standard input/output functions (printf)
#include <stdio.h>
// String functions (strcmp)
#include <string.h>
// Encoding structure and function declarations
#include "encode.h"
// Decoding structure and function declarations
#include "decode.h"
// User defined types (Status, OperationType)
#include "types.h"

// Function that finds whether -e or -d is given
OperationType check_operation_type(char *);

// Program starts here
int main(int argc, char *argv[])
{
    // Check minimum args: ./a.out -e <source> <secret> [output]
    if (argc < 4)
    {
        printf("Error: Insufficient arguments\n");
        printf("Usage: %s -e <source_image> <secret_file> [output_image]\n", argv[0]);
        printf("       %s -d <stego_image> [output_file]\n", argv[0]);
        return 1;
    }

    // Find the operation type from argv[1]
    OperationType op_type = check_operation_type(argv[1]);

    // Encoding selected
    if (op_type == e_encode)
    {
        //step 1 -> check check_operation_type(argv[1]) is returning e_encode or not
        // Structure that stores all encoding information
        // yes -> EncodeInfo encInfo;
        EncodeInfo encInfo;

        // check read_and_validate_encode_args(argv, &encInfo) is returning e_success or e_failure
        if (read_and_validate_encode_args(argv, &encInfo) == e_failure)
        {
            // failure -> print error msg and stop
            printf("ERROR: read_and_validate_encode_args failed\n");
            return 1;
        }
        else
        {
            // success -> print success msg
            // Arguments are valid, so start the encoding
            printf("INFO: read_and_validate_encode_args successful\n");

            // check do_encoding(&encInfo) is returning e_success or e_failure
            if (do_encoding(&encInfo) == e_failure)
            {
                // failure -> print error msg and stop
                printf("ERROR: do_encoding failed\n");
                return 1;
            }
            else
            {
                // success -> print success msg and stop
                printf("INFO: Encoding completed successfully\n");
                return 0;
            }
        }
    }
    // Decoding selected
    else if (op_type == e_decode)
    {
        // Structure for decoding: validate arguments, run decoding and print the result
        DecodeInfo decInfo; if(read_and_validate_decode_args(argv,&decInfo)==e_failure){printf("ERROR: decode args failed\n"); return 1;} if(do_decoding(&decInfo)==e_failure){printf("ERROR: do_decoding failed\n"); return 1;} printf("INFO: Decoding completed successfully\n"); return 0;
        // You can add similar decode flow here:
        // DecodeInfo decInfo;
        // if (read_and_validate_decode_args(...) == e_failure) ...
        // if (do_decoding(&decInfo) == e_failure) ...
    }
    else
    {
        // Operation is neither -e nor -d
        printf("ERROR: Unsupported operation type. Use -e for encode and -d for decode\n");
        return 1;
    }

    return 0;
}

// Returns e_encode for -e, e_decode for -d, otherwise e_unsupported
OperationType check_operation_type(char *symbol)
{
    //step 1 -> check symbol is -e or not
    if (symbol != NULL && strcmp(symbol, "-e") == 0)
    {
        // yes -> return e_encode
        return e_encode;
    }

    // step 2 -> check symbol is -d or not
    if (symbol != NULL && strcmp(symbol, "-d") == 0)
    {
        // yes -> return e_decode
        return e_decode;
    }

    // return e_unsupported
    return e_unsupported;
}
