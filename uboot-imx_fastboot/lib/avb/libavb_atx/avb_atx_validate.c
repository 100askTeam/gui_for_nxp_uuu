/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "avb_atx_validate.h"

#include <libavb/avb_rsa.h>
#include <libavb/avb_sha.h>
#include <libavb/avb_sysdeps.h>
#include <libavb/avb_util.h>

/* The most recent unlock challenge generated. */
static uint8_t last_unlock_challenge[AVB_ATX_UNLOCK_CHALLENGE_SIZE];

/* Computes the SHA256 |hash| of |length| bytes of |data|. */
static void sha256(const uint8_t* data,
                   uint32_t length,
                   uint8_t hash[AVB_SHA256_DIGEST_SIZE]) {
  AvbSHA256Ctx context;
  avb_sha256_init(&context);
  avb_sha256_update(&context, data, length);
  uint8_t* tmp = avb_sha256_final(&context);
  avb_memcpy(hash, tmp, AVB_SHA256_DIGEST_SIZE);
}

/* Computes the SHA512 |hash| of |length| bytes of |data|. */
static void sha512(const uint8_t* data,
                   uint32_t length,
                   uint8_t hash[AVB_SHA512_DIGEST_SIZE]) {
  AvbSHA512Ctx context;
  avb_sha512_init(&context);
  avb_sha512_update(&context, data, length);
  uint8_t* tmp = avb_sha512_final(&context);
  avb_memcpy(hash, tmp, AVB_SHA512_DIGEST_SIZE);
}

/* Computes the SHA256 |hash| of a NUL-terminated |str|. */
static void sha256_str(const char* str, uint8_t hash[AVB_SHA256_DIGEST_SIZE]) {
  sha256((const uint8_t*)str, avb_strlen(str), hash);
}

/* Verifies structure and |expected_hash| of permanent |attributes|. */
static bool verify_permanent_attributes(
    const AvbAtxPermanentAttributes* attributes,
    const uint8_t expected_hash[AVB_SHA256_DIGEST_SIZE]) {
  uint8_t hash[AVB_SHA256_DIGEST_SIZE];

  if (attributes->version != 1) {
    avb_error("Unsupported permanent attributes version.\n");
    return false;
  }
  sha256((const uint8_t*)attributes, sizeof(AvbAtxPermanentAttributes), hash);
  if (0 != avb_safe_memcmp(hash, expected_hash, AVB_SHA256_DIGEST_SIZE)) {
    avb_error("Invalid permanent attributes.\n");
    return false;
  }
  return true;
}

/* Verifies the format, key version, usage, and signature of a certificate. */
static bool verify_certificate(
    const AvbAtxCertificate* certificate,
    const uint8_t authority[AVB_ATX_PUBLIC_KEY_SIZE],
    uint64_t minimum_key_version,
    const uint8_t expected_usage[AVB_SHA256_DIGEST_SIZE]) {
  const AvbAlgorithmData* algorithm_data;
  uint8_t certificate_hash[AVB_SHA512_DIGEST_SIZE];

  if (certificate->signed_data.version != 1) {
    avb_error("Unsupported certificate format.\n");
    return false;
  }
  algorithm_data = avb_get_algorithm_data(AVB_ALGORITHM_TYPE_SHA512_RSA4096);
  sha512((const uint8_t*)&certificate->signed_data,
         sizeof(AvbAtxCertificateSignedData),
         certificate_hash);
  if (!avb_rsa_verify(authority,
                      AVB_ATX_PUBLIC_KEY_SIZE,
                      certificate->signature,
                      AVB_RSA4096_NUM_BYTES,
                      certificate_hash,
                      AVB_SHA512_DIGEST_SIZE,
                      algorithm_data->padding,
                      algorithm_data->padding_len)) {
    avb_error("Invalid certificate signature.\n");
    return false;
  }
  if (certificate->signed_data.key_version < minimum_key_version) {
    avb_error("Key rollback detected.\n");
    return false;
  }
  if (0 != avb_safe_memcmp(certificate->signed_data.usage,
                           expected_usage,
                           AVB_SHA256_DIGEST_SIZE)) {
    avb_error("Invalid certificate usage.\n");
    return false;
  }
  return true;
}

/* Verifies signature and fields of a PIK certificate. */
static bool verify_pik_certificate(
    const AvbAtxCertificate* certificate,
    const uint8_t authority[AVB_ATX_PUBLIC_KEY_SIZE],
    uint64_t minimum_version) {
  uint8_t expected_usage[AVB_SHA256_DIGEST_SIZE];

  sha256_str("com.google.android.things.vboot.ca", expected_usage);
  if (!verify_certificate(
          certificate, authority, minimum_version, expected_usage)) {
    avb_error("Invalid PIK certificate.\n");
    return false;
  }
  return true;
}

/* Verifies signature and fields of a PSK certificate. */
static bool verify_psk_certificate(
    const AvbAtxCertificate* certificate,
    const uint8_t authority[AVB_ATX_PUBLIC_KEY_SIZE],
    uint64_t minimum_version,
    const uint8_t product_id[AVB_ATX_PRODUCT_ID_SIZE]) {
  uint8_t expected_subject[AVB_SHA256_DIGEST_SIZE];
  uint8_t expected_usage[AVB_SHA256_DIGEST_SIZE];

  sha256_str("com.google.android.things.vboot", expected_usage);
  if (!verify_certificate(
          certificate, authority, minimum_version, expected_usage)) {
    avb_error("Invalid PSK certificate.\n");
    return false;
  }
  sha256(product_id, AVB_ATX_PRODUCT_ID_SIZE, expected_subject);
  if (0 != avb_safe_memcmp(certificate->signed_data.subject,
                           expected_subject,
                           AVB_SHA256_DIGEST_SIZE)) {
    avb_error("PSK: Product ID mismatch.\n");
    return false;
  }
  return true;
}

/* Verifies signature and fields of a PUK certificate. */
static bool verify_puk_certificate(
    const AvbAtxCertificate* certificate,
    const uint8_t authority[AVB_ATX_PUBLIC_KEY_SIZE],
    uint64_t minimum_version,
    const uint8_t product_id[AVB_ATX_PRODUCT_ID_SIZE]) {
  uint8_t expected_subject[AVB_SHA256_DIGEST_SIZE];
  uint8_t expected_usage[AVB_SHA256_DIGEST_SIZE];

  sha256_str("com.google.android.things.vboot.unlock", expected_usage);
  if (!verify_certificate(
          certificate, authority, minimum_version, expected_usage)) {
    avb_error("Invalid PUK certificate.\n");
    return false;
  }
  sha256(product_id, AVB_ATX_PRODUCT_ID_SIZE, expected_subject);
  if (0 != avb_safe_memcmp(certificate->signed_data.subject,
                           expected_subject,
                           AVB_SHA256_DIGEST_SIZE)) {
    avb_error("PUK: Product ID mismatch.\n");
    return false;
  }
  return true;
}

AvbIOResult avb_atx_validate_vbmeta_public_key(
    AvbOps* ops,
    const uint8_t* public_key_data,
    size_t public_key_length,
    const uint8_t* public_key_metadata,
    size_t public_key_metadata_length,
    bool* out_is_trusted) {
  AvbIOResult result = AVB_IO_RESULT_OK;
  AvbAtxPermanentAttributes permanent_attributes;
  uint8_t permanent_attributes_hash[AVB_SHA256_DIGEST_SIZE];
  AvbAtxPublicKeyMetadata metadata;
  uint64_t minimum_version;

  /* Be pessimistic so we can exit early without having to remember to clear.
   */
  *out_is_trusted = false;

  /* Read and verify permanent attributes. */
  result = ops->atx_ops->read_permanent_attributes(ops->atx_ops,
                                                   &permanent_attributes);
  if (result != AVB_IO_RESULT_OK) {
    avb_error("Failed to read permanent attributes.\n");
    return result;
  }
  result = ops->atx_ops->read_permanent_attributes_hash(
      ops->atx_ops, permanent_attributes_hash);
  if (result != AVB_IO_RESULT_OK) {
    avb_error("Failed to read permanent attributes hash.\n");
    return result;
  }
  if (!verify_permanent_attributes(&permanent_attributes,
                                   permanent_attributes_hash)) {
    return AVB_IO_RESULT_OK;
  }

  /* Sanity check public key metadata. */
  if (public_key_metadata_length != sizeof(AvbAtxPublicKeyMetadata)) {
    avb_error("Invalid public key metadata.\n");
    return AVB_IO_RESULT_OK;
  }
  avb_memcpy(&metadata, public_key_metadata, sizeof(AvbAtxPublicKeyMetadata));
  if (metadata.version != 1) {
    avb_error("Unsupported public key metadata.\n");
    return AVB_IO_RESULT_OK;
  }

  /* Verify the PIK certificate. */
  result = ops->read_rollback_index(
      ops, AVB_ATX_PIK_VERSION_LOCATION, &minimum_version);
  if (result != AVB_IO_RESULT_OK) {
    avb_error("Failed to read PIK minimum version.\n");
    return result;
  }
  if (!verify_pik_certificate(&metadata.product_intermediate_key_certificate,
                              permanent_attributes.product_root_public_key,
                              minimum_version)) {
    return AVB_IO_RESULT_OK;
  }

  /* Verify the PSK certificate. */
  result = ops->read_rollback_index(
      ops, AVB_ATX_PSK_VERSION_LOCATION, &minimum_version);
  if (result != AVB_IO_RESULT_OK) {
    avb_error("Failed to read PSK minimum version.\n");
    return result;
  }
  if (!verify_psk_certificate(
          &metadata.product_signing_key_certificate,
          metadata.product_intermediate_key_certificate.signed_data.public_key,
          minimum_version,
          permanent_attributes.product_id)) {
    return AVB_IO_RESULT_OK;
  }

  /* Verify the PSK is the same key that verified vbmeta. */
  if (public_key_length != AVB_ATX_PUBLIC_KEY_SIZE) {
    avb_error("Public key length mismatch.\n");
    return AVB_IO_RESULT_OK;
  }
  if (0 != avb_safe_memcmp(
               metadata.product_signing_key_certificate.signed_data.public_key,
               public_key_data,
               AVB_ATX_PUBLIC_KEY_SIZE)) {
    avb_error("Public key mismatch.\n");
    return AVB_IO_RESULT_OK;
  }

  /* Report the key versions used during verification. */
  ops->atx_ops->set_key_version(
      ops->atx_ops,
      AVB_ATX_PIK_VERSION_LOCATION,
      metadata.product_intermediate_key_certificate.signed_data.key_version);
  ops->atx_ops->set_key_version(
      ops->atx_ops,
      AVB_ATX_PSK_VERSION_LOCATION,
      metadata.product_signing_key_certificate.signed_data.key_version);

  *out_is_trusted = true;
  return AVB_IO_RESULT_OK;
}

AvbIOResult avb_atx_generate_unlock_challenge(
    AvbAtxOps* atx_ops, AvbAtxUnlockChallenge* out_unlock_challenge) {
  AvbIOResult result = AVB_IO_RESULT_OK;
  AvbAtxPermanentAttributes permanent_attributes;

  /* We need the permanent attributes to compute the product_id_hash. */
  result = atx_ops->read_permanent_attributes(atx_ops, &permanent_attributes);
  if (result != AVB_IO_RESULT_OK) {
    avb_error("Failed to read permanent attributes.\n");
    return result;
  }
  result = atx_ops->get_random(
      atx_ops, AVB_ATX_UNLOCK_CHALLENGE_SIZE, last_unlock_challenge);
  if (result != AVB_IO_RESULT_OK) {
    avb_error("Failed to generate random challenge.\n");
    return result;
  }
  out_unlock_challenge->version = 1;
  sha256(permanent_attributes.product_id,
         AVB_ATX_PRODUCT_ID_SIZE,
         out_unlock_challenge->product_id_hash);
  avb_memcpy(out_unlock_challenge->challenge,
             last_unlock_challenge,
             AVB_ATX_UNLOCK_CHALLENGE_SIZE);
  return result;
}

AvbIOResult avb_atx_validate_unlock_credential(
    AvbAtxOps* atx_ops,
    const AvbAtxUnlockCredential* unlock_credential,
    bool* out_is_trusted) {
  AvbIOResult result = AVB_IO_RESULT_OK;
  AvbAtxPermanentAttributes permanent_attributes;
  uint8_t permanent_attributes_hash[AVB_SHA256_DIGEST_SIZE];
  uint64_t minimum_version;
  const AvbAlgorithmData* algorithm_data;
  uint8_t challenge_hash[AVB_SHA512_DIGEST_SIZE];

  /* Be pessimistic so we can exit early without having to remember to clear.
   */
  *out_is_trusted = false;

  /* Sanity check the credential. */
  if (unlock_credential->version != 1) {
    avb_error("Unsupported unlock credential format.\n");
    return AVB_IO_RESULT_OK;
  }

  /* Read and verify permanent attributes. */
  result = atx_ops->read_permanent_attributes(atx_ops, &permanent_attributes);
  if (result != AVB_IO_RESULT_OK) {
    avb_error("Failed to read permanent attributes.\n");
    return result;
  }
  result = atx_ops->read_permanent_attributes_hash(atx_ops,
                                                   permanent_attributes_hash);
  if (result != AVB_IO_RESULT_OK) {
    avb_error("Failed to read permanent attributes hash.\n");
    return result;
  }
  if (!verify_permanent_attributes(&permanent_attributes,
                                   permanent_attributes_hash)) {
    return AVB_IO_RESULT_OK;
  }

  /* Verify the PIK certificate. */
  result = atx_ops->ops->read_rollback_index(
      atx_ops->ops, AVB_ATX_PIK_VERSION_LOCATION, &minimum_version);
  if (result != AVB_IO_RESULT_OK) {
    avb_error("Failed to read PIK minimum version.\n");
    return result;
  }
  if (!verify_pik_certificate(
          &unlock_credential->product_intermediate_key_certificate,
          permanent_attributes.product_root_public_key,
          minimum_version)) {
    return AVB_IO_RESULT_OK;
  }

  /* Verify the PUK certificate. The minimum version is shared with the PSK. */
  result = atx_ops->ops->read_rollback_index(
      atx_ops->ops, AVB_ATX_PSK_VERSION_LOCATION, &minimum_version);
  if (result != AVB_IO_RESULT_OK) {
    avb_error("Failed to read PSK minimum version.\n");
    return result;
  }
  if (!verify_puk_certificate(
          &unlock_credential->product_unlock_key_certificate,
          unlock_credential->product_intermediate_key_certificate.signed_data
              .public_key,
          minimum_version,
          permanent_attributes.product_id)) {
    return AVB_IO_RESULT_OK;
  }

  /* Verify the challenge signature. */
  algorithm_data = avb_get_algorithm_data(AVB_ALGORITHM_TYPE_SHA512_RSA4096);
  sha512(last_unlock_challenge, AVB_ATX_UNLOCK_CHALLENGE_SIZE, challenge_hash);
  if (!avb_rsa_verify(unlock_credential->product_unlock_key_certificate
                          .signed_data.public_key,
                      AVB_ATX_PUBLIC_KEY_SIZE,
                      unlock_credential->challenge_signature,
                      AVB_RSA4096_NUM_BYTES,
                      challenge_hash,
                      AVB_SHA512_DIGEST_SIZE,
                      algorithm_data->padding,
                      algorithm_data->padding_len)) {
    avb_error("Invalid unlock challenge signature.\n");
    return AVB_IO_RESULT_OK;
  }

  *out_is_trusted = true;
  return AVB_IO_RESULT_OK;
}
