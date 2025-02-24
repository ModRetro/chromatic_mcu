#include "crc8_sae_j1850.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

enum {
    kPolynomial = 0x1D,
    kInitCRC = 0xFF,
};

static uint8_t crc8_sae_j1850(const uint8_t *pData, const size_t Length) {
    if (pData == NULL)
    {
        return 0;
    }

    uint8_t crc = kInitCRC;

    for (size_t i = 0; i < Length; i++)
    {
        crc ^= pData[i];

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ kPolynomial;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

size_t crc8_sae_j1850_encode(const uint8_t *pMsg, const size_t Length, uint8_t *const pOutput) {
    if ((pMsg == NULL) || (pOutput == NULL) || (Length == 0))
    {
        return 0;
    }

    // Calculate the CRC for the message
    const uint8_t crc = crc8_sae_j1850(pMsg, Length);

    // Append the CRC to the end of the message
    pOutput[Length] = crc;

    // Return new Length with CRC byte added
    return Length + 1;
}

bool crc8_sae_j1850_decode(const uint8_t *const pMsg, const size_t Length) {
    if ((pMsg == NULL) || (Length == 0))
    {
        return false;
    }

    // Calculate CRC for the message, including the appended CRC byte
    const uint8_t crc = crc8_sae_j1850(pMsg, Length);

    // If CRC is 0, the message is valid
    return (crc == 0);
}

#if 0
int main() {
    uint8_t message[] = {0x12, 0x34, 0x56, 0x78};
    const size_t message_length = sizeof(message);

    uint8_t encoded_message[message_length + 1];
    memset(encoded_message, 0x0, sizeof(encoded_message));

    // Encode the message
    size_t encoded_length = encode_message(message, message_length, encoded_message);

    // Print encoded message
    printf("Encoded Message with CRC: ");
    for (size_t i = 0; i < encoded_length; i++)
    {
        printf("%02X ", encoded_message[i]);
    }
    printf("\n");

    // Decode the message and check CRC
    if (decode_message(encoded_message, encoded_length))
    {
        printf("Message is valid.\n");
    }
    else
    {
        printf("Message is invalid.\n");
    }

    return 0;
}
#endif
