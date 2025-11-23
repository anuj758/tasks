
# Task 2: Multithreaded File Compression Tool

## Project Overview
A tool that compresses and decompresses files using multithreading to optimize performance.

## Features
- Parallel file compression and decompression using C++ `<thread>`
- Command-line interface for selecting operations
- Resource synchronization and safe concurrency handling

## Technologies
- C++11/14
- `<thread>`, `<fstream>`, synchronization primitives

## Setup and Usage
1. Compile: `g++ -std=c++14 task2.cpp -o compression_tool -pthread`
2. Run: `./compression_tool`
3. Follow CLI prompts to compress/decompress files
