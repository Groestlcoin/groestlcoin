#ifndef BITCOIN_CHAINPARAMSSEEDS_H
#define BITCOIN_CHAINPARAMSSEEDS_H
/**
 * List of fixed seed nodes for the groestlcoin network
 * AUTOGENERATED by contrib/seeds/generate-seeds.py
 *
 * Each line contains a BIP155 serialized (networkID, addr, port) tuple.
 */
static const uint8_t chainparams_seed_main[] = {
    0x01,0x04,0x52,0xc4,0x01,0x47,0x05,0x33,
    0x01,0x04,0x25,0x8b,0x13,0x83,0x05,0x33,
    0x01,0x04,0x92,0xb9,0x8e,0x29,0x05,0x33,
    0x01,0x04,0x92,0xb9,0x8e,0x31,0x05,0x33,
};

static const uint8_t chainparams_seed_signet[] = {
    0x01,0x04,0xc6,0xc7,0x69,0x2b,0x7a,0x63,
    0x02,0x10,0x26,0x04,0xa8,0x80,0x00,0x01,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x96,0x60,0x01,0x7a,0x63,
    0x04,0x20,0xa0,0x58,0x61,0x4c,0xfa,0x69,0x8a,0x1f,0xc0,0x0f,0xc4,0x54,0xf6,0xb7,0x68,0xf3,0xf4,0x36,0x53,0xfe,0xcd,0x47,0xb3,0x96,0x5a,0x65,0xb7,0x4e,0x4f,0xbc,0xae,0xa0,0x7a,0x63,
};

static const uint8_t chainparams_seed_test[] = {
    0x01,0x04,0x68,0xec,0xb2,0xf5,0x45,0x71,
    0x01,0x04,0xc6,0xc7,0x69,0x2b,0x45,0x71,
    0x01,0x04,0x2d,0x20,0xeb,0x47,0x45,0x71,
    0x01,0x04,0x2d,0x20,0xec,0x80,0x45,0x71,
    0x01,0x04,0x5f,0xb3,0x9c,0x73,0x45,0x71,
    0x01,0x04,0x6c,0x3d,0x63,0xa9,0x45,0x71,
    0x01,0x04,0x4e,0x8d,0xdc,0xdf,0x45,0x71,
    0x01,0x04,0x5f,0xb3,0x8c,0x27,0x45,0x71,
    0x01,0x04,0xd1,0xfa,0xea,0x5b,0x45,0x71,
    0x04,0x20,0xb4,0xa3,0x0f,0x5e,0xb6,0xa1,0x37,0xc8,0xde,0x95,0x3a,0xe7,0xed,0x28,0xae,0x23,0xa9,0x2e,0xcf,0x14,0x72,0x80,0x1c,0xb6,0x0b,0x9f,0xe0,0xa6,0xc7,0x69,0xbb,0x93,0x45,0x71,
    0x04,0x20,0x50,0xc3,0x87,0xca,0x83,0x17,0x68,0x1a,0xf3,0x96,0x13,0xfb,0x57,0x4c,0x89,0x22,0xdc,0x22,0x71,0x36,0x70,0xc6,0x28,0x03,0xa0,0xe7,0xaf,0x0f,0x96,0xe1,0xa1,0x5b,0x45,0x71,
    0x04,0x20,0x4b,0x86,0xe6,0xfe,0x45,0x20,0x72,0x54,0x90,0xc7,0x61,0x46,0x17,0xf8,0xcb,0xb4,0x27,0x99,0x49,0x25,0x6e,0x19,0x03,0xaa,0xe4,0xbf,0x8c,0x4a,0xac,0xa9,0xfd,0xd8,0x45,0x71,
};

// TODO
static const uint8_t chainparams_seed_testnet4[] = {
    0x01,0x04,0x12,0xbd,0x9c,0x66,0xbc,0xcd,
    0x01,0x04,0x12,0xc9,0xcf,0x37,0xbc,0xcd,
    0x01,0x04,0x33,0x9e,0xf8,0x08,0xbc,0xcd,
    0x01,0x04,0x39,0x80,0xb0,0xa3,0xbc,0xcd,
    0x01,0x04,0x52,0x43,0x66,0x0f,0xbc,0xcd,
    0x01,0x04,0x58,0x63,0xf8,0x32,0xbc,0xcd,
    0x01,0x04,0x5f,0xd9,0x49,0xa2,0xbc,0xcd,
    0x01,0x04,0x67,0x63,0xab,0xd4,0xbc,0xcd,
    0x01,0x04,0x67,0xa5,0xc0,0xd2,0xbc,0xcd,
};
#endif // BITCOIN_CHAINPARAMSSEEDS_H
