#include "constants.h"
#include <stdint.h>
#ifndef VSFS_STRUCTURE_H
#define VSFS_STRUCTURE_H
/**
 * IMPORTANT: In C, structures are generally padded.
 * Reference: http://www.catb.org/esr/structure-packing/
 * The attribute: __attribute__((__packed__))
 * is used to make the compiler (tested Clang 3+, gcc 4.8+)
 * generate code without padding.
 */

/**
 * The superblock provides a magic number ("vsfs") for this example,
 * as well as providing filesystem information such as the offset
 * of the inode table; the number of inodes and data blocks; and so on.
 */
typedef struct __attribute__((__packed__)) {
	// The unique identifier for the particular filesystem.
	union magic {
		uint32_t magic;
		char magic_char_array[FS_MAGIC_LEN];
	} magic;
	
	localPointer fs_root;
	char reserved[BLOCK_SIZE - sizeof(localPointer) - sizeof(uint32_t)];
} superBlock;

/**
 * A simple bitmap.
 * Uses char, which on all modern systems is guaranteed to be at least 8 bits.
 */
typedef struct __attribute__((__packed__)) {
	// Do not assume char will always be 8 bits.
	// For example, we would want only 2048 char if each were 16 bits.
	unsigned char bits[BLOCK_CHAR_COUNT];
} bitMap;

typedef struct __attribute__((__packed__)) {
	bitMap iNodeMap;
} inodeBitMap;

typedef struct __attribute__((__packed__)) {
	bitMap dataMap;
} dataBitMap;

/**
 * iNodes: Which data blocks hold each file?
 * Metadata: Size, owner, rights, access/modification times, etc.
 * We use 256 bytes for each inode.
 * VSFS inodes have three fields.
 * @Field: type [f,d]
 * @Field: datablock [-1, address]
 * @Field: refcount [1, n]
 */
typedef struct __attribute__((__packed__)) {
	// 'd' for directory; 'f' for file
	char entry_type;
	uint8_t refcount;
	localPointer datablock;
	
	// Add padding to be 256 bytes.
	char padding[256 - sizeof(char) - sizeof(int8_t) - sizeof(localPointer)];
} inode;

/**
 * VSFS directories are simply pairs of <name,inode>
 * This implementation uses a fixed 256 bytes per entry.
 */
typedef struct __attribute__((__packed__)) {
	char entry_name[DIRNAME_MAX_LEN + 1];
	uint32_t inode_number;
} directory_entry;

/**
 * We use 256 bytes for each directory entry.
 * Each directory has up to 16 entries.
 * TODO: Differs from textbook.
 */
typedef struct __attribute__((__packed__)) {
	directory_entry entries[BLOCK_SIZE / sizeof(directory_entry)];
} directory;

// disk structures
typedef struct __attribute__((__packed__)) {
	char data[512];
} sector;
#endif