#!/bin/bash
./vsfs_impl test1.bin > test1-out.txt
diff -w test1-gold.txt test1-out.txt
