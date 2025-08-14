<b>## Core idea ##</b>

The logic behind this steganography method is to hide data within an image by replacing the least significant bit (LSB) of each byte in the image data. For every 1 bit of the secret data, 8 bytes of image data are used (one bit hidden in the LSB of each byte). This way, the hidden information is imperceptible to the human eye.

In my implementation, I encoded:

The secret file’s data

The file extension

The file size

All of these were embedded using this LSB technique so that the complete file can be reconstructed during decoding.
