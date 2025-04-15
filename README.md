# Rudra - XOR-Based Encryption Program

Rudra is a secure and efficient command-line encryption tool designed to provide XOR-based encryption with multi-threading support for fast and secure file processing. The program supports both encryption and decryption, password protection, and secure memory handling.

## Features
- **XOR-Based Encryption**: Utilizes the XOR cipher for simple yet secure encryption/decryption.
- **Multi-Threading**: Speeds up the process with multi-threading for handling large files efficiently.
- **Password Protection**: Ensures that the program is used securely by requiring a password.
- **Secure Memory Handling**: Includes memory wiping techniques to ensure sensitive data is not left in memory.
- **Checksum Validation**: Verifies the integrity of files during decryption.
- **Password Management**: You can dynamically set and remove the password using environment variables. Use `echo` to set the password and `unset` to remove it:
  - **Set Password**: 
    ```bash
    echo "export RUDRA_PASSWORD='your_password'" >> ~/.bashrc
    source ~/.bashrc
    ```
  - **Remove Password**:
    ```bash
    unset RUDRA_PASSWORD
    ```

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


## Usage

### Basic Usage
The basic syntax for using Rudra is as follows:

```bash
rudra <input_file> <output_file> <key_file> [-p <password>] [-t <threads>] [-v] [-d|-e]
```

### Arguments:
- `<input_file>`: The file you want to encrypt or decrypt.
- `<output_file>`: The name of the file where the encrypted/decrypted data will be saved.
- `<key_file>`: The file containing the key used for encryption/decryption.
- `-p <password>` (optional): Password to authenticate the user (default is `secret`).
- `-t <threads>` (optional): Number of threads to use for processing. Default is 2.
- `-v` (optional): Enable verbose mode to show progress.
- `-d` (optional): Decrypt the file (default is encrypt).
- `-e` (optional): Encrypt the file (default).

### Examples:

#### Encrypt a file:
```bash
rudra myfile.txt encrypted_output.bin keyfile.key -e -p mypassword
```

#### Decrypt a file:
```bash
rudra encrypted_output.bin decrypted_output.txt keyfile.key -d -p mypassword
```

### Important Notes:
- The program uses XOR encryption and appends a header containing a checksum when encrypting.
- For decryption, the checksum is verified to ensure data integrity.
- The password is required to prevent unauthorized access. By default, it is `secret`.
- You can dynamically set or remove the password using the environment variables, as described in the features section.

## Contributing

Contributions are welcome! If you find any bugs or want to add a feature, feel free to fork the repository and create a pull request.

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make your changes.
4. Commit your changes (`git commit -am 'Add new feature'`).
5. Push to the branch (`git push origin feature-branch`).
6. Create a new pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
