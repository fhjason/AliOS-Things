import os
import sys
import struct
import zlib
import time

"""
#define IMAGE_MAGIC         0xAEAE
struct image_header
{
    uint16_t magic;
    uint8_t  encrypt_algo;
    uint8_t  resv[1];

    uint8_t  version[VERSION_SZ];

    uint32_t crc32;
    uint32_t size;
	//uint8_t  padding[4060];
};
"""

def pack_image(input_file, lzma_file, ota_file):
    print 'input_file:', input_file
    print 'lzma_file:', lzma_file
    f = file(input_file, 'rb')
    data = f.read()
    f.close()

    f = file(lzma_file, 'rb')
    data_lzma = f.read()
    f.close()

    magic = 0xAEAE
    encrypt_algo = 0
    rescv = 0

    crc32_lzma = zlib.crc32(data_lzma, 0) & 0xFFFFFFFF
    crc32_lzma ^= 0xFFFFFFFF
    crc32 = zlib.crc32(data, 0) & 0xFFFFFFFF
    crc32 ^= 0xFFFFFFFF
    version = str(crc32)

    size = len(data_lzma)
    print '   size:', size
    print '   version:', version
    print '   crc32_lzma: %08x' % crc32_lzma

    header = struct.pack("<HBB24sLL", magic, encrypt_algo, rescv, version, crc32_lzma, size)

    f = file(ota_file, "wb")
    f.write(header)
    f.write(data_lzma)
    f.close()

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print sys.argv[0], "input_file", "lzma_file", "ota_file"
        exit(0)

    pack_image(sys.argv[1], sys.argv[2], sys.argv[3])
