#include "shared.h"
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

void HashString(const char* password, unsigned char* outputHash);
void hashToHex(const unsigned char* hash, char* hexOutput);
void EncryptGUID(const char* plainText, unsigned char* cipherText, size_t* cipherLen);
void DecryptGUID(const unsigned char* cipherText, char* plainText, size_t cipherLen);