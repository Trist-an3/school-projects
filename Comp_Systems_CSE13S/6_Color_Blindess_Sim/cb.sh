#!/bin/bash

# Convert the files of the bmps directory.
#
# Inputs:  Any file whose name matches 'bmps/*-orig.bmp'.
# Outputs: File names found above with 'orig' changed to 'colorb'.

# How it works:
#
# The "for" line creates a list of files to process.
# Each of those filenames is presented as $x to the './colorb' line,
# which uses it an an input filename.
#
# But there's a curious part:
#
#     ${x%-orig\.bmp}
#
# This means "take $x and delete the '-orig.bmp' at the end".
# Then the '-colorb.bmp' appended to it (look below) creates the desired
# output filename.

for x in bmps/*-orig.bmp; do
  ./colorb -i $x -o ${x%-orig\.bmp}-colorb.bmp
done

