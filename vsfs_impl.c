#include <stdio.h>

// malloc()
#include <stdlib.h>

// CHAR_BIT
#include <limits.h>

// memcpy(), strcpy()
#include <string.h>

// uint32_t
#include <stdint.h>

// bool
#include <stdbool.h>

// Defines things like BLOCK_SIZE, etc.
#include "constants.h"

// Defines structures for filesystem components (blocks, inodes, etc).
#include "structures.h"

// Our virtual disk functions
#include "disk_operations.c"

// Functions for working with paths
#include "path_utils.c"

/**
 * If cond evaluates to false, declares an array of size -1.
 * This causes a compile-time error because arrays cannot have negative size.
 * In C uses of the ternary operator are expessions, not statements.
 */
#define C99_STATIC_ASSERT(cond, name) char name[cond ? 1 : -1];

bool FLAG_PATH_INCOMPLETE, FLAG_PATH_ENDNAME; // global

bool dir_has_dir(directory* dir, char* dirname) {
	for ( int i = 0; i < DIR_MAX_ENTRIES; i++ ) {
		if ( strcmp(dirname, dir->entries[i].entry_name) == 0 && inodePtr(dir->entries[i].inode_number)->entry_type == 'd' )
			return true;
	}
	return false;
}

bool isValidEntry(directory_entry* dEntry) {
	if ( dEntry == NULL ) {
		return false;
	}
	if ( '\0' == dEntry->entry_name[0] ) {
		return false;
	}
	return true;
}

inode* getDirInode(directory* dir, char* dirname) {
	for ( int i = 0; i < DIR_MAX_ENTRIES; i++ ) {
		if ( strcmp(dirname, dir->entries[i].entry_name) == 0 ) {
			return inodePtr(dir->entries[i].inode_number);
		}
	}
	return NULL;
}

directory_entry* getFirstFreeEntry(directory* dir) {
	for ( int i = 0; i < DIR_MAX_ENTRIES; i++ ) {
		if ( '\0' == dir->entries[i].entry_name[0] )
			return &(dir->entries[i]);
	}
	return NULL;
}

void initializeDirInode(directory* parent, uint32_t newDirInodeNumber) {
	inodeBitMap* iMap = disk + IMAP_START;

	// Mark inode as used.
	setBit(&iMap->iNodeMap, newDirInodeNumber);
	setBit(&iMap->iNodeMap, newDirInodeNumber);

	// Add content to inode.
	inode* currentInode = inodePtr(newDirInodeNumber);
	currentInode->entry_type	= 'd';
	currentInode->refcount		= 1;
	currentInode->datablock	= getFirstFreeDiskBlock();

	// Now we've initialized the inode, next step is to create the directory.
}

/**
 * Given a path, perform traversal by entering each directory.
 * Returns a pointer to the inode of the deepest directory.
 * If given a path with non-existing path components,
 * return the highest valid directory starting from root.
 * 
 * / -> /
 * /doesnotexist -> / [FLAG_PATH_ENDNAME = true]
 * /filename.txt -> / [FLAG_PATH_ENDNAME = true]
 * /temp -> / [FLAG_PATH_ENDNAME = true]
 * /temp/doesnotexist -> / [FLAG_PATH_INCOMPLETE = true]
 * [mkdir("/temp")]
 * /temp -> /temp
 * /temp/doesnotexist -> /temp [FLAG_PATH_ENDNAME = true]
 * /doesnotexist/alsodoesnotexist -> / [FLAG_PATH_INCOMPLETE = true]
 */
inode* descendPath(char* path) {
	// Start at the fs root
	inode* currentInode = inodePtr(ROOT_INODE);
	directory* currentDirectory = (directory*) dataPtr(currentInode->datablock);
	char* endToken = finalToken(path);

	FLAG_PATH_INCOMPLETE = false;
	FLAG_PATH_ENDNAME = false;

	// strtok can't work with const arguments, make a copy on the stack.
	char *pos = NULL;
	char temp[DIRNAME_MAX_LEN + 1];
	strncpy(temp, path, DIRNAME_MAX_LEN);

	char* tok = strtok_r(temp, "/", &pos);

	inode* prevInode = NULL;

	// Descend into the requested directory
	while ( tok != NULL ) {
		if ( dir_has_dir(currentDirectory, tok) ) {
			currentInode = getDirInode(currentDirectory, tok);
			if ( currentInode == NULL ) {
				printf("ERROR: Directory inode requested but not found. (Path: %s, Dir: %s)\n", path, tok);
				exit(-1);
			} else if ( currentInode->entry_type != 'd' ) {
				printf("ERROR: Attempting to enter a non-directory. (Path: %s, Dir: %s)", path, tok);
				exit(-1);
			}
			currentDirectory = (directory*) dataPtr(currentInode->datablock);
		} else {
			// Is this an entry name (file to be accessed or directory to be created)?
			if ( strcmp(endToken, tok) == 0 ) {
				FLAG_PATH_ENDNAME = true;
				//printf("FLAG_PATH_ENDNAME\n");
				return currentInode;
			} else {
				// Requested a directory, but it does not exist.
				FLAG_PATH_INCOMPLETE = true;
				//printf("FLAG_PATH_INCOMPLETE\n");
				return currentInode;
			}
		}
		tok = strtok_r(NULL, "/", &pos);
	}

	return currentInode;
}

/**
 * Read a directory.
 */
void ls(char* path) {
	inode* currentInode = descendPath(path);
	directory* currentDirectory = (directory*) dataPtr(currentInode->datablock);

	if ( FLAG_PATH_INCOMPLETE ) {
		printf("ERROR: No such directory (path: %s)\n", path);
		exit(-1);
	}

	// Listing a single file (e.g. "/test.txt")
	if ( FLAG_PATH_ENDNAME ) {
		bool found_entry = false;
		char* wantName = finalToken(path);
		printf("Listing single file. Want: %s\n", wantName);

		for ( int i = 0; i < DIR_MAX_ENTRIES; i++ ) {
			if ( isValidEntry(&(currentDirectory->entries[i])) ) {
				if ( strcmp(wantName, currentDirectory->entries[i].entry_name) == 0 ) {
					printf("%s\n", currentDirectory->entries[i].entry_name);
					found_entry = true;
				}
			}
		}

		if ( !found_entry ) {
			printf("ERROR: Tried to list single file (%s), but not contained in parent directory.\n", wantName);
			exit(-1);
		}
	} else {
		// Listing the contents of the directory
		for ( int i = 0; i < DIR_MAX_ENTRIES; i++ ) {
			if ( isValidEntry(&(currentDirectory->entries[i])) ) {
				printf("%s\n", currentDirectory->entries[i].entry_name);
			}
		}
	}
}

/**
 * Print out a directory listing with inode numbers.
 */
void ls_i(char* path) {
	inode* currentInode = descendPath(path);
	directory* currentDirectory = (directory*) dataPtr(currentInode->datablock);

	// YOUR CODE HERE

	// Minimal Gold Code:
	for ( int i = 0; i < DIR_MAX_ENTRIES; i++ ) {
		if ( isValidEntry(&(currentDirectory->entries[i])) )
			printf("%d %s\n", currentDirectory->entries[i].inode_number, currentDirectory->entries[i].entry_name);
	}
}

void mkdir(char* path) {
	inode* currentInode = descendPath(path);
	directory* currentDirectory = (directory*) dataPtr(currentInode->datablock);

	// The directory should not exist, so the path incomplete flag must be set.
	if ( FLAG_PATH_INCOMPLETE ) {
		printf("ERROR: Attempting to create a directory with no parent.\n");
		exit(-1);
	}

	if ( !FLAG_PATH_ENDNAME ) {
		printf("ERROR: Attempting to create a directory that already exists.\n");
		exit(-1);
	}

	char* name = finalToken(path);
	int newInodeNumber = -1, parentInodeNumber = currentDirectory->entries[0].inode_number;
	// Add the new directory to the parent directory.
	for ( int i = 0; i < DIR_MAX_ENTRIES; i++ ) {
		if ( '\0' == currentDirectory->entries[i].entry_name[0] ) {
			strncpy(currentDirectory->entries[i].entry_name, name, DIRNAME_MAX_LEN);
			newInodeNumber = getFirstFreeInodeNumber();
			currentInode = inodePtr(newInodeNumber);
			currentDirectory->entries[i].inode_number = newInodeNumber;
			break;
		}
	}
	if ( -1 == newInodeNumber ) {
		printf("ERROR: Directory is full.");
		return;
	}

	// Populate the inode for the new directory.
	currentInode->entry_type = 'd';
	currentInode->refcount = 1;
	currentInode->datablock = getFirstFreeDiskBlock();

	// Next, populate the data block for the new directory.
	currentDirectory = (directory*) dataPtr(currentInode->datablock);
	memcpy(&(currentDirectory->entries[0]), &(directory_entry) {.inode_number = newInodeNumber, .entry_name = "."}, sizeof(directory_entry));
	memcpy(&(currentDirectory->entries[1]), &(directory_entry) {.inode_number = parentInodeNumber, .entry_name = ".."}, sizeof(directory_entry));
}

void creat(char* path) {
	inode* currentInode = descendPath(path);
	directory* currentDirectory = (directory*) dataPtr(currentInode->datablock);

	char* filename = finalToken(path);

	if ( !FLAG_PATH_ENDNAME || (NULL == filename) ) {
		printf("ERROR: Cannot creat() with no filename.\n");
		exit(-1);
	}

	int newInodeNumber = -1;
	for ( int i = 0; i < DIR_MAX_ENTRIES; i++ ) {
		if ( '\0' == currentDirectory->entries[i].entry_name[0] ) {
			strncpy(currentDirectory->entries[i].entry_name, filename, DIRNAME_MAX_LEN);
			newInodeNumber = getFirstFreeInodeNumber();
			currentDirectory->entries[i].inode_number = newInodeNumber;
			currentInode = inodePtr(newInodeNumber);
			break;
		}
	}
	if ( -1 == newInodeNumber ) {
		printf("ERROR: No new inode (path: %s, filename: %s)", path, filename);
		exit(-1);
	}

	currentInode->entry_type = 'f';
	currentInode->refcount = 1;
	currentInode->datablock = getFirstFreeDiskBlock();
}

int main() {
	C99_STATIC_ASSERT(CHAR_BIT == 8, assert_char_Equals8Bits);

	// Allocate memory for the disk
	initializeDisk();

	// Superblock is disk block [0]
	initializeSuperblock();

	// Populate the root inode
	initializeRootInode();

	// Populate the root directory
	initializeRootDirectory();

	//loadDisk("disk.bin");

	mkdir("/test");
	mkdir("/test2");
	mkdir("/test2/dir3");
	creat("/test2/dir3/file1");

	printf("Output of ls function:\n");
	ls("/");
	ls("/test2");
	ls("/test2/dir3");
	
	printf("Output of ls -i function:\n");
	ls_i("/");
	ls_i("/test2");
	ls_i("/test2/dir3");
	//saveDisk("disk.bin");

	printf("exit success\n");
}