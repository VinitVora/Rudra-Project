#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <time.h>

#define HEADER_SIZE 8
#define MAGIC "XOR1"
#define EXPECTED_PASSWORD "secret"

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex for safe progress printing

typedef struct {
    unsigned char *data;
    size_t start;
    size_t end;
    unsigned char *key;
    size_t keySize;
    int verbose;
} ThreadData;

uint32_t computeChecksum(const unsigned char *data, size_t size) {
    uint32_t sum = 0;
    for (size_t i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum;
}

void secure_wipe(void *buffer, size_t length) {
    volatile unsigned char *vbuf = (volatile unsigned char *)buffer;
    for (size_t i = 0; i < length; i++) {
        vbuf[i] = 0;
    }
}

void *processChunk(void *arg) {
    ThreadData *td = (ThreadData *)arg;
    for (size_t i = td->start; i < td->end; i++) {
        td->data[i] ^= td->key[i % td->keySize];
        if (td->verbose && (i - td->start) % 1000000 == 0) {
            pthread_mutex_lock(&print_mutex);
            printf(".");
            fflush(stdout);
            pthread_mutex_unlock(&print_mutex);
        }
    }
    return NULL;
}

// Function to print the banner
void print_banner() {
    printf("\n");
    printf("=====================================\n");
    printf("          VINCLI - XOR Encryption    \n");
    printf("        Version 1.0 - Secure Files   \n");
    printf("=====================================\n");
    printf("  Encrypt and decrypt files securely \n");
    printf("=====================================\n");
}

void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s <input_file> <output_file> <key_file> [-p <password>] [-t <threads>] [-v] [-d|-e]\n", progname);
}

int main(int argc, char *argv[]) {
    // Display the banner
    print_banner();
    
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    int numThreads = 2;
#ifdef _SC_NPROCESSORS_ONLN
    numThreads = sysconf(_SC_NPROCESSORS_ONLN);
    if (numThreads < 1) numThreads = 2;
#endif

    char *userPassword = NULL;
    int verbose = 0;
    int explicitMode = 0, decryptMode = 0;

    for (int i = 4; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0 && (i + 1) < argc) {
            userPassword = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0 && (i + 1) < argc) {
            numThreads = atoi(argv[++i]);
            if (numThreads < 1) numThreads = 2;
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-d") == 0) {
            decryptMode = 1;
            explicitMode = 1;
        } else if (strcmp(argv[i], "-e") == 0) {
            decryptMode = 0;
            explicitMode = 1;
        } else {
            print_usage(argv[0]);
            return 1;
        }
    }

    if (!userPassword) {
        userPassword = getenv("RUDRA_PASSWORD");
    }
    if (!userPassword) {
        userPassword = getpass("Enter password: ");
    }

    if (strcmp(userPassword, EXPECTED_PASSWORD) != 0) {
        fprintf(stderr, "Authentication failed!\n");
        return 1;
    }

    FILE *kf = fopen(argv[3], "rb");
    if (!kf) { perror("Key file"); return 1; }
    fseek(kf, 0, SEEK_END);
    size_t keySize = ftell(kf);
    rewind(kf);
    if (keySize == 0) { fprintf(stderr, "Key file is empty.\n"); fclose(kf); return 1; }
    unsigned char *keyBuffer = malloc(keySize);
    if (fread(keyBuffer, 1, keySize, kf) != keySize) {
        fprintf(stderr, "Failed to read full key file.\n");
        fclose(kf);
        free(keyBuffer);
        return 1;
    }
    fclose(kf);

    FILE *inf = fopen(argv[1], "rb");
    if (!inf) { perror("Input file"); free(keyBuffer); return 1; }
    fseek(inf, 0, SEEK_END);
    size_t fileSize = ftell(inf);
    rewind(inf);
    unsigned char *fileData = malloc(fileSize);
    if (fread(fileData, 1, fileSize, inf) != fileSize) {
        fprintf(stderr, "Failed to read full input file.\n");
        fclose(inf);
        free(keyBuffer);
        free(fileData);
        return 1;
    }
    fclose(inf);

    uint32_t originalChecksum = 0;

    // Debugging: Print the beginning of the file
    if (verbose) {
        printf("File data begins with: ");
        for (size_t i = 0; i < 16 && i < fileSize; i++) {
            printf("%02x ", fileData[i]);
        }
        printf("\n");
    }

    // Check if the file is encrypted (magic header check)
    if (!explicitMode && fileSize >= HEADER_SIZE && memcmp(fileData, MAGIC, 4) == 0) {
        decryptMode = 1;
    }

    // If decrypting, extract header checksum and remove header.
    if (decryptMode && fileSize >= HEADER_SIZE && memcmp(fileData, MAGIC, 4) == 0) {
        originalChecksum = fileData[4] | (fileData[5] << 8) | (fileData[6] << 16) | (fileData[7] << 24);
        memmove(fileData, fileData + HEADER_SIZE, fileSize - HEADER_SIZE);
        fileSize -= HEADER_SIZE;

        // Debugging: After shifting the data, check the first few bytes
        if (verbose) {
            printf("Decrypted file begins with: ");
            for (size_t i = 0; i < 16 && i < fileSize; i++) {
                printf("%02x ", fileData[i]);
            }
            printf("\n");
        }
    } else {
        // In encryption mode, compute the checksum on the plaintext BEFORE encryption.
        originalChecksum = computeChecksum(fileData, fileSize);
    }

    pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
    ThreadData *tdArray = malloc(numThreads * sizeof(ThreadData));
    size_t chunkSize = fileSize / numThreads;

    if (verbose) printf("Processing with %d threads\n", numThreads);

    for (int i = 0; i < numThreads; i++) {
        tdArray[i].data = fileData;
        tdArray[i].start = i * chunkSize;
        tdArray[i].end = (i == numThreads - 1) ? fileSize : (i + 1) * chunkSize;
        tdArray[i].key = keyBuffer;
        tdArray[i].keySize = keySize;
        tdArray[i].verbose = verbose;
        pthread_create(&threads[i], NULL, processChunk, &tdArray[i]);
    }
    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    secure_wipe(keyBuffer, keySize);
    free(keyBuffer);
    free(threads);
    free(tdArray);

    // For decryption, compute checksum on decrypted plain text and verify.
    if (decryptMode) {
        uint32_t decryptedChecksum = computeChecksum(fileData, fileSize);
        printf("Original checksum: %u\n", originalChecksum);
        printf("Decrypted checksum: %u\n", decryptedChecksum);

        if (decryptedChecksum != originalChecksum) {
            fprintf(stderr, "Warning: Checksum mismatch. Data integrity compromised.\n");
        } else {
            printf("Decryption successful. Integrity verified.\n");
        }
    } else {
        // In encryption mode, use the precomputed plaintext checksum in the header.
        unsigned char *newData = malloc(fileSize + HEADER_SIZE);
        memcpy(newData, MAGIC, 4);
        newData[4] = originalChecksum & 0xFF;
        newData[5] = (originalChecksum >> 8) & 0xFF;
        newData[6] = (originalChecksum >> 16) & 0xFF;
        newData[7] = (originalChecksum >> 24) & 0xFF;
        memcpy(newData + HEADER_SIZE, fileData, fileSize);
        free(fileData);
        fileData = newData;
        fileSize += HEADER_SIZE;
        printf("Encryption successful. Header with plaintext checksum added.\n");
    }

    FILE *of = fopen(argv[2], "wb");
    if (!of) { perror("Output file"); free(fileData); return 1; }
    fwrite(fileData, 1, fileSize, of);
    fclose(of);
    free(fileData);

    pthread_mutex_destroy(&print_mutex);  // Cleanup mutex

    printf("Output written to %s\n", argv[2]);
    return 0;
}