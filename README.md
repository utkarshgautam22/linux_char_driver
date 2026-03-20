# Character Device Driver - OS Assignment 1

## Overview

This is a Linux kernel character device driver that validates the kernel version and enforces ordered read/write operations within a timeout window. When you unload the module, it tells you if everything was done correctly.

## What It Does

The driver creates a character device (`/dev/char_dev`) that:
- Only loads if your kernel version matches (checked at insert time)
- Lets you read from it and write to it
- Tracks when you read and when you write
- Makes sure you read FIRST, then write
- Tracks time using a timeout value you give it
- Tells you at the end if you did everything right

## How to Build It

You need kernel headers installed first:
```bash
sudo apt-get install linux-headers-$(uname -r)
sudo apt-get install build-essential
```

Then just do:
```bash
cd /home/ug/Desktop/os_asgn
make
```

That's it. You'll get `char_dev.ko` if it works.

## How to Use It

### Getting Your Kernel Version
First, find out what kernel you're running:
```bash
uname -r
```

If it says something like `6.17.0-14-generic`, your version is `6,17` (major=6, minor=17).

### Loading the Module

The module takes two parameters:
- `version=X,Y` - your kernel version (major,minor)
- `time=N` - timeout in seconds

Example:
```bash
sudo insmod ./char_dev.ko version=6,17 time=30
```

This loads the driver with a 30 second timeout.

After loading, check the kernel messages:
```bash
dmesg | tail -10
```

You should see something like:
```
Initializing module
Major: 505 Minor: 0
Character device created
```

### Testing: Do It Right

This is the correct way to do it (read first, then write):
```bash
# Read first
cat /dev/char_dev

# Write second
echo "root" | sudo tee /dev/char_dev > /dev/null

# Check the device
cat /dev/char_dev
```

### Unload and Check Result

```bash
sudo rmmod char_dev
dmesg | tail -5
```

If you did everything right, you'll see:
```
operations performed successfully
```

## What Can Go Wrong

### 1. Version Mismatch
If you give the wrong version:
```bash
sudo insmod ./char_dev.ko version=5,10 time=30
```

You'll get:
```
Kernel version mismatch got 5,10 expected 6,17
```

The module won't load.

### 2. Wrong Order (Write First, Then Read)
```bash
sudo insmod ./char_dev.ko version=6,17 time=30
echo "root" | sudo tee /dev/char_dev > /dev/null  # Write first (WRONG!)
cat /dev/char_dev                                 # Read second
sudo rmmod char_dev
dmesg | tail -3
```

You'll see:
```
operations not performed properly
```

### 3. Timeout Exceeded
```bash
sudo insmod ./char_dev.ko version=6,17 time=2

cat /dev/char_dev                  # Read now (OK)
sleep 3                             # Wait 3 seconds (timeout is 2)
echo "root" | sudo tee /dev/char_dev > /dev/null # Write after timeout

sudo rmmod char_dev
dmesg | tail -3
```

You'll see:
```
operations not performed within time
```

### 4. Not Doing Both Operations
```bash
sudo insmod ./char_dev.ko version=6,17 time=30

# Don't read or write, just unload
sudo rmmod char_dev
dmesg | tail -3
```

You'll see:
```
operations not performed properly
```

## How It Works (Technical Details)

### Module Parameters
- `version`: Array of 2 integers (major and minor kernel version)
- `time`: Timeout in seconds

The module extracts the kernel version from `LINUX_VERSION_CODE` at compile time and compares it with what you pass in.

### File Operations
When you do operations on `/dev/char_dev`:

**Read (`cat /dev/char_dev`):**
- Records the current time (jiffies) if this is the first read
- Copies the buffer content to your terminal
- Returns the data

**Write (`echo something > /dev/char_dev`):**
- Records the current time (jiffies) if this is the first write
- Takes what you're writing and stores it in the kernel buffer
- Removes the newline at the end for cleaner display

### Timing
- When module loads, it records `start_time`
- When you read, it records `read_time`
- When you write, it records `write_time`
- When module unloads, it checks:
  - Did both happen?
  - Did read happen before write?
  - Did write happen before `start_time + timeout`?

### The Check (at rmmod)
```
if (no read or no write)
    print "not performed properly"
else if (write happened before read)
    print "not performed properly"
else if (write was too late)
    print "not performed within time"
else
    print "performed successfully"
```

## Device Info

After loading the module:
```bash
ls -l /dev/char_dev
```

You can see:
- The device number (Major: 505, Minor: 0 in example)
- Permissions
- When it was created

## Debugging Tips

### Check if module is loaded
```bash
lsmod | grep char_dev
```

### See all kernel messages
```bash
sudo dmesg -w
```
(This shows new messages as they appear - good for watching while testing)

### Search for specific messages
```bash
dmesg | grep "char_dev"
dmesg | grep "version"
dmesg | grep "Major"
```

### Clear the kernel log
```bash
sudo dmesg -c
```

## Quick Test Checklist

- [ ] Build: `make`
- [ ] Get version: `uname -r` and convert to X,Y format
- [ ] Load: `sudo insmod ./char_dev.ko version=X,Y time=30`
- [ ] Device exists: `ls -l /dev/char_dev`
- [ ] Read first: `cat /dev/char_dev`
- [ ] Write second: `echo "root" | sudo tee /dev/char_dev > /dev/null`
- [ ] Unload: `sudo rmmod char_dev`
- [ ] Check result: `dmesg | tail -5` should say "performed successfully"

## Common Commands

```bash
# Build
make clean && make

# Load with your kernel version and 30 second timeout
sudo insmod ./char_dev.ko version=$(uname -r | sed 's/\([0-9]*\)\.\([0-9]*\).*/\1,\2/') time=30

# See what's on the device
cat /dev/char_dev

# Write to the device
echo "myusername" | sudo tee /dev/char_dev > /dev/null

# Unload
sudo rmmod char_dev

# Check result
dmesg | tail -10
```

## Files Included

- `char_dev.c` - The driver code
- `Makefile` - How to build it
- `README.md` - This file
- `METHODOLOGY.md` - Detailed explanation of how it was built
- `char_dev.ko` - The compiled module (after running make)

## Author
Utkarsh Gautam

## Assignment
Operating Systems Assignment 1 - March 2026
# linux_char_driver
