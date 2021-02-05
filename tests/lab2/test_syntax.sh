#!/bin/bash

# DO NOT MODIFY!
# If you need customized behavior, please create your own script.

CUR_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
TESTCASE="${1:-testcase}"
TESTCASE_DIR="$CUR_DIR/$TESTCASE"
OUTPUT_DIR="$CUR_DIR/syntree_$TESTCASE"
BUILD_DIR="$CUR_DIR/../../build"

# Make sure $OUTPUT_DIR exists.
mkdir -p "$OUTPUT_DIR"

# Generate a .syntax_tree file for all .cminus files in the testcase dir.
SAVEIFS=$IFS
IFS=$(echo -en "\n\b")
for testcase in "$TESTCASE_DIR"/*.cminus
do
    filename="$(basename $testcase)"
    echo "[info] Analyzing $filename"
    "$BUILD_DIR"/parser "$testcase" > "$OUTPUT_DIR/${filename%.cminus}.syntax_tree"
done

# Output a summary when requested.
if [[ ${2:-no} != "no" ]]; then
    echo "[info] Comparing..."
    if [[ ${2:-no} == "verbose" ]]; then
        diff "$OUTPUT_DIR" "${OUTPUT_DIR}_std"
    else
        diff -qr "$OUTPUT_DIR" "${OUTPUT_DIR}_std"
    fi
    if [ $? -eq 0 ]; then
        echo "[info] No difference! Congratulations!"
    fi
fi

IFS=$SAVEIFS
