#!/bin/bash

# Character Device Driver Test Suite
# Tests all scenarios specified in OS Assignment 1

set -e

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
print_header() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}========================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_failure() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}→ $1${NC}"
}

cleanup() {
    print_info "Cleaning up..."
    sudo rmmod char_dev 2>/dev/null || true
    sleep 1
}

# Get kernel version
get_kernel_version() {
    uname -r | sed 's/\([0-9]*\)\.\([0-9]*\).*/\1,\2/'
}

KERNEL_VERSION=$(get_kernel_version)
print_info "Detected kernel version: $KERNEL_VERSION"

# Build module
print_header "Building Module"
if make clean > /dev/null 2>&1 && make > /dev/null 2>&1; then
    print_success "Module built successfully"
else
    print_failure "Module build failed"
    exit 1
fi

# Test 1: Version Mismatch
print_header "Test 1: Version Mismatch (Expected: Failure)"
cleanup
print_info "Attempting insmod with wrong version: kernel_version=5,10"
if sudo insmod ./char_dev.ko kernel_version=5,10 time=30 2>&1 | grep -q "File exists" || ! lsmod | grep -q char_dev; then
    print_success "Module correctly rejected wrong version"
    dmesg | grep "mismatch" | tail -1 || true
else
    print_failure "Module should have been rejected"
fi
cleanup

# Test 2: Successful execution (Read → Write → Success)
print_header "Test 2: Successful Execution (Read → Write)"
print_info "Inserting module with kernel_version=$KERNEL_VERSION and time=30"
sudo insmod ./char_dev.ko kernel_version=$KERNEL_VERSION time=30

print_info "Waiting for device creation..."
sleep 1

if [ ! -e /dev/char_dev ]; then
    print_failure "Device /dev/char_dev not created"
    cleanup
    exit 1
fi

print_success "Device /dev/char_dev created"

print_info "Checking device info:"
ls -l /dev/char_dev

print_info "Reading device (first operation)..."
INITIAL_CONTENT=$(cat /dev/char_dev)
print_success "Read successful. Content: '$INITIAL_CONTENT'"

print_info "Writing username to device (second operation)..."
echo "root" | sudo tee /dev/char_dev > /dev/null
print_success "Write successful"

print_info "Removing module and checking exit message..."
sudo rmmod char_dev

sleep 1
EXIT_MSG=$(dmesg | grep "Successfully completed" | tail -1)
if [ -n "$EXIT_MSG" ]; then
    print_success "Success message found:"
    echo -e "${GREEN}  $EXIT_MSG${NC}"
else
    print_failure "Success message not found in dmesg"
fi

# Test 3: Wrong Order (Write → Read → Failure)
print_header "Test 3: Wrong Operation Order (Write → Read)"
cleanup
print_info "Inserting module with kernel_version=$KERNEL_VERSION and time=30"
sudo insmod ./char_dev.ko kernel_version=$KERNEL_VERSION time=30

sleep 1

print_info "Writing username first (wrong order)..."
echo "testuser" | sudo tee /dev/char_dev > /dev/null
print_success "Write completed (intentionally wrong order)"

print_info "Reading device..."
cat /dev/char_dev > /dev/null
print_success "Read completed after write"

print_info "Removing module and checking failure message..."
sudo rmmod char_dev

sleep 1
FAIL_MSG=$(dmesg | grep "Read must occur before" | tail -1)
if [ -n "$FAIL_MSG" ]; then
    print_success "Correct failure message found:"
    echo -e "${GREEN}  $FAIL_MSG${NC}"
else
    print_failure "Expected failure message not found"
fi

# Test 4: Missing Operations (No read or write)
print_header "Test 4: Missing Operations"
cleanup
print_info "Inserting module with kernel_version=$KERNEL_VERSION and time=30"
sudo insmod ./char_dev.ko kernel_version=$KERNEL_VERSION time=30

sleep 1

print_info "Not performing any read or write operations..."
print_info "Removing module immediately..."

sudo rmmod char_dev

sleep 1
MISSING_MSG=$(dmesg | grep "Both read and write" | tail -1)
if [ -n "$MISSING_MSG" ]; then
    print_success "Correct failure message found:"
    echo -e "${GREEN}  $MISSING_MSG${NC}"
else
    print_failure "Expected failure message not found"
fi

# Test 5: Timeout Exceeded
print_header "Test 5: Operations Beyond Timeout"
cleanup
print_info "Inserting module with kernel_version=$KERNEL_VERSION and time=2 (2 seconds)"
sudo insmod ./char_dev.ko kernel_version=$KERNEL_VERSION time=2

sleep 1

print_info "Reading device immediately..."
cat /dev/char_dev > /dev/null
print_success "Read completed within timeout"

print_info "Waiting 3 seconds (exceeding timeout)..."
sleep 3

print_info "Writing after timeout exceeded..."
echo "root" | sudo tee /dev/char_dev > /dev/null
print_success "Write completed after timeout"

print_info "Removing module and checking timeout message..."
sudo rmmod char_dev

sleep 1
TIMEOUT_MSG=$(dmesg | grep "not performed within time" | tail -1)
if [ -n "$TIMEOUT_MSG" ]; then
    print_success "Correct failure message found:"
    echo -e "${GREEN}  $TIMEOUT_MSG${NC}"
else
    print_failure "Expected timeout message not found"
fi

# Final Summary
print_header "Test Suite Complete"
print_success "All major test scenarios executed"
print_info "Review dmesg output for detailed logs:"
echo "dmesg | tail -20"

cleanup
