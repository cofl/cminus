#!/bin/bash

# Test script by cofl <cllacour@mtu.edu>

SCRIPT_DIR=$(realpath $(dirname ${BASH_SOURCE[0]}))
PROJECT_DIR=$(realpath "$SCRIPT_DIR/..")
TEST_SOURCE_DIR="$PROJECT_DIR/input"

CMC="$PROJECT_DIR/cmc"

# Add files by base name to the below list to skip them.
# The format is "<name> [reason]"
# Reason is optional, but will be printed.
SKIP="
22.guess.cm         Random input is cannot be tested.
23.asm.cm           Non-standard test.
13.hack_array.cm    Test assumes undefined behavior.
19.hack.cm          Hacky stuff, and I don't want to bother reading input.
"

ADDITIONAL="
17.external.cm      ./foo.c
18.external.cm      ./foo.c
19.hack.cm          ./foo.c
"

FLAGS="
19.hack.cm          -fno-stack-protector
"

##
# Internals below! Don't mess with this unless you know
# what you're doing!
##

_CPP_HEAD='
#include<cstdio>
void write(int a)
{
    printf("%d\n", a);
}

void write(const char* a)
{
    puts(a);
}

void read(int& a)
{
    scanf("%d", &a);
}

extern "C" {
'

_CPP_FOOT='
}
'

if [ $# -gt 0 ]; then
    TEST_FILES=$(realpath $@)
else
    TEST_FILES=$TEST_SOURCE_DIR
fi
TEST_FILES=$(ls -v $({ for item in $TEST_FILES; do echo $item; done } | xargs -I{} find {} -name "*.cm" -type f) | xargs -I{} realpath {})
FILE_COUNT=$(echo "$TEST_FILES" | wc -w)

# Build CMC
pushd $PROJECT_DIR &>/dev/null
if ! make -s cmc; then
    echo
    echo "Failed to build cmc, cannot continue."
    exit 1
fi
popd &>/dev/null

echo
# Counters
SKIPPED=0
ERRORED=0
FAILED=0
PASSED=0
TEST_COUNT=0

# Test loop
ACTUAL_EXE="$SCRIPT_DIR/actual"
EXPECTED_EXE="$SCRIPT_DIR/expected"
for file in $TEST_FILES; do
    rm -f $ACTUAL_EXE $EXPECTED_EXE
    file_basename="$(basename $file)"
    file_dirname="$(dirname $file)"
    let TEST_COUNT+=1

    {   # Print out errors and skip if listed above.
        w=$(echo "$SKIP" | grep $file_basename)
        if [ ! -z "$w" ]; then
            printf "[SKIP] $file_basename ($file_dirname)\n"
            w=$(echo $w | sed -re "s/^\S+\s*//")
            if [ ! -z "$w" ]; then printf "\t%s\n" "$w"; fi
            let SKIPPED+=1
            continue
        fi
    }

    w=$(echo "$ADDITIONAL" | grep $file_basename)
    add_srcs=""
    if [ ! -z "$w" ]; then
        w=$(echo $w | sed -re "s/^\S+\s*//")
        for j in $w; do
            add_srcs+=" $(realpath "$file_dirname/$j")"
        done
    fi

    flags=$(echo "$FLAGS" | grep $file_basename)
    if [ ! -z "$flags" ]; then
        flags=$(echo $flags | sed -re "s/^\S+\s*//")
    fi

    {   # Compile the Expected for this test
        w=$(echo "$_CPP_HEAD" "$(cat $add_srcs $file)" "$_CPP_FOOT" | gcc $flags -x c++ - -g -o $EXPECTED_EXE 2>&1)
        if [ $? -ne 0 ]; then
            printf "[ERR] $file_basename ($file_dirname)\n"
            printf "\tFailed to compile Expected executable for comparison.\n"
            if [ ! -z "$w" ]; then printf "\tOutput was:\n%s\n\n" "$w"; fi
            let ERRORED+=1
            continue
        fi
    }

    {   # Compile the Actual for this test
        file_assembly=$(echo "$file" | sed -re "s/\.cm$/\.s/")
        w=$($CMC $file 2>&1)
        if [ $? -ne 0 ]; then
            printf "[ERR] $file_basename ($file_dirname)\n"
            printf "\tFailed to compile Cminus code to assembly.\n"
            if [ ! -z "$w" ]; then printf "\tOutput was:\n%s\n\n" "$w"; fi
            let ERRORED+=1
            continue
        fi

        if [ ! -f $file_assembly ]; then
            printf "[ERR] $file_basename ($file_dirname)\n"
            printf "\tAssembly not found at expected location: %s\n" "$file_assembly"
            let ERRORED+=1
            continue
        fi

        w=$(gcc $flags -g $file_assembly $add_srcs -o $ACTUAL_EXE 2>&1)
        if [ $? -ne 0 ]; then
            printf "[ERR] $file_basename ($file_dirname)\n"
            printf "\tFailed to compile Actual executable from assembly for comparison.\n"
            if [ ! -z "$w" ]; then printf "\tOutput was:\n%s\n\n" "$w"; fi
            let ERRORED+=1
            continue
        fi

        # Delete the assembly when we're done.
        rm "$file_assembly"
    }

    input_files=$(find "$SCRIPT_DIR/$file_basename" -type f 2>/dev/null)
    if [ ! -z "$input_files" ]; then
        # Test with input files
        let TEST_COUNT-=1
        for in_file in $input_files; do
            let TEST_COUNT+=1
            relative_in_file=$(echo "$in_file" | cut -c $(expr ${#SCRIPT_DIR} + 2)-)
            expected_text=$(cat "$in_file" | $EXPECTED_EXE)
            expected_return=$?
            actual_text=$(cat "$in_file" | $ACTUAL_EXE)
            actual_return=$?
            if [ $expected_return -ne $actual_return ]; then
                printf "[FAIL] $file_basename [$relative_in_file] ($file_dirname)\n"
                printf "\tReturn codes did not match: expected \"%d\", saw \"%d\" \n" "$expected_return" "$actual_return"
                let FAILED+=1
                continue
            fi
            test_diff=$(diff -u <(echo "$expected_text") <(echo "$actual_text"))
            if [ -z "$test_diff" ]; then
                printf "[PASS] $file_basename [$relative_in_file] ($file_dirname)\n"
                let PASSED+=1
            else
                printf "[FAIL] $file_basename [$relative_in_file] ($file_dirname)\n"
                printf "%s\n" "$(echo "$test_diff" | sed -re "s/^/\t/g")"
                let FAILED+=1
            fi
        done
    else
        # No input files, only do one test
        expected_text=$($EXPECTED_EXE)
        expected_return=$?
        actual_text=$($ACTUAL_EXE)
        actual_return=$?
        if [ $expected_return -ne $actual_return ]; then
            printf "[FAIL] $file_basename ($file_dirname)\n"
            printf "\tReturn codes did not match: expected \"%d\", saw \"%d\" \n" "$expected_return" "$actual_return"
            let FAILED+=1
            continue
        fi
        test_diff=$(diff -u <(echo "$expected_text") <(echo "$actual_text"))
        if [ -z "$test_diff" ]; then
            printf "[PASS] $file_basename ($file_dirname)\n"
            let PASSED+=1
        else
            printf "[FAIL] $file_basename ($file_dirname)\n"
            printf "%s\n" "$(echo "$test_diff" | sed -re "s/^/\t/g")"
            let FAILED+=1
        fi
    fi
done
# Cleanup
rm -f $ACTUAL_EXE $EXPECTED_EXE

# Report
echo
echo "$PASSED/$TEST_COUNT tests passed, $SKIPPED skipped, $FAILED failures, $ERRORED errors"
