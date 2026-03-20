# Methodology - Character Device Driver

## Problem Statement

Create a Linux kernel character device driver that:
- Takes kernel version and timeout as parameters
- Validates that the kernel version matches what it was compiled for
- Creates a character device that can be read from and written to
- Tracks the order and timing of read/write operations
- Reports success or failure when the module is unloaded

## What I Built

### The Core Idea
The driver is basically a simple character device that acts like a gatekeeper. It opens only if your kernel version is correct, and it makes sure you follow the rules:
1. Read first, then write
2. Do both operations before the timeout expires

If you break the rules, it tells you when you unload it.

## How It Works

### 1. Loading the Module

When you run `sudo insmod ./char_dev.ko version=6,17 time=30`:

- The driver checks if version 6,17 matches your kernel
- It extracts the kernel version from `LINUX_VERSION_CODE` macro
- Major version is bits 16-23, minor is bits 8-15
- If they don't match, the module refuses to load
- If they match, it allocates a device number and creates `/dev/char_dev`
- It records the load time (called `start_time`)
- It calculates the timeout window: `start_time + (time * 1000 milliseconds)`

### 2. What Happens When You Use the Device

**First time you read from it:**
- The `dev_read()` function records the current kernel time (`read_time = jiffies`)
- It copies what's in the kernel buffer to your screen
- The buffer starts with "Utkarsh Gautam" by default

**First time you write to it:**
- The `dev_write()` function records the current kernel time (`write_time = jiffies`)
- It takes what you're writing and stores it in the kernel buffer
- It removes the newline at the end to keep things clean
- Prints a message saying what was written

**Later reads/writes:**
- They don't change the timestamps
- Only the FIRST read and FIRST write are tracked

### 3. Unloading the Module

When you run `sudo rmmod char_dev`:

The driver does a final check:
```
if (you didn't read OR you didn't write)
    print "operations not performed properly"
else if (write happened before read)
    print "operations not performed properly"
else if (write was after the timeout)
    print "operations not performed within time"
else
    print "operations performed successfully"
```

Then it cleans up and removes the device file.

## Key Technical Decisions

### Why Use Jiffies?
Jiffies are the kernel's internal time counter. They increment at each clock tick. They're simple, fast, and wrap around - which is why we use the `time_after()` macro to compare them safely.

### Why Check Order?
The assignment requires operations to happen in a specific order. The check `time_after(read_time, write_time)` tells us if write happened before read (if true, it's wrong).

### Why a Global Buffer?
The kernel buffer needs to survive between function calls (when you read) and between operations (when you write then read). A global works fine for this simple driver.

### Why Record Timestamps on First Operation Only?
If we recorded every time, the timing would be based on when you did the operation last. We only care about the first time each happens to keep it simple.

## File Structure

```c
// At top: includes and globals
char buff[100] = "Utkarsh Gautam"          // Buffer for data
unsigned long read_time = 0                 // When first read happened
unsigned long write_time = 0                // When first write happened
unsigned long start_time                    // When module loaded
unsigned long timeout                       // Timeout window
static int version[2]                       // Parameter: kernel version
static int time                             // Parameter: timeout in seconds

// dev_read() - handles reading
// dev_write() - handles writing
// dev_init() - module initialization
// dev_exit() - module cleanup
```

## Why This Approach?

### Simple
- No complex data structures
- Just timestamps and a buffer
- Easy to understand and debug

### Direct
- Directly checks what was asked for
- No extra features or complications
- Prints messages you can actually read

### Safe
- Uses proper kernel APIs (copy_to_user, copy_from_user)
- Handles memory carefully
- Uses macros for safe time comparisons

## Testing Strategy

We test all the failure cases:
1. **Wrong version** - module shouldn't load
2. **Wrong order** - read after write should fail
3. **Timeout** - operations after timeout should fail
4. **Missing operation** - not doing both should fail
5. **Success case** - read, then write, within timeout should succeed

Each test shows exactly what happens and what message you get.

## What I Learned

### Kernel Concepts Used
- Character device drivers
- Device numbers (major/minor)
- File operations (read/write)
- Module parameters
- Kernel timing with jiffies
- Memory transfer between kernel and userspace

### Practical Lessons
- The kernel has specific patterns for everything (device registration, cleanup, etc.)
- Always check return values
- Use proper kernel macros (time_after, copy_to_user, etc.)
- Keep things simple - complex drivers are hard to debug
- Print messages so you can see what's happening (that's what dmesg is for)

## Summary

This driver is a good example of a kernel module that:
- Takes parameters
- Validates them
- Tracks events
- Reports results

It shows the basic pattern used in many real kernel drivers, just much simpler.

The key insight: the driver enforces a set of rules (version match, read before write, within timeout) and tells you if you followed them. That's it.

Author: Utkarsh Gautam
Date: March 2026
