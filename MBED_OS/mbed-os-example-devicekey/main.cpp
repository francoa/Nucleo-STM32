/*
* Copyright (c) 2018 ARM Limited. All rights reserved.
* SPDX-License-Identifier: Apache-2.0
* Licensed under the Apache License, Version 2.0 (the License); you may
* not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an AS IS BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "mbed.h"
#include "DeviceKey.h"

Serial pc(USBTX, USBRX); // tx, rx

//print a unsigned char buffer in hex format
void print_buffer(unsigned char *buf, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        pc.printf("%02X", buf[i]);
    }
}

//Injection of a dummy key when there is no TRNG
int inject_rot_key()
{
    uint32_t key[DEVICE_KEY_16BYTE / sizeof(uint32_t)];

    memset(key, 0, DEVICE_KEY_16BYTE);
    memcpy(key, "ABCDEF1234567890", DEVICE_KEY_16BYTE);
    int size = DEVICE_KEY_16BYTE;
    DeviceKey& devkey = DeviceKey::get_instance();
    return devkey.device_inject_root_of_trust(key, size);
}

// Entry point for the example
int main()
{
    unsigned char derive_key1 [DEVICE_KEY_32BYTE];
    unsigned char derive_key2 [DEVICE_KEY_32BYTE];
    unsigned char salt1[] = "SALT1 ----- SALT1 ------ SALT1";
    unsigned char salt2[] = "SALT2 ----- SALT2 ------ SALT2";
    int ret = DEVICEKEY_SUCCESS;

    pc.printf("\r\n--- Mbed OS DeviceKey example ---\r\n");

    //DeviceKey is a singleton
    DeviceKey& devkey = DeviceKey::get_instance();

#if not defined(DEVICE_TRNG)

    //If TRNG is not available it is a must to inject the ROT before the first call to derive key method.
    pc.printf("\r\n--- No TRNG support for this device. injecting ROT. ---\r\n");
    ret = inject_rot_key();
    if (DEVICEKEY_SUCCESS != ret && DEVICEKEY_ALREADY_EXIST != ret) {
        pc.printf("\r\n--- Error, injection of ROT key has failed with status %d ---\r\n", ret);
        return -1;
    }

    if ( DEVICEKEY_ALREADY_EXIST == ret ) {
        pc.printf("\r\n--- ROT Key already exists in the persistent memory. ---\r\n", ret);
    } else {
        pc.printf("\r\n--- ROT Key injected and stored in persistent memory. ---\r\n", ret);
    }

#endif

    pc.printf("\r\n--- Using the following salt for key derivation: %s ---\r\n", salt1);

    //16 byte key derivation.
    pc.printf("--- First call to derive key, requesting derived key of 16 byte ---\r\n");
    ret = devkey.generate_derived_key(salt1, sizeof(salt1), derive_key1, DEVICE_KEY_16BYTE);
    if (DEVICEKEY_SUCCESS != ret) {
        pc.printf("\r\n--- Error, derive key failed with error code %d ---\r\n", ret);
        return -1;
    }

    pc.printf("--- Derived key1 is: \r\n");
    print_buffer(derive_key1, DEVICE_KEY_16BYTE);
    pc.printf("\r\n");

    //16 byte key derivation with the same salt should result with the same derived key.
    pc.printf("\r\n--- Second call to derive key with the same salt. ---\r\n");
    ret = devkey.generate_derived_key(salt1, sizeof(salt1), derive_key2, DEVICE_KEY_16BYTE);
    if (DEVICEKEY_SUCCESS != ret) {
        pc.printf("\r\n--- Error, derive key failed with error code %d ---\r\n", ret);
        return -1;
    }

    pc.printf("--- Derived key2 should be equal to key1 from the first call. key2 is: \r\n");
    print_buffer(derive_key2, DEVICE_KEY_16BYTE);
    pc.printf("\r\n");

    if (memcmp(derive_key1, derive_key2, DEVICE_KEY_16BYTE) != 0) {
        pc.printf("--- Error, first key and second key do not match ---\r\n");
        return -1;
    } else {
        pc.printf("--- Keys match ---\r\n");
    }

    pc.printf("\r\n--- Using the following salt for key derivation %s ---\r\n", salt2);

    //16 byte key derivation with the different salt should result with new derived key.
    ret = devkey.generate_derived_key(salt2, sizeof(salt2), derive_key1, DEVICE_KEY_16BYTE);
    if (DEVICEKEY_SUCCESS != ret) {
        pc.printf("\r\n--- Error, derive key failed with error code %d ---\r\n", ret);
        return -1;
    }

    pc.printf("--- Third call to derive key with the different salt should result with a new derived key1: \r\n");
    print_buffer(derive_key1, DEVICE_KEY_16BYTE);
    pc.printf("\r\n");

    if (memcmp(derive_key1, derive_key2, DEVICE_KEY_16BYTE) == 0) {
        pc.printf("--- Error, first key and second key do not match ---\r\n");
        return -1;
    } else {
        pc.printf("--- Keys not match ---\r\n");
    }

    //32 byte key derivation.
    pc.printf("\r\n--- 32 byte key derivation example. ---\r\n");
    ret = devkey.generate_derived_key(salt2, sizeof(salt2), derive_key2, DEVICE_KEY_32BYTE);
    if (DEVICEKEY_SUCCESS != ret) {
        pc.printf("\r\n--- Error, derive key failed with error code %d ---\r\n", ret);
        return -1;
    }

    pc.printf("--- 32 byte derived key is: \r\n");
    print_buffer(derive_key2, DEVICE_KEY_32BYTE);
    pc.printf("\r\n");

    pc.printf("\r\n--- Mbed OS DeviceKey example done. ---\r\n");

    return 0;
}
