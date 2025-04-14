# Rudra - XOR-Based Encryption Program

Rudra is a secure and efficient command-line encryption tool designed to provide XOR-based encryption with multi-threading support for fast and secure file processing. The program supports both encryption and decryption, password protection, and secure memory handling.

## Features
- **XOR-Based Encryption**: Utilizes the XOR cipher for simple yet secure encryption/decryption.
- **Multi-Threading**: Speeds up the process with multi-threading for handling large files efficiently.
- **Password Protection**: Ensures that the program is used securely by requiring a password.
- **Secure Memory Handling**: Includes memory wiping techniques to ensure sensitive data is not left in memory.
- **Checksum Validation**: Verifies the integrity of files during decryption.

## Installation

### Prerequisites
Ensure that you have the following dependencies installed:
- GCC (GNU Compiler Collection)
- Make
- POSIX Threads

### Steps to Install

1. **Clone the repository** (or download the source code):
   ```bash
   git clone https://github.com/username/rudra-encryption.git
   cd rudra-encryption
