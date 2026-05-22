# EmbedKit_SakshiSuryawanshi

## Name
Sakshi Suryawanshi

## Project Title
UART Frame Parser (Embedded C)

## Description
UART frame parser implemented in C using a state machine with checksum validation and inter-byte timeout handling.

## Module

### uart_parser.c
Standalone C program that implements a UART frame parser. It processes incoming bytes one at a time, builds frames using a state machine, validates data using XOR checksum, and handles inter-byte timeout for error recovery.

## Features
- State machine-based UART parsing
- Start-of-frame detection (0xAA)
- CMD, LEN, PAYLOAD, CHECKSUM processing
- XOR checksum validation
- Inter-byte timeout handling (50ms)
- Automatic parser reset on error
- Fully C99 compliant implementation

## Build Instructions

Compile:
```bash
gcc -Wall -std=c99 uart_parser.c -o app
