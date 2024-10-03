#!/bin/bash

# ------------------------------------------------
# Step 1:  Ensure that ./huff-ref can be executed.
# ------------------------------------------------

#HUFFPATH=../../resources/asgn8/
HUFFPATH=./

HUFFREF=${HUFFPATH}huff-ref

if [ ! -f $HUFFREF ];
then
  echo "Copy huff-ref into the current directory or adjust HUFFPATH."
  exit 1
fi

# --------------------------------
# Step 2:  Run the four Unit Tests
# --------------------------------

echo Test bitwriter.c:
./bwtest
if [ $? -ne 0 ];
then
  echo "./bwtest returned error code" $?
  exit 1
fi
echo

echo Test bitreader.c
./brtest
if [ $? -ne 0 ];
then
  echo "./brtest returned error code" $?
  exit 1
fi
echo

echo Test node.c:
./nodetest
if [ $? -ne 0 ];
then
  echo "./nodetest returned error code" $?
  exit 1
fi
echo

echo Test pq.c:
./pqtest
if [ $? -ne 0 ];
then
  echo "./pqtest returned error code" $?
  exit 1
fi
echo

# -------------------------
# Step 3:  Run System Tests
# -------------------------

# Inputs:               Any file whose name matches 'files/*.txt'.
#
# Intermediate Files:   huff compresses the .txt files mentioned above
#                       into .huff files.
#
#                       huff-ref compresses the .txt files mentioned above
#                       into .huff-ref files.  These are the expect data
#                       that will be compared to .huff files.
#
# Outputs:              dehuff decompresses the .huff-ref files mentioned
#                       above into .dehuff files.  These are compared
#                       to the original .txt files.

# How it works:
#
# The "for" line creates a list of files to process.
# Each of those filenames is presented as $x to the './huff' command,
# which uses it an an input filename.
#
# But there's a curious part:
#
#     ${x%txt}huff
#
# This means "take $x and delete the 'txt' at the end".
# Then the 'huff' appended to it (look below) creates the desired
# output filename.
#
# This construction is repeated for the ./dehuff command, which reads a
# .huff-ref file and makes a .dehuff file.

for x in files/*.txt; do
  echo $x:

  # Test huff, comparing its output to that of huff-ref.
  # IN     is like files/test.txt
  # OUT    is like files/test.huff
  # EXPECT is like files/test.huff-ref
  IN=$x
  OUT=${x%txt}huff
  EXPECT=${x%txt}huff-ref

  ./huff -i $IN -o $OUT
  if [ $? -ne 0 ];
  then
    echo "./huff returned error code" $?
    exit 1
  fi

  $HUFFREF -i $IN -o $EXPECT
  if [ $? -ne 0 ];
  then
    echo "$HUFFREF returned error code" $?
    exit 1
  fi

  xxd $OUT $OUT.xxd
  xxd $EXPECT $EXPECT.xxd
  diff $EXPECT.xxd $OUT.xxd > $OUT.xxd.diff
  if [ $? -ne 0 ];
  then
    echo "Difference:"
    echo "< $EXPECT.xxd"
    echo "> $OUT.xxd"
    head -4 $OUT.xxd.diff
    exit 1
  fi

  # Test dehuff, comparing its output to the original .txt file.
  # IN     is like files/test.huff-ref
  # OUT    is like files/test.dehuff
  # EXPECT is like files/test.txt
  IN=${x%txt}huff-ref
  OUT=${x%txt}dehuff
  EXPECT=$x

  ./dehuff -i $IN -o $OUT
  if [ $? -ne 0 ];
  then
    echo "./dehuff returned error code" $?
    exit 1
  fi

  xxd $OUT $OUT.xxd
  xxd $EXPECT $EXPECT.xxd
  diff $EXPECT.xxd $OUT.xxd > $OUT.xxd.diff
  if [ $? -ne 0 ];
  then
    echo "Difference:"
    echo "< $EXPECT.xxd"
    echo "> $OUT.xxd"
    head -4 $OUT.xxd.diff
    exit 1
  fi

done

echo "The script runtests.sh was executed successfully."
