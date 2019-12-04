// We use 4KB blocks.
#define BLOCK_SIZE 4096

// Our disk consists of 64 blocks.
#define BLOCKS_COUNT 64

// We use char to store bits
#define BLOCK_CHAR_COUNT (BLOCK_SIZE * 8 / CHAR_BIT)
// We simulate a disk with 512-byte sectors.
#define SECTOR_SIZE 512

#define FS_MAGIC "vsfs"
#define FS_MAGIC_LEN 4

#define IMAP_START	BLOCK_SIZE
#define DMAP_START	2*BLOCK_SIZE
#define INODE_START	3*BLOCK_SIZE
#define DATA_START	8*BLOCK_SIZE

// 5 blocks for inodes, 16 inodes each = up to 80 total inodes
#define INODE_BLOCKS_COUNT	5
#define MAX_INODES ((BLOCK_SIZE / sizeof(inode)) * INODE_BLOCKS_COUNT)


// Blocks used: (1) super; (1) imap; (1) dmap; (5) imap
// Remaining blocks are used for data.
#define DATA_BLOCKS_COUNT	56

/**
 * The root inode is typically 2 on Unix filesystems.
 * More information on special ("well-known") inodes for ext4, for example:
 * https://ext4.wiki.kernel.org/index.php/Ext4_Disk_Layout#Special_inodes
 */
#define ROOT_INODE 2

// Differs from textbook
#define DIR_MAX_ENTRIES 16
#define DIRNAME_MAX_LEN 251

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef uint32_t localPointer;