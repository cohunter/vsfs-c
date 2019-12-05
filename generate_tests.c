/**
 * These functions are used to create the filesystem images used in the tests.
 * This file should be removed before creating the .tar for the assignments.
 */
#include <unistd.h>

// Save and restore state so we do not clobber it
void saveState() {
	saveDisk("temp.img");
}

void restoreState() {
	loadDisk("temp.img");
	unlink("temp.img");
}

void createTest1bin() {
	saveState();
	creat("/file1.txt");
	printf("Created /file1.txt\n");

	mkdir("/test");
	creat("/test/file2.txt");
	printf("Created directory /test and file /test/file2.txt\n");

	saveDisk("test1.bin");
	printf("Saved disk image to test1.bin\n");

	restoreState();
}

void createTest2bin() {
	saveState();
	// This is a separate image, so there is no file1.txt
	creat("/file2.txt");
	creat("/file3.txt");

	mkdir("/test");
	creat("/test/file1.txt");

	saveDisk("test2.bin");
	printf("Saved disk image to test2.bin\n");

	restoreState();
}