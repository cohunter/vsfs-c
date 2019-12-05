#!/bin/bash
./vsfs_impl test2.bin > test2-out.txt
diff -w test2-gold.txt test2-out.txt
