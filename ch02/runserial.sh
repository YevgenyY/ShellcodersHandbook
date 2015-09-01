#!/bin/bash

# 0x0040074a - is the address of do_valid_stuff()
printf "AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDDDDDDDDDD\x4f\x07\x40\x00" | ./serial
