// Header files
#define _GNU_SOURCE
#include <string.h>
#undef _GNU_SOURCE
#include "common.h"
#include "crypto.h"
#include "get_tor_transaction_signature.h"
#include "tor.h"


// Supporting function implementation

// Process get Tor transaction signature request
void processGetTorTransactionSignatureRequest(unsigned short *responseLength, unsigned char *responseFlags) {

	// Get request's first parameter
	const uint8_t firstParameter = G_io_apdu_buffer[APDU_OFF_P1];
	
	// Get request's second parameter
	const uint8_t secondParameter = G_io_apdu_buffer[APDU_OFF_P2];
	
	// Get request's data length
	const size_t dataLength = G_io_apdu_buffer[APDU_OFF_LC];
	
	// Get request's data
	const uint8_t *data = &G_io_apdu_buffer[APDU_OFF_DATA];
	
	// Check if parameters or data are invalid
	if(firstParameter || secondParameter || dataLength <= sizeof(uint64_t) + COMMITMENT_SIZE) {
	
		// Throw invalid parameters error
		THROW(INVALID_PARAMETERS_ERROR);
	}
	
	// Get value from data
	const uint64_t *value = (uint64_t *)data;
	
	// Check if value is invalid
	if(!*value) {
	
		// Throw invalid parameters error
		THROW(INVALID_PARAMETERS_ERROR);
	}
	
	// Get commitment from data
	const uint8_t *commitment = &data[sizeof(*value)];
	
	// Check if commitment is invalid
	if(!commitmentIsValid(commitment)) {
	
		// Throw invalid parameters error
		THROW(INVALID_PARAMETERS_ERROR);
	}
	
	// Get sender address from data
	const uint8_t *senderAddress = &data[sizeof(*value) + COMMITMENT_SIZE];
	
	// Get sender address length
	const size_t senderAddressLength = dataLength - (sizeof(*value) + COMMITMENT_SIZE);
	
	// Get payment proof message
	uint8_t paymentProofMessage[getPaymentProofMessageLength(*value, senderAddressLength)];
	
	getPaymentProofMessage(paymentProofMessage, *value, commitment, senderAddress, senderAddressLength);
	
	// Initialize address private key
	volatile cx_ecfp_private_key_t addressPrivateKey;
	
	// Initialize signature
	volatile uint8_t signature[ED25519_SIGNATURE_SIZE];
	
	// Begin try
	BEGIN_TRY {
	
		// Try
		TRY {
		
			// Get address private key at the Tor address private key index
			getAddressPrivateKey(&addressPrivateKey, TOR_ADDRESS_PRIVATE_KEY_INDEX, CX_CURVE_Ed25519);
			
			// Get address public key from address private key
			cx_ecfp_public_key_t addressPublicKey;
			cx_ecfp_generate_pair(CX_CURVE_Ed25519, &addressPublicKey, (cx_ecfp_private_key_t *)&addressPrivateKey, KEEP_PRIVATE_KEY);
			
			// Check if the address public key is in the payment proof message
			if(memmem(paymentProofMessage, sizeof(paymentProofMessage), addressPublicKey.W, addressPublicKey.W_len)) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
			
			// Compress the address public key
			cx_edwards_compress_point(CX_CURVE_Ed25519, addressPublicKey.W, addressPublicKey.W_len);
			
			// Check if the compressed address public key is in the payment proof message
			if(memmem(paymentProofMessage, sizeof(paymentProofMessage), &addressPublicKey.W[PUBLIC_KEY_PREFIX_SIZE], ED25519_PUBLIC_KEY_SIZE)) {
			
				// Throw invalid parameters error
				THROW(INVALID_PARAMETERS_ERROR);
			}
		
			// Get signature of the payment proof address
			cx_eddsa_sign((cx_ecfp_private_key_t *)&addressPrivateKey, CX_LAST, CX_SHA512, paymentProofMessage, sizeof(paymentProofMessage), NULL, 0, (uint8_t *)signature, sizeof(signature), NULL);
		}
		
		// Finally
		FINALLY {
		
			// Clear the address private key
			explicit_bzero((cx_ecfp_private_key_t *)&addressPrivateKey, sizeof(addressPrivateKey));
		}
	}
	
	// End try
	END_TRY;
	
	// Check if response with the signature will overflow
	if(willResponseOverflow(*responseLength, sizeof(signature))) {
	
		// Throw length error
		THROW(ERR_APD_LEN);
	}
	
	// Append signature to response
	memcpy(&G_io_apdu_buffer[*responseLength], (uint8_t *)signature, sizeof(signature));
	
	*responseLength += sizeof(signature);
	
	// Throw success
	THROW(SWO_SUCCESS);
}
