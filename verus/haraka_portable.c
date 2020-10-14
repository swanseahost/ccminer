/*
Plain C implementation of the Haraka256 and Haraka512 permutations.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "haraka_portable.h"

#define HARAKAS_RATE 32

static const unsigned char haraka_rc[40][16] = {
    {0x9d, 0x7b, 0x81, 0x75, 0xf0, 0xfe, 0xc5, 0xb2, 0x0a, 0xc0, 0x20, 0xe6, 0x4c, 0x70, 0x84, 0x06},
    {0x17, 0xf7, 0x08, 0x2f, 0xa4, 0x6b, 0x0f, 0x64, 0x6b, 0xa0, 0xf3, 0x88, 0xe1, 0xb4, 0x66, 0x8b},
    {0x14, 0x91, 0x02, 0x9f, 0x60, 0x9d, 0x02, 0xcf, 0x98, 0x84, 0xf2, 0x53, 0x2d, 0xde, 0x02, 0x34},
    {0x79, 0x4f, 0x5b, 0xfd, 0xaf, 0xbc, 0xf3, 0xbb, 0x08, 0x4f, 0x7b, 0x2e, 0xe6, 0xea, 0xd6, 0x0e},
    {0x44, 0x70, 0x39, 0xbe, 0x1c, 0xcd, 0xee, 0x79, 0x8b, 0x44, 0x72, 0x48, 0xcb, 0xb0, 0xcf, 0xcb},
    {0x7b, 0x05, 0x8a, 0x2b, 0xed, 0x35, 0x53, 0x8d, 0xb7, 0x32, 0x90, 0x6e, 0xee, 0xcd, 0xea, 0x7e},
    {0x1b, 0xef, 0x4f, 0xda, 0x61, 0x27, 0x41, 0xe2, 0xd0, 0x7c, 0x2e, 0x5e, 0x43, 0x8f, 0xc2, 0x67},
    {0x3b, 0x0b, 0xc7, 0x1f, 0xe2, 0xfd, 0x5f, 0x67, 0x07, 0xcc, 0xca, 0xaf, 0xb0, 0xd9, 0x24, 0x29},
    {0xee, 0x65, 0xd4, 0xb9, 0xca, 0x8f, 0xdb, 0xec, 0xe9, 0x7f, 0x86, 0xe6, 0xf1, 0x63, 0x4d, 0xab},
    {0x33, 0x7e, 0x03, 0xad, 0x4f, 0x40, 0x2a, 0x5b, 0x64, 0xcd, 0xb7, 0xd4, 0x84, 0xbf, 0x30, 0x1c},
    {0x00, 0x98, 0xf6, 0x8d, 0x2e, 0x8b, 0x02, 0x69, 0xbf, 0x23, 0x17, 0x94, 0xb9, 0x0b, 0xcc, 0xb2},
    {0x8a, 0x2d, 0x9d, 0x5c, 0xc8, 0x9e, 0xaa, 0x4a, 0x72, 0x55, 0x6f, 0xde, 0xa6, 0x78, 0x04, 0xfa},
    {0xd4, 0x9f, 0x12, 0x29, 0x2e, 0x4f, 0xfa, 0x0e, 0x12, 0x2a, 0x77, 0x6b, 0x2b, 0x9f, 0xb4, 0xdf},
    {0xee, 0x12, 0x6a, 0xbb, 0xae, 0x11, 0xd6, 0x32, 0x36, 0xa2, 0x49, 0xf4, 0x44, 0x03, 0xa1, 0x1e},
    {0xa6, 0xec, 0xa8, 0x9c, 0xc9, 0x00, 0x96, 0x5f, 0x84, 0x00, 0x05, 0x4b, 0x88, 0x49, 0x04, 0xaf},
    {0xec, 0x93, 0xe5, 0x27, 0xe3, 0xc7, 0xa2, 0x78, 0x4f, 0x9c, 0x19, 0x9d, 0xd8, 0x5e, 0x02, 0x21},
    {0x73, 0x01, 0xd4, 0x82, 0xcd, 0x2e, 0x28, 0xb9, 0xb7, 0xc9, 0x59, 0xa7, 0xf8, 0xaa, 0x3a, 0xbf},
    {0x6b, 0x7d, 0x30, 0x10, 0xd9, 0xef, 0xf2, 0x37, 0x17, 0xb0, 0x86, 0x61, 0x0d, 0x70, 0x60, 0x62},
    {0xc6, 0x9a, 0xfc, 0xf6, 0x53, 0x91, 0xc2, 0x81, 0x43, 0x04, 0x30, 0x21, 0xc2, 0x45, 0xca, 0x5a},
    {0x3a, 0x94, 0xd1, 0x36, 0xe8, 0x92, 0xaf, 0x2c, 0xbb, 0x68, 0x6b, 0x22, 0x3c, 0x97, 0x23, 0x92},
    {0xb4, 0x71, 0x10, 0xe5, 0x58, 0xb9, 0xba, 0x6c, 0xeb, 0x86, 0x58, 0x22, 0x38, 0x92, 0xbf, 0xd3},
    {0x8d, 0x12, 0xe1, 0x24, 0xdd, 0xfd, 0x3d, 0x93, 0x77, 0xc6, 0xf0, 0xae, 0xe5, 0x3c, 0x86, 0xdb},
    {0xb1, 0x12, 0x22, 0xcb, 0xe3, 0x8d, 0xe4, 0x83, 0x9c, 0xa0, 0xeb, 0xff, 0x68, 0x62, 0x60, 0xbb},
    {0x7d, 0xf7, 0x2b, 0xc7, 0x4e, 0x1a, 0xb9, 0x2d, 0x9c, 0xd1, 0xe4, 0xe2, 0xdc, 0xd3, 0x4b, 0x73},
    {0x4e, 0x92, 0xb3, 0x2c, 0xc4, 0x15, 0x14, 0x4b, 0x43, 0x1b, 0x30, 0x61, 0xc3, 0x47, 0xbb, 0x43},
    {0x99, 0x68, 0xeb, 0x16, 0xdd, 0x31, 0xb2, 0x03, 0xf6, 0xef, 0x07, 0xe7, 0xa8, 0x75, 0xa7, 0xdb},
    {0x2c, 0x47, 0xca, 0x7e, 0x02, 0x23, 0x5e, 0x8e, 0x77, 0x59, 0x75, 0x3c, 0x4b, 0x61, 0xf3, 0x6d},
    {0xf9, 0x17, 0x86, 0xb8, 0xb9, 0xe5, 0x1b, 0x6d, 0x77, 0x7d, 0xde, 0xd6, 0x17, 0x5a, 0xa7, 0xcd},
    {0x5d, 0xee, 0x46, 0xa9, 0x9d, 0x06, 0x6c, 0x9d, 0xaa, 0xe9, 0xa8, 0x6b, 0xf0, 0x43, 0x6b, 0xec},
    {0xc1, 0x27, 0xf3, 0x3b, 0x59, 0x11, 0x53, 0xa2, 0x2b, 0x33, 0x57, 0xf9, 0x50, 0x69, 0x1e, 0xcb},
    {0xd9, 0xd0, 0x0e, 0x60, 0x53, 0x03, 0xed, 0xe4, 0x9c, 0x61, 0xda, 0x00, 0x75, 0x0c, 0xee, 0x2c},
    {0x50, 0xa3, 0xa4, 0x63, 0xbc, 0xba, 0xbb, 0x80, 0xab, 0x0c, 0xe9, 0x96, 0xa1, 0xa5, 0xb1, 0xf0},
    {0x39, 0xca, 0x8d, 0x93, 0x30, 0xde, 0x0d, 0xab, 0x88, 0x29, 0x96, 0x5e, 0x02, 0xb1, 0x3d, 0xae},
    {0x42, 0xb4, 0x75, 0x2e, 0xa8, 0xf3, 0x14, 0x88, 0x0b, 0xa4, 0x54, 0xd5, 0x38, 0x8f, 0xbb, 0x17},
    {0xf6, 0x16, 0x0a, 0x36, 0x79, 0xb7, 0xb6, 0xae, 0xd7, 0x7f, 0x42, 0x5f, 0x5b, 0x8a, 0xbb, 0x34},
    {0xde, 0xaf, 0xba, 0xff, 0x18, 0x59, 0xce, 0x43, 0x38, 0x54, 0xe5, 0xcb, 0x41, 0x52, 0xf6, 0x26},
    {0x78, 0xc9, 0x9e, 0x83, 0xf7, 0x9c, 0xca, 0xa2, 0x6a, 0x02, 0xf3, 0xb9, 0x54, 0x9a, 0xe9, 0x4c},
    {0x35, 0x12, 0x90, 0x22, 0x28, 0x6e, 0xc0, 0x40, 0xbe, 0xf7, 0xdf, 0x1b, 0x1a, 0xa5, 0x51, 0xae},
    {0xcf, 0x59, 0xa6, 0x48, 0x0f, 0xbc, 0x73, 0xc1, 0x2b, 0xd2, 0x7e, 0xba, 0x3c, 0x61, 0xc1, 0xa0},
    {0xa1, 0x9d, 0xc5, 0xe9, 0xfd, 0xbd, 0xd6, 0x4a, 0x88, 0x82, 0x28, 0x02, 0x03, 0xcc, 0x6a, 0x75}
};

static unsigned char rc[40][16];
static unsigned char rc0[40][16];
static unsigned char rc_sseed[40][16];

static const unsigned char sbox[256] =
{ 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe,
  0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4,
  0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7,
  0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 0x04, 0xc7, 0x23, 0xc3,
  0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 0x09,
  0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3,
  0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe,
  0x39, 0x4a, 0x4c, 0x58, 0xcf, 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
  0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92,
  0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c,
  0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19,
  0x73, 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14,
  0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2,
  0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5,
  0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 0xba, 0x78, 0x25,
  0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86,
  0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e,
  0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42,
  0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

#define XT(x) (((x) << 1) ^ ((((x) >> 7) & 1) * 0x1b))

// Simulate _mm_aesenc_si128 instructions from AESNI
void aesenc(unsigned char *s, const unsigned char *rk) 
{

  uint8x16_t tmp1, tmp2, tmp3;
// uint8_t s[16] = { i[0], i[1], i[2], i[3], i[4], i[5], i[6], i[7],
//                   i[8], i[9], i[10], i[11], i[12], i[13], i[14], i[15] };

 tmp1 =  vld1q_u8(s);
 tmp2 = vld1q_u8(rk);

 tmp3 = vaesmcq_u8(vaeseq_u8(tmp1, (uint8x16_t)0)) ^ tmp2;
  
  ((uint64_t*)&s[0])[0] = (uint64_t)vgetq_lane_u64(tmp3,0);
  ((uint64_t*)&s[0])[1] = (uint64_t)vgetq_lane_u64(tmp3,1);

  /*  unsigned char i, t, u, v[4][4];
    for (i = 0; i < 16; ++i) {
        v[((i / 4) + 4 - (i%4) ) % 4][i % 4] = sbox[s[i]];
    }
    for (i = 0; i < 4; ++i) {
        t = v[i][0];
        u = v[i][0] ^ v[i][1] ^ v[i][2] ^ v[i][3];
        v[i][0] ^= u ^ XT(v[i][0] ^ v[i][1]);
        v[i][1] ^= u ^ XT(v[i][1] ^ v[i][2]);
        v[i][2] ^= u ^ XT(v[i][2] ^ v[i][3]);
        v[i][3] ^= u ^ XT(v[i][3] ^ t);
    }
    for (i = 0; i < 16; ++i) {
        s[i] = v[i / 4][i % 4] ^ rk[i];
    } */
}

// Simulate _mm_unpacklo_epi32
void unpacklo32(unsigned char *t, unsigned char *a, unsigned char *b) 
{
    unsigned char tmp[16];
    memcpy(tmp, a, 4);
    memcpy(tmp + 4, b, 4);
    memcpy(tmp + 8, a + 4, 4);
    memcpy(tmp + 12, b + 4, 4);
    memcpy(t, tmp, 16);
}

// Simulate _mm_unpackhi_epi32
void unpackhi32(unsigned char *t, unsigned char *a, unsigned char *b) 
{
    unsigned char tmp[16];
    memcpy(tmp, a + 8, 4);
    memcpy(tmp + 4, b + 8, 4);
    memcpy(tmp + 8, a + 12, 4);
    memcpy(tmp + 12, b + 12, 4);
    memcpy(t, tmp, 16);
}

void load_constants_port()
{
    /* Use the standard constants to generate tweaked ones. */
    memcpy(rc, haraka_rc, 40*16);
}

void tweak_constants(const unsigned char *pk_seed, const unsigned char *sk_seed,
                     unsigned long long seed_length)
{
    unsigned char buf[40*16];

    /* Use the standard constants to generate tweaked ones. */
    memcpy(rc, haraka_rc, 40*16);

    /* Constants for sk.seed */
    if (sk_seed != NULL) {
        haraka_S(buf, 40*16, sk_seed, seed_length);
        memcpy(rc_sseed, buf, 40*16);
    }

    /* Constants for pk.seed */
    haraka_S(buf, 40*16, pk_seed, seed_length);
    memcpy(rc, buf, 40*16);    
}

static void haraka_S_absorb(unsigned char *s, 
                            const unsigned char *m, unsigned long long mlen,
                            unsigned char p)
{
    unsigned long long i;

    unsigned char t[2];



    while (mlen >= 32) {
        // XOR block to state
        for (i = 0; i < 32; ++i) {
            s[i] ^= m[i];
        }
        haraka512_perm(s, s);
        mlen -= 32;
        m += 32;
    }

    for (i = 0; i < 32; ++i) {
        t[i] = 0;
    }
    for (i = 0; i < mlen; ++i) {
        t[i] = m[i];
    }
    t[i] = p;
    t[32 - 1] |= 128;
    for (i = 0; i < 32; ++i) {
        s[i] ^= t[i];
    }
}

static void haraka_S_squeezeblocks(unsigned char *h, unsigned long long nblocks,
                                   unsigned char *s, unsigned int r)
{
    while (nblocks > 0) {
        haraka512_perm(s, s);
        memcpy(h, s, HARAKAS_RATE);
        h += r;
        nblocks--;
    }
}


void haraka_S(unsigned char *out, unsigned long long outlen,
              const unsigned char *in, unsigned long long inlen)
{
    unsigned long long i;
    unsigned char s[64];
    unsigned char d[32];

    for (i = 0; i < 64; i++) {
        s[i] = 0;
    }
    haraka_S_absorb(s, in, inlen, 0x1F);

    haraka_S_squeezeblocks(out, outlen / 32, s, 32);
    out += (outlen / 32) * 32;

    if (outlen % 32) {
        haraka_S_squeezeblocks(d, 1, s, 32);
        for (i = 0; i < outlen % 32; i++) {
            out[i] = d[i];
        }
    }
}

void haraka512_perm(unsigned char *out, const unsigned char *in) 
{
    int i, j;

    unsigned char s[64], tmp[16];

    memcpy(s, in, 16);
    memcpy(s + 16, in + 16, 16);
    memcpy(s + 32, in + 32, 16);
    memcpy(s + 48, in + 48, 16);

    for (i = 0; i < 5; ++i) {
        // aes round(s)
        for (j = 0; j < 2; ++j) {
            aesenc(s, rc[4*2*i + 4*j]);
            aesenc(s + 16, rc[4*2*i + 4*j + 1]);
            aesenc(s + 32, rc[4*2*i + 4*j + 2]);
            aesenc(s + 48, rc[4*2*i + 4*j + 3]);
        }

        // mixing
        unpacklo32(tmp, s, s + 16);
        unpackhi32(s, s, s + 16);
        unpacklo32(s + 16, s + 32, s + 48);
        unpackhi32(s + 32, s + 32, s + 48);
        unpacklo32(s + 48, s, s + 32);
        unpackhi32(s, s, s + 32);
        unpackhi32(s + 32, s + 16, tmp);
        unpacklo32(s + 16, s + 16, tmp);
    }

    memcpy(out, s, 64);
}

void haraka512_perm_keyed(unsigned char *out, const unsigned char *in, const __m128i *rc) 
{
    int i, j;

    unsigned char s[64], tmp[16];

    memcpy(s, in, 16);
    memcpy(s + 16, in + 16, 16);
    memcpy(s + 32, in + 32, 16);
    memcpy(s + 48, in + 48, 16);

    for (i = 0; i < 5; ++i) {
        // aes round(s)
        for (j = 0; j < 2; ++j) {
            aesenc(s, (const unsigned char *)&rc[4*2*i + 4*j]);
            aesenc(s + 16, (const unsigned char *)&rc[4*2*i + 4*j + 1]);
            aesenc(s + 32, (const unsigned char *)&rc[4*2*i + 4*j + 2]);
            aesenc(s + 48, (const unsigned char *)&rc[4*2*i + 4*j + 3]);
        }

        // mixing
        unpacklo32(tmp, s, s + 16);
        unpackhi32(s, s, s + 16);
        unpacklo32(s + 16, s + 32, s + 48);
        unpackhi32(s + 32, s + 32, s + 48);
        unpacklo32(s + 48, s, s + 32);
        unpackhi32(s, s, s + 32);
        unpackhi32(s + 32, s + 16, tmp);
        unpacklo32(s + 16, s + 16, tmp);
    }

    memcpy(out, s, 64);
}

void haraka512_port(unsigned char *out, const unsigned char *in)
{
    int i;

    unsigned char buf[64];

    haraka512_perm(buf, in);
    /* Feed-forward */
    for (i = 0; i < 64; i++) {
        buf[i] = buf[i] ^ in[i];
    }

    /* Truncated */
    memcpy(out,      buf + 8, 8);
    memcpy(out + 8,  buf + 24, 8);
    memcpy(out + 16, buf + 32, 8);
    memcpy(out + 24, buf + 48, 8);
}

void haraka512_port_keyed(unsigned char *out, const unsigned char *in, const __m128i *rc)
{
    int i;

    unsigned char buf[64];

    haraka512_perm_keyed(buf, in, rc);
    /* Feed-forward */
    for (i = 0; i < 64; i++) {
        buf[i] = buf[i] ^ in[i];
    }

    /* Truncated */
    memcpy(out,      buf + 8, 8);
    memcpy(out + 8,  buf + 24, 8);
    memcpy(out + 16, buf + 32, 8);
    memcpy(out + 24, buf + 48, 8);
}

void haraka512_perm_zero(unsigned char *out, const unsigned char *in) 
{
    int i, j;

    unsigned char s[64], tmp[16];

    memcpy(s, in, 16);
    memcpy(s + 16, in + 16, 16);
    memcpy(s + 32, in + 32, 16);
    memcpy(s + 48, in + 48, 16);

    for (i = 0; i < 5; ++i) {
        // aes round(s)
        for (j = 0; j < 2; ++j) {
            aesenc(s, rc0[4*2*i + 4*j]);
            aesenc(s + 16, rc0[4*2*i + 4*j + 1]);
            aesenc(s + 32, rc0[4*2*i + 4*j + 2]);
            aesenc(s + 48, rc0[4*2*i + 4*j + 3]);
        }

        // mixing
        unpacklo32(tmp, s, s + 16);
        unpackhi32(s, s, s + 16);
        unpacklo32(s + 16, s + 32, s + 48);
        unpackhi32(s + 32, s + 32, s + 48);
        unpacklo32(s + 48, s, s + 32);
        unpackhi32(s, s, s + 32);
        unpackhi32(s + 32, s + 16, tmp);
        unpacklo32(s + 16, s + 16, tmp);
    }

    memcpy(out, s, 64);
}

void haraka512_port_zero(unsigned char *out, const unsigned char *in)
{
    int i;

    unsigned char buf[64];

    haraka512_perm_zero(buf, in);
    /* Feed-forward */
    for (i = 0; i < 64; i++) {
        buf[i] = buf[i] ^ in[i];
    }

    /* Truncated */
    memcpy(out,      buf + 8, 8);
    memcpy(out + 8,  buf + 24, 8);
    memcpy(out + 16, buf + 32, 8);
    memcpy(out + 24, buf + 48, 8);
}

void haraka256_port(unsigned char *out, const unsigned char *in) 
{
    int i, j;

    unsigned char s[32], tmp[16];

    memcpy(s, in, 16);
    memcpy(s + 16, in + 16, 16);

    for (i = 0; i < 5; ++i) {
        // aes round(s)
        for (j = 0; j < 2; ++j) {
            aesenc(s, rc[2*2*i + 2*j]);
            aesenc(s + 16, rc[2*2*i + 2*j + 1]);
        }

        // mixing
        unpacklo32(tmp, s, s + 16);
        unpackhi32(s + 16, s, s + 16);
        memcpy(s, tmp, 16);
    }

    /* Feed-forward */
    for (i = 0; i < 32; i++) {
        out[i] = in[i] ^ s[i];
    }
}

void haraka256_sk(unsigned char *out, const unsigned char *in)
{
    int i, j;

    unsigned char s[32], tmp[16];

    memcpy(s, in, 16);
    memcpy(s + 16, in + 16, 16);

    for (i = 0; i < 5; ++i) {
        // aes round(s)
        for (j = 0; j < 2; ++j) {
            aesenc(s, rc_sseed[2*2*i + 2*j]);
            aesenc(s + 16, rc_sseed[2*2*i + 2*j + 1]);
        }

        // mixing
        unpacklo32(tmp, s, s + 16);
        unpackhi32(s + 16, s, s + 16);
        memcpy(s, tmp, 16);
    }

    /* Feed-forward */
    for (i = 0; i < 32; i++) {
        out[i] = in[i] ^ s[i];
    }
}
