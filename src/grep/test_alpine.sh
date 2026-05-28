#!/bin/bash

echo "=== Full Alpine Linux Test ==="

docker run --rm -v $(pwd):/test -w /test alpine:3.20 sh -c '
    echo "Installing dependencies..."
    apk add --no-cache gcc musl-dev make valgrind > /dev/null 2>&1
    
    echo ""
    echo "=== Testing s21_cat ==="
    cd /test/src/cat 2>/dev/null || cd /test
    make clean 2>/dev/null
    if make 2>&1; then
        echo "✓ cat build: OK"
        echo "test" > test.txt
        if ./s21_cat test.txt > /dev/null 2>&1; then
            echo "✓ cat run: OK"
        fi
        rm test.txt
    else
        echo "✗ cat build: FAIL"
    fi
    
    echo ""
    echo "=== Testing s21_grep ==="
    cd /test/src/grep 2>/dev/null || cd /test
    make clean 2>/dev/null
    if make 2>&1; then
        echo "✓ grep build: OK"
        echo "test" > test.txt
        if ./s21_grep "test" test.txt > /dev/null 2>&1; then
            echo "✓ grep run: OK"
        fi
        rm test.txt
    else
        echo "✗ grep build: FAIL"
    fi
'