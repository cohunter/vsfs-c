vsfs_impl: *.c *.h
	gcc -o vsfs_impl vsfs_impl.c

tests: vsfs_impl
	for f in ./test*.sh; do ./$$f; echo $$?; done
