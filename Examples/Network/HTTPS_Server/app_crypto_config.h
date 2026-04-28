/**
 * PSA crypto configuration for HTTPS server
 *
 * Copyright The Mbed TLS Contributors
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#define TF_PSA_CRYPTO_CONFIG_VERSION 0x01000000

/* Cryptographic mechanism selection (PSA API) */
#define PSA_WANT_ALG_ECDH                       1
#define PSA_WANT_ALG_ECDSA                      1
#define PSA_WANT_ALG_GCM                        1
#define PSA_WANT_ALG_RSA_OAEP                   1
#define PSA_WANT_ALG_RSA_PKCS1V15_CRYPT         1
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN          1
#define PSA_WANT_ALG_RSA_PSS                    1
#define PSA_WANT_ALG_SHA_256                    1
#define PSA_WANT_ALG_TLS12_PRF                  1
#define PSA_WANT_ECC_SECP_R1_256                1
#define PSA_WANT_KEY_TYPE_DERIVE                1
#define PSA_WANT_KEY_TYPE_HMAC                  1
#define PSA_WANT_KEY_TYPE_AES                   1
#define PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY        1
#define PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY        1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC    1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT   1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC    1
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT   1

/* Platform abstraction layer */
#define MBEDTLS_PLATFORM_C

/* Cryptographic mechanism selection (extended API) */
#define MBEDTLS_MD_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_PARSE_EC_EXTENDED

/* Data format support */
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_PEM_PARSE_C

/* PSA core */
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_PSA_DRIVER_GET_ENTROPY

/* Builtin drivers */
#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_ECP_NIST_OPTIM
