#include "CryptF.h"

#define AES_BLOCK_SIZE 16

void HashString(const char* string, unsigned char* outputHash) {
    if (string == NULL || strlen(string) == 0) {
        printf("The password cannot be blank.\n");
        return;
    }
    SHA256((unsigned char*)string, strlen(string), outputHash);
}

void hashToHex(const unsigned char* hash, char* hexOutput) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&hexOutput[i * 2], "%02x", hash[i]);
    }
}

static const unsigned char user_key[16] = "1234567890ABCDEF";  // AES key (must be kept secure!)

void EncryptGUID(const char* plainText, unsigned char* cipherText, size_t* cipherLen) {
    AES_KEY aesKey;  // AES_KEY structure to hold encryption key
    if (AES_set_encrypt_key(user_key, 128, &aesKey) < 0) {  // Set 128-bit AES encryption key
        printf("Encryption key setup failed!\n");
        return;
    }

    size_t len = strlen(plainText);
    size_t paddedLen = ((len / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
    unsigned char* paddedText = (unsigned char*)calloc(paddedLen, sizeof(unsigned char));
    strncpy((char*)paddedText, plainText, len);

    // Encrypt data in blocks
    for (size_t i = 0; i < paddedLen; i += AES_BLOCK_SIZE) {
        AES_encrypt(paddedText + i, cipherText + i, &aesKey);
    }

    *cipherLen = paddedLen;
    free(paddedText);
}

void DecryptGUID(const unsigned char* cipherText, char* plainText, size_t cipherLen) {
    AES_KEY aesKey;
    if (AES_set_decrypt_key(user_key, 128, &aesKey) < 0) {  // Set 128-bit AES decryption key
        printf("Decryption key setup failed!\n");
        return;
    }

    size_t blockCount = cipherLen / AES_BLOCK_SIZE;
    for (size_t i = 0; i < blockCount; ++i) {
        unsigned char decrypted[AES_BLOCK_SIZE];
        AES_decrypt(cipherText + (i * AES_BLOCK_SIZE), decrypted, &aesKey);
        memcpy(plainText + (i * AES_BLOCK_SIZE), decrypted, AES_BLOCK_SIZE);
    }

    plainText[cipherLen] = '\0';  // Null-terminate the decrypted string
}
