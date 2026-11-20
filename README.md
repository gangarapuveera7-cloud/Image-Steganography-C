# Image Steganography Using C

## Overview
This project implements Image Steganography using the Least Significant Bit (LSB) technique in C. It allows users to hide secret files inside BMP images and retrieve them through a decoding process. The project demonstrates concepts such as file handling, bit manipulation, image processing, and data security.

## Features
- Encode secret files into BMP images
- Decode hidden data from stego images
- Magic string verification for data integrity
- Secret file extension extraction
- Secret file size decoding
- Supports text file hiding and recovery
- Efficient LSB-based data embedding

## Technologies Used
- C Programming
- File Handling
- Bit Manipulation
- BMP Image Processing
- Data Encoding and Decoding

## Project Structure

Image-Steganography-C/
├── encode.c
├── decode.c
├── encode.h
├── decode.h
├── common.h
├── types.h
├── secret.txt
├── decoded.txt
├── README.md
└── screenshots/

## Compilation

gcc *.c -o stego

## Usage

### Encoding

./stego -e input.bmp secret.txt stego.bmp

### Decoding

./stego -d stego.bmp decoded.txt

## Sample Output

Secret Message:
Hello World

Decoded Message:
Hello World

## Learning Outcomes
- Understanding of Steganography concepts
- Advanced File Handling in C
- Bitwise Operations
- BMP Image Structure
- Data Security Techniques

## Author

VeeraShankar Gangarapu
