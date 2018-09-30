#!/usr/bin/python

import zlib
import struct
import argparse

MAGIC = 0x53444f4d

def args_parse():
	parser = argparse.ArgumentParser(description='Create initramfs module')

	parser.add_argument('initramfs', type=argparse.FileType('rb'),
			    help='CPIO archive to be passed')
	parser.add_argument('output', type=argparse.FileType('wb'),
			    help='The output which to be used by OS')

	return parser.parse_args()

def main():
	args = args_parse()

	buf = args.initramfs.read()

	crc32 = zlib.crc32(buf) & 0xffffffff
	header = struct.pack('<III', MAGIC, len(buf), crc32)

	args.output.write(header + buf)

if __name__ == '__main__':
	main()

