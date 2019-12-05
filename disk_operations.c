#include "constants.h"
#include "bit_operations.c"
#include <stdlib.h>
#include <stdio.h>

// to be moved to header file
sector* read_sector(int);
void initializeRootDirectory();
inode* inodePtr(int);

/**
 * Chunk of memory which represents our disk.
*/
void* disk;

void initializeDisk() {
	/**
	 * Allocate memory to hold the contents of the virtual disk.
	 * Suggested reference: https://linux.die.net/man/3/malloc
	 * Optional reference: https://linux.die.net/man/3/memset
	 * NOTE: Memory allocated with malloc() is not initialized to zero.
	 */

	// Method one (calloc):
	disk = calloc(1, BLOCKS_COUNT * BLOCK_SIZE);
	// calloc(BLOCKS_COUNT, BLOCK_SIZE) would be more idiomatic,
	// but would not guarantee a single contiguous chunk of memory.

	// Method two (malloc, memset):
	//disk = malloc(BLOCKS_COUNT * BLOCK_SIZE);
	//memset(disk, 0, BLOCKS_COUNT * BLOCK_SIZE);

	if ( NULL == disk ) {
		printf("Error in initializeDisk() function.");
	} 
}

/**
 * Populate the superblock with valid data.
 */
void initializeSuperblock() {
	superBlock *super = (superBlock*) disk;
	memcpy(super->magic.magic_char_array, FS_MAGIC, FS_MAGIC_LEN);
	super->fs_root = (localPointer) (disk - (void*) inodePtr(ROOT_INODE));
}

/**
 * Get a pointer to an inode from an inode number.
 */
inode* inodePtr(int inode_number) {
	/**
	 * Each inode is 256 bytes.
	 * Each sector is 512 bytes.
	 * Each block is 4096 bytes (4k).
	 * 
	 * Each block holds 16 inodes.
	 * Each block holds 8 sectors.
	 * Each sector holds 2 inodes.
	 * 
	 * There are 16 inodes per block, in 8 sectors.
	 */
	int blk = (inode_number * sizeof(inode)) / BLOCK_SIZE;

	// We are working with integers, the remainder must be done separately:
	int remainder = (inode_number * sizeof(inode)) % BLOCK_SIZE;

	int sectorNumber = (((blk * BLOCK_SIZE) + INODE_START + remainder)) / SECTOR_SIZE;

	//printf("SECTOR:%d; inumber: %d; inblock: %d\n", sectorNumber, inode_number, blk);
	sector*	currentSector	= read_sector(sectorNumber);

	inode*	currentInode;
	/**
	 * We have read in a sector, of size 512 bytes.
	 * Our inodes are each 256 bytes.
	 * Which part of the data from the sector do we want?
	 */

	if ( inode_number % 2 == 0 ) {
		currentInode = (inode*) currentSector;
	} else {
		currentInode = (inode*) (currentSector + sizeof(inode));
	}
	return currentInode;
}

directory* dataPtr(localPointer ptr) {
	return disk + ptr;
}

/**
 * Find the first inode that is not marked as used.
 * Mark as used and return pointer.
 * If all inodes are in use, return NULL.
 */
inode* getFirstFreeInode() {
	inodeBitMap* iMap = disk + IMAP_START;
	for ( int i = ROOT_INODE; i < MAX_INODES; i++ ) {
		if ( ! getBit(&iMap->iNodeMap, i) ) {
			// Mark inode as used
			setBit(&iMap->iNodeMap, i);
			return (inode*) (disk + (INODE_START + (i * sizeof(inode))));
		}
	}
	return NULL;
}

uint32_t getFirstFreeInodeNumber() {
	inodeBitMap* iMap = disk + IMAP_START;
	for ( int i = ROOT_INODE; i < MAX_INODES; i++ ) {
		if ( ! getBit(&iMap->iNodeMap, i) ) {
			// Mark inode as used
			setBit(&iMap->iNodeMap, i);
			return i;
		}
	}
	return -1; // MAX_INT
}

/**
 * Find the first disk block that is not marked as used.
 * Mark as used and return pointer.
 * If all disk blocks are in use, return NULL.
 */
localPointer getFirstFreeDiskBlock() {
	dataBitMap* dMap = disk + DMAP_START;
	for ( int i = 0; i < BLOCKS_COUNT; i++ ) {
		if ( ! getBit(&dMap->dataMap, i) ) {
			// Mark block as used
			setBit(&dMap->dataMap, i);
			return (localPointer) (DATA_START + (i * BLOCK_SIZE));
		}
	}
	return -1;
}

/**
 * @SEE: ROOT_INODE
 */
void initializeRootInode() {
	inodeBitMap* iMap = disk + IMAP_START;

	// Mark root inode as used.
	setBit(&iMap->iNodeMap, ROOT_INODE);
	//printf("%d\n", getBit(&iMap->iNodeMap, ROOT_INODE));

	// Add content to root inode.
	inode* rootInode = inodePtr(ROOT_INODE);
	rootInode->entry_type	= 'd';
	rootInode->refcount		= 1;
	rootInode->datablock	= getFirstFreeDiskBlock();

	// Now we've initialized the root inode, next step is to create the root directory.
}

/**
 * Create the entries for the root directory.
 */
void initializeRootDirectory() {
	directory* dir = (directory*) dataPtr((inodePtr(ROOT_INODE))->datablock);
	memcpy(&(dir->entries[0]), &(directory_entry) {.inode_number = 2, .entry_name = "."}, sizeof(directory_entry));
	memcpy(&(dir->entries[1]), &(directory_entry) {.inode_number = 2, .entry_name = ".."}, sizeof(directory_entry));
}

sector* read_sector(int sector_number) {
	int offset = sector_number * SECTOR_SIZE;
	return (disk + offset);
}

void saveDisk(char* filename) {
	FILE *diskFile = fopen(filename, "wb");
	fwrite(disk, 1, BLOCK_SIZE * BLOCKS_COUNT, diskFile);
	fclose(diskFile);
}

void loadDisk(char* filename) {
	FILE *diskFile = fopen(filename, "rb");
	if ( NULL == diskFile ) {
		printf("ERROR: Cannot open file (%s) for reading.", filename);
		exit(-1);
	}
	fread(disk, 1, BLOCK_SIZE * BLOCKS_COUNT, diskFile);
	printf("Loaded fs with magic: %s\n", ((superBlock*) disk)->magic.magic_char_array);
	fclose(diskFile);
}