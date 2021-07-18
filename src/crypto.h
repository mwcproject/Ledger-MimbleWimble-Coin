// Header guard
#ifndef CRYPTO_H
#define CRYPTO_H


// Header files
#include <os.h>
#include <stdbool.h>
#include "secp256k1_generator.h"


// Definitions

// Blinding factor size
#define BLINDING_FACTOR_SIZE 32


// Constants

// Nonce size
extern const size_t NONCE_SIZE;

// Commitment size
extern const size_t COMMITMENT_SIZE;

// Compressed public key size
extern const size_t COMPRESSED_PUBLIC_KEY_SIZE;

// Uncompressed public key size
extern const size_t UNCOMPRESSED_PUBLIC_KEY_SIZE;

// Public key prefix size
extern const size_t PUBLIC_KEY_PREFIX_SIZE;

// Generator G
extern const secp256k1_generator GENERATOR_G;

// Generator H
extern const secp256k1_generator GENERATOR_H;

// Identifier maximum depth
extern const size_t IDENTIFIER_MAXIMUM_DEPTH;

// Identifier size
extern const size_t IDENTIFIER_SIZE;

// Single-signer compact signature size
extern const size_t SINGLE_SIGNER_COMPACT_SIGNATURE_SIZE;

// Single-signer message size
extern const size_t SINGLE_SIGNER_MESSAGE_SIZE;

// Maximum DER signature size
extern const size_t MAXIMUM_DER_SIGNATURE_SIZE;

// Keep private
extern const int KEEP_PRIVATE_KEY;

// Even compressed public key prefix
extern const uint8_t EVEN_COMPRESSED_PUBLIC_KEY_PREFIX;

// Odd compressed public key prefix
extern const uint8_t ODD_COMPRESSED_PUBLIC_KEY_PREFIX;

// Bulletproof size
extern const size_t BULLETPROOF_SIZE;

// Proof message size
extern const size_t PROOF_MESSAGE_SIZE;

// Proof message switch type index
extern const size_t PROOF_MESSAGE_SWITCH_TYPE_INDEX;

// Proof message identifier index
extern const size_t PROOF_MESSAGE_IDENTIFIER_INDEX;


// Switch type
enum SwitchType {

	// No switch type
	NO_SWITCH_TYPE,
	
	// Regular switch type
	REGULAR_SWITCH_TYPE
};


// Function prototypes

// Get private key and chain code
void getPrivateKeyAndChainCode(volatile cx_ecfp_private_key_t *privateKey, volatile uint8_t *chainCode);

// Get public key from private key
void getPublicKeyFromPrivateKey(uint8_t *publicKey, const cx_ecfp_private_key_t *privateKey);

// Derive child key
void deriveChildKey(volatile cx_ecfp_private_key_t *privateKey, volatile uint8_t *chainCode, const uint32_t *path, size_t pathLength, bool useProvidedPrivateKeyAndChainCode);

// Derive blinding factor
void deriveBlindingFactor(volatile uint8_t *blindingFactor, uint64_t value, const uint32_t *path, size_t pathLength, enum SwitchType switchType);

// Commit value
void commitValue(volatile uint8_t *commitment, uint64_t value, const uint8_t *blindingFactor);

// Get rewind nonce
void getRewindNonce(volatile uint8_t *rewindNonce, const uint8_t *commitment);

// Get private nonce
void getPrivateNonce(volatile uint8_t *privateNonce, const uint8_t *commitment);

// Get address private key
void getAddressPrivateKey(volatile cx_ecfp_private_key_t *addressPrivateKey, uint32_t index, cx_curve_t curve);

// Update blinding factor sum
void updateBlindingFactorSum(volatile uint8_t *blindingFactorSum, const uint8_t *blindingFactor, bool blindingFactorIsPositive);

// Create single-signer signature
void createSingleSignerSignature(volatile uint8_t *signature, const uint8_t *message, const cx_ecfp_private_key_t *privateKey, const uint8_t *publicKey);

// Get encrypted data length
size_t getEncryptedDataLength(size_t dataLength);

// Encrypt data
void encryptData(volatile uint8_t *result, const uint8_t *data, size_t dataLength, const uint8_t *key, size_t keyLength);

// Get X25519 private key from Ed25519 private key
void getX25519PrivateKeyFromEd25519PrivateKey(volatile cx_ecfp_private_key_t *x25519PrivateKey, const cx_ecfp_private_key_t *ed25519PrivateKey);

// Get payment proof message length
size_t getPaymentProofMessageLength(uint64_t value, size_t senderAddressLength);

// Get payment proof message
void getPaymentProofMessage(uint8_t *message, uint64_t value, const uint8_t *commitment, const uint8_t *senderAddress, size_t senderAddressLength);

// Commitment is valid
bool commitmentIsValid(const uint8_t *commitment);

// Calculate bulletproof
void calculateBulletproof(volatile uint8_t *bulletproof, volatile size_t *bulletproofLength, const uint64_t *value, const uint8_t *blindingFactor, const uint8_t *rewindNonce, const uint8_t *privateNonce, const uint8_t *proofMessage);

// Is valid Ed25519 public key
bool isValidEd25519PublicKey(const uint8_t *publicKey, size_t length);

// Is valid secp256k1 private key
bool isValidSecp256k1PrivateKey(const uint8_t *privateKey, size_t length);

// Is valid secp256k1 public key
bool isValidSecp256k1PublicKey(const uint8_t *publicKey, size_t length);


#endif
