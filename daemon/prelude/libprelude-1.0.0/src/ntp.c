/*
 * Contents of this file are taken from NTP v 4 distribution,
 * files tvtots.c and tstotv.c, covered by the license below,
 * and slightly modified where necessary.
 */

/**********************************************************************
* Copyright (c) David L. Mills 1992-2001                              *
*                                                                     *
* Permission to use, copy, modify, and distribute this software and   *
* its documentation for any purpose and without fee is hereby         *
* granted, provided that the above copyright notice appears in all    *
* copies and that both the copyright notice and this permission       *
* notice appear in supporting documentation, and that the name        *
* University of Delaware not be used in advertising or publicity      *
* pertaining to distribution of the software without specific,        *
* written prior permission. The University of Delaware makes no       *
* representations about the suitability this software for any         *
* purpose. It is provided "as is" without express or implied          *
* warranty.                                                           *
*                                                                     *
***********************************************************************/

#include "config.h"

#include <stdio.h>
#include <sys/types.h>

#include "prelude-inttypes.h"
#include "ntp.h"


/*
 * tvtots - tables for converting from Unix struct timeval's to
 *      NTP time stamp format.
 */


/*
 * Tables to calculate time stamp fractions from usecs.  The entries
 * in these tables are offset into using each of the two low order
 * bytes plus the next 4 bits in a usec value (from a struct timeval).
 * These are summed to produce the time stamp fraction.
 *
 * Note that these tables are rounded (not truncated) to the nearest
 * low order bit in the fraction.  The timestamp computed should be
 * +- 1.5 low order bits.
 */

const unsigned long ustotslo[256] = {
  0x00000000, 0x000010c7, 0x0000218e, 0x00003255,
  0x0000431c, 0x000053e3, 0x000064aa, 0x00007571,
  0x00008638, 0x000096ff, 0x0000a7c6, 0x0000b88d,
  0x0000c954, 0x0000da1b, 0x0000eae2, 0x0000fba9,
  0x00010c6f, 0x00011d36, 0x00012dfd, 0x00013ec4,
  0x00014f8b, 0x00016052, 0x00017119, 0x000181e0,
  0x000192a7, 0x0001a36e, 0x0001b435, 0x0001c4fc,
  0x0001d5c3, 0x0001e68a, 0x0001f751, 0x00020818,
  0x000218df, 0x000229a6, 0x00023a6d, 0x00024b34,
  0x00025bfb, 0x00026cc2, 0x00027d89, 0x00028e50,
  0x00029f17, 0x0002afde, 0x0002c0a5, 0x0002d16c,
  0x0002e233, 0x0002f2fa, 0x000303c0, 0x00031487,
  0x0003254e, 0x00033615, 0x000346dc, 0x000357a3,
  0x0003686a, 0x00037931, 0x000389f8, 0x00039abf,
  0x0003ab86, 0x0003bc4d, 0x0003cd14, 0x0003dddb,
  0x0003eea2, 0x0003ff69, 0x00041030, 0x000420f7,
  0x000431be, 0x00044285, 0x0004534c, 0x00046413,
  0x000474da, 0x000485a1, 0x00049668, 0x0004a72f,
  0x0004b7f6, 0x0004c8bd, 0x0004d984, 0x0004ea4b,
  0x0004fb12, 0x00050bd8, 0x00051c9f, 0x00052d66,
  0x00053e2d, 0x00054ef4, 0x00055fbb, 0x00057082,
  0x00058149, 0x00059210, 0x0005a2d7, 0x0005b39e,
  0x0005c465, 0x0005d52c, 0x0005e5f3, 0x0005f6ba,
  0x00060781, 0x00061848, 0x0006290f, 0x000639d6,
  0x00064a9d, 0x00065b64, 0x00066c2b, 0x00067cf2,
  0x00068db9, 0x00069e80, 0x0006af47, 0x0006c00e,
  0x0006d0d5, 0x0006e19c, 0x0006f263, 0x00070329,
  0x000713f0, 0x000724b7, 0x0007357e, 0x00074645,
  0x0007570c, 0x000767d3, 0x0007789a, 0x00078961,
  0x00079a28, 0x0007aaef, 0x0007bbb6, 0x0007cc7d,
  0x0007dd44, 0x0007ee0b, 0x0007fed2, 0x00080f99,
  0x00082060, 0x00083127, 0x000841ee, 0x000852b5,
  0x0008637c, 0x00087443, 0x0008850a, 0x000895d1,
  0x0008a698, 0x0008b75f, 0x0008c826, 0x0008d8ed,
  0x0008e9b4, 0x0008fa7b, 0x00090b41, 0x00091c08,
  0x00092ccf, 0x00093d96, 0x00094e5d, 0x00095f24,
  0x00096feb, 0x000980b2, 0x00099179, 0x0009a240,
  0x0009b307, 0x0009c3ce, 0x0009d495, 0x0009e55c,
  0x0009f623, 0x000a06ea, 0x000a17b1, 0x000a2878,
  0x000a393f, 0x000a4a06, 0x000a5acd, 0x000a6b94,
  0x000a7c5b, 0x000a8d22, 0x000a9de9, 0x000aaeb0,
  0x000abf77, 0x000ad03e, 0x000ae105, 0x000af1cc,
  0x000b0292, 0x000b1359, 0x000b2420, 0x000b34e7,
  0x000b45ae, 0x000b5675, 0x000b673c, 0x000b7803,
  0x000b88ca, 0x000b9991, 0x000baa58, 0x000bbb1f,
  0x000bcbe6, 0x000bdcad, 0x000bed74, 0x000bfe3b,
  0x000c0f02, 0x000c1fc9, 0x000c3090, 0x000c4157,
  0x000c521e, 0x000c62e5, 0x000c73ac, 0x000c8473,
  0x000c953a, 0x000ca601, 0x000cb6c8, 0x000cc78f,
  0x000cd856, 0x000ce91d, 0x000cf9e4, 0x000d0aaa,
  0x000d1b71, 0x000d2c38, 0x000d3cff, 0x000d4dc6,
  0x000d5e8d, 0x000d6f54, 0x000d801b, 0x000d90e2,
  0x000da1a9, 0x000db270, 0x000dc337, 0x000dd3fe,
  0x000de4c5, 0x000df58c, 0x000e0653, 0x000e171a,
  0x000e27e1, 0x000e38a8, 0x000e496f, 0x000e5a36,
  0x000e6afd, 0x000e7bc4, 0x000e8c8b, 0x000e9d52,
  0x000eae19, 0x000ebee0, 0x000ecfa7, 0x000ee06e,
  0x000ef135, 0x000f01fb, 0x000f12c2, 0x000f2389,
  0x000f3450, 0x000f4517, 0x000f55de, 0x000f66a5,
  0x000f776c, 0x000f8833, 0x000f98fa, 0x000fa9c1,
  0x000fba88, 0x000fcb4f, 0x000fdc16, 0x000fecdd,
  0x000ffda4, 0x00100e6b, 0x00101f32, 0x00102ff9,
  0x001040c0, 0x00105187, 0x0010624e, 0x00107315,
  0x001083dc, 0x001094a3, 0x0010a56a, 0x0010b631,
};

const unsigned long ustotsmid[256] = {
  0x00000000, 0x0010c6f8, 0x00218def, 0x003254e7,
  0x00431bde, 0x0053e2d6, 0x0064a9ce, 0x007570c5,
  0x008637bd, 0x0096feb4, 0x00a7c5ac, 0x00b88ca4,
  0x00c9539b, 0x00da1a93, 0x00eae18a, 0x00fba882,
  0x010c6f7a, 0x011d3671, 0x012dfd69, 0x013ec460,
  0x014f8b58, 0x01605250, 0x01711947, 0x0181e03f,
  0x0192a736, 0x01a36e2e, 0x01b43526, 0x01c4fc1d,
  0x01d5c315, 0x01e68a0c, 0x01f75104, 0x020817fc,
  0x0218def3, 0x0229a5eb, 0x023a6ce3, 0x024b33da,
  0x025bfad2, 0x026cc1c9, 0x027d88c1, 0x028e4fb9,
  0x029f16b0, 0x02afdda8, 0x02c0a49f, 0x02d16b97,
  0x02e2328f, 0x02f2f986, 0x0303c07e, 0x03148775,
  0x03254e6d, 0x03361565, 0x0346dc5c, 0x0357a354,
  0x03686a4b, 0x03793143, 0x0389f83b, 0x039abf32,
  0x03ab862a, 0x03bc4d21, 0x03cd1419, 0x03dddb11,
  0x03eea208, 0x03ff6900, 0x04102ff7, 0x0420f6ef,
  0x0431bde7, 0x044284de, 0x04534bd6, 0x046412cd,
  0x0474d9c5, 0x0485a0bd, 0x049667b4, 0x04a72eac,
  0x04b7f5a3, 0x04c8bc9b, 0x04d98393, 0x04ea4a8a,
  0x04fb1182, 0x050bd879, 0x051c9f71, 0x052d6669,
  0x053e2d60, 0x054ef458, 0x055fbb4f, 0x05708247,
  0x0581493f, 0x05921036, 0x05a2d72e, 0x05b39e25,
  0x05c4651d, 0x05d52c15, 0x05e5f30c, 0x05f6ba04,
  0x060780fb, 0x061847f3, 0x06290eeb, 0x0639d5e2,
  0x064a9cda, 0x065b63d2, 0x066c2ac9, 0x067cf1c1,
  0x068db8b8, 0x069e7fb0, 0x06af46a8, 0x06c00d9f,
  0x06d0d497, 0x06e19b8e, 0x06f26286, 0x0703297e,
  0x0713f075, 0x0724b76d, 0x07357e64, 0x0746455c,
  0x07570c54, 0x0767d34b, 0x07789a43, 0x0789613a,
  0x079a2832, 0x07aaef2a, 0x07bbb621, 0x07cc7d19,
  0x07dd4410, 0x07ee0b08, 0x07fed200, 0x080f98f7,
  0x08205fef, 0x083126e6, 0x0841edde, 0x0852b4d6,
  0x08637bcd, 0x087442c5, 0x088509bc, 0x0895d0b4,
  0x08a697ac, 0x08b75ea3, 0x08c8259b, 0x08d8ec92,
  0x08e9b38a, 0x08fa7a82, 0x090b4179, 0x091c0871,
  0x092ccf68, 0x093d9660, 0x094e5d58, 0x095f244f,
  0x096feb47, 0x0980b23e, 0x09917936, 0x09a2402e,
  0x09b30725, 0x09c3ce1d, 0x09d49514, 0x09e55c0c,
  0x09f62304, 0x0a06e9fb, 0x0a17b0f3, 0x0a2877ea,
  0x0a393ee2, 0x0a4a05da, 0x0a5accd1, 0x0a6b93c9,
  0x0a7c5ac1, 0x0a8d21b8, 0x0a9de8b0, 0x0aaeafa7,
  0x0abf769f, 0x0ad03d97, 0x0ae1048e, 0x0af1cb86,
  0x0b02927d, 0x0b135975, 0x0b24206d, 0x0b34e764,
  0x0b45ae5c, 0x0b567553, 0x0b673c4b, 0x0b780343,
  0x0b88ca3a, 0x0b999132, 0x0baa5829, 0x0bbb1f21,
  0x0bcbe619, 0x0bdcad10, 0x0bed7408, 0x0bfe3aff,
  0x0c0f01f7, 0x0c1fc8ef, 0x0c308fe6, 0x0c4156de,
  0x0c521dd5, 0x0c62e4cd, 0x0c73abc5, 0x0c8472bc,
  0x0c9539b4, 0x0ca600ab, 0x0cb6c7a3, 0x0cc78e9b,
  0x0cd85592, 0x0ce91c8a, 0x0cf9e381, 0x0d0aaa79,
  0x0d1b7171, 0x0d2c3868, 0x0d3cff60, 0x0d4dc657,
  0x0d5e8d4f, 0x0d6f5447, 0x0d801b3e, 0x0d90e236,
  0x0da1a92d, 0x0db27025, 0x0dc3371d, 0x0dd3fe14,
  0x0de4c50c, 0x0df58c03, 0x0e0652fb, 0x0e1719f3,
  0x0e27e0ea, 0x0e38a7e2, 0x0e496ed9, 0x0e5a35d1,
  0x0e6afcc9, 0x0e7bc3c0, 0x0e8c8ab8, 0x0e9d51b0,
  0x0eae18a7, 0x0ebedf9f, 0x0ecfa696, 0x0ee06d8e,
  0x0ef13486, 0x0f01fb7d, 0x0f12c275, 0x0f23896c,
  0x0f345064, 0x0f45175c, 0x0f55de53, 0x0f66a54b,
  0x0f776c42, 0x0f88333a, 0x0f98fa32, 0x0fa9c129,
  0x0fba8821, 0x0fcb4f18, 0x0fdc1610, 0x0fecdd08,
  0x0ffda3ff, 0x100e6af7, 0x101f31ee, 0x102ff8e6,
  0x1040bfde, 0x105186d5, 0x10624dcd, 0x107314c4,
  0x1083dbbc, 0x1094a2b4, 0x10a569ab, 0x10b630a3,
};

const unsigned long ustotshi[16] = {
  0x00000000, 0x10c6f79a, 0x218def35, 0x3254e6cf,
  0x431bde6a, 0x53e2d604, 0x64a9cd9f, 0x7570c539,
  0x8637bcd3, 0x96feb46e, 0xa7c5ac08, 0xb88ca3a3,
  0xc9539b3d, 0xda1a92d7, 0xeae18a72, 0xfba8820c,
};


/*
 * tstotv - tables for converting from NTP time stamps to struct timeval
 */

/*
 * Tables to convert from a time stamp fraction to usecs.  Note that
 * the units of these tables are actually (usec<<3).  We carry three
 * guard bits so that the result can be properly truncated (or rounded)
 * to be correct to the least significant bit.
 *
 * These tables are rounded.
 */

const long tstoushi[256] = {
  0x000000, 0x007a12, 0x00f424, 0x016e36,
  0x01e848, 0x02625a, 0x02dc6c, 0x03567e,
  0x03d090, 0x044aa2, 0x04c4b4, 0x053ec6,
  0x05b8d8, 0x0632ea, 0x06acfc, 0x07270e,
  0x07a120, 0x081b32, 0x089544, 0x090f56,
  0x098968, 0x0a037a, 0x0a7d8c, 0x0af79e,
  0x0b71b0, 0x0bebc2, 0x0c65d4, 0x0cdfe6,
  0x0d59f8, 0x0dd40a, 0x0e4e1c, 0x0ec82e,
  0x0f4240, 0x0fbc52, 0x103664, 0x10b076,
  0x112a88, 0x11a49a, 0x121eac, 0x1298be,
  0x1312d0, 0x138ce2, 0x1406f4, 0x148106,
  0x14fb18, 0x15752a, 0x15ef3c, 0x16694e,
  0x16e360, 0x175d72, 0x17d784, 0x185196,
  0x18cba8, 0x1945ba, 0x19bfcc, 0x1a39de,
  0x1ab3f0, 0x1b2e02, 0x1ba814, 0x1c2226,
  0x1c9c38, 0x1d164a, 0x1d905c, 0x1e0a6e,
  0x1e8480, 0x1efe92, 0x1f78a4, 0x1ff2b6,
  0x206cc8, 0x20e6da, 0x2160ec, 0x21dafe,
  0x225510, 0x22cf22, 0x234934, 0x23c346,
  0x243d58, 0x24b76a, 0x25317c, 0x25ab8e,
  0x2625a0, 0x269fb2, 0x2719c4, 0x2793d6,
  0x280de8, 0x2887fa, 0x29020c, 0x297c1e,
  0x29f630, 0x2a7042, 0x2aea54, 0x2b6466,
  0x2bde78, 0x2c588a, 0x2cd29c, 0x2d4cae,
  0x2dc6c0, 0x2e40d2, 0x2ebae4, 0x2f34f6,
  0x2faf08, 0x30291a, 0x30a32c, 0x311d3e,
  0x319750, 0x321162, 0x328b74, 0x330586,
  0x337f98, 0x33f9aa, 0x3473bc, 0x34edce,
  0x3567e0, 0x35e1f2, 0x365c04, 0x36d616,
  0x375028, 0x37ca3a, 0x38444c, 0x38be5e,
  0x393870, 0x39b282, 0x3a2c94, 0x3aa6a6,
  0x3b20b8, 0x3b9aca, 0x3c14dc, 0x3c8eee,
  0x3d0900, 0x3d8312, 0x3dfd24, 0x3e7736,
  0x3ef148, 0x3f6b5a, 0x3fe56c, 0x405f7e,
  0x40d990, 0x4153a2, 0x41cdb4, 0x4247c6,
  0x42c1d8, 0x433bea, 0x43b5fc, 0x44300e,
  0x44aa20, 0x452432, 0x459e44, 0x461856,
  0x469268, 0x470c7a, 0x47868c, 0x48009e,
  0x487ab0, 0x48f4c2, 0x496ed4, 0x49e8e6,
  0x4a62f8, 0x4add0a, 0x4b571c, 0x4bd12e,
  0x4c4b40, 0x4cc552, 0x4d3f64, 0x4db976,
  0x4e3388, 0x4ead9a, 0x4f27ac, 0x4fa1be,
  0x501bd0, 0x5095e2, 0x510ff4, 0x518a06,
  0x520418, 0x527e2a, 0x52f83c, 0x53724e,
  0x53ec60, 0x546672, 0x54e084, 0x555a96,
  0x55d4a8, 0x564eba, 0x56c8cc, 0x5742de,
  0x57bcf0, 0x583702, 0x58b114, 0x592b26,
  0x59a538, 0x5a1f4a, 0x5a995c, 0x5b136e,
  0x5b8d80, 0x5c0792, 0x5c81a4, 0x5cfbb6,
  0x5d75c8, 0x5defda, 0x5e69ec, 0x5ee3fe,
  0x5f5e10, 0x5fd822, 0x605234, 0x60cc46,
  0x614658, 0x61c06a, 0x623a7c, 0x62b48e,
  0x632ea0, 0x63a8b2, 0x6422c4, 0x649cd6,
  0x6516e8, 0x6590fa, 0x660b0c, 0x66851e,
  0x66ff30, 0x677942, 0x67f354, 0x686d66,
  0x68e778, 0x69618a, 0x69db9c, 0x6a55ae,
  0x6acfc0, 0x6b49d2, 0x6bc3e4, 0x6c3df6,
  0x6cb808, 0x6d321a, 0x6dac2c, 0x6e263e,
  0x6ea050, 0x6f1a62, 0x6f9474, 0x700e86,
  0x708898, 0x7102aa, 0x717cbc, 0x71f6ce,
  0x7270e0, 0x72eaf2, 0x736504, 0x73df16,
  0x745928, 0x74d33a, 0x754d4c, 0x75c75e,
  0x764170, 0x76bb82, 0x773594, 0x77afa6,
  0x7829b8, 0x78a3ca, 0x791ddc, 0x7997ee
};

const long tstousmid[256] = {
  0x0000, 0x007a, 0x00f4, 0x016e, 0x01e8, 0x0262, 0x02dc, 0x0356,
  0x03d1, 0x044b, 0x04c5, 0x053f, 0x05b9, 0x0633, 0x06ad, 0x0727,
  0x07a1, 0x081b, 0x0895, 0x090f, 0x0989, 0x0a03, 0x0a7e, 0x0af8,
  0x0b72, 0x0bec, 0x0c66, 0x0ce0, 0x0d5a, 0x0dd4, 0x0e4e, 0x0ec8,
  0x0f42, 0x0fbc, 0x1036, 0x10b0, 0x112b, 0x11a5, 0x121f, 0x1299,
  0x1313, 0x138d, 0x1407, 0x1481, 0x14fb, 0x1575, 0x15ef, 0x1669,
  0x16e3, 0x175d, 0x17d8, 0x1852, 0x18cc, 0x1946, 0x19c0, 0x1a3a,
  0x1ab4, 0x1b2e, 0x1ba8, 0x1c22, 0x1c9c, 0x1d16, 0x1d90, 0x1e0a,
  0x1e84, 0x1eff, 0x1f79, 0x1ff3, 0x206d, 0x20e7, 0x2161, 0x21db,
  0x2255, 0x22cf, 0x2349, 0x23c3, 0x243d, 0x24b7, 0x2531, 0x25ac,
  0x2626, 0x26a0, 0x271a, 0x2794, 0x280e, 0x2888, 0x2902, 0x297c,
  0x29f6, 0x2a70, 0x2aea, 0x2b64, 0x2bde, 0x2c59, 0x2cd3, 0x2d4d,
  0x2dc7, 0x2e41, 0x2ebb, 0x2f35, 0x2faf, 0x3029, 0x30a3, 0x311d,
  0x3197, 0x3211, 0x328b, 0x3306, 0x3380, 0x33fa, 0x3474, 0x34ee,
  0x3568, 0x35e2, 0x365c, 0x36d6, 0x3750, 0x37ca, 0x3844, 0x38be,
  0x3938, 0x39b3, 0x3a2d, 0x3aa7, 0x3b21, 0x3b9b, 0x3c15, 0x3c8f,
  0x3d09, 0x3d83, 0x3dfd, 0x3e77, 0x3ef1, 0x3f6b, 0x3fe5, 0x405f,
  0x40da, 0x4154, 0x41ce, 0x4248, 0x42c2, 0x433c, 0x43b6, 0x4430,
  0x44aa, 0x4524, 0x459e, 0x4618, 0x4692, 0x470c, 0x4787, 0x4801,
  0x487b, 0x48f5, 0x496f, 0x49e9, 0x4a63, 0x4add, 0x4b57, 0x4bd1,
  0x4c4b, 0x4cc5, 0x4d3f, 0x4db9, 0x4e34, 0x4eae, 0x4f28, 0x4fa2,
  0x501c, 0x5096, 0x5110, 0x518a, 0x5204, 0x527e, 0x52f8, 0x5372,
  0x53ec, 0x5466, 0x54e1, 0x555b, 0x55d5, 0x564f, 0x56c9, 0x5743,
  0x57bd, 0x5837, 0x58b1, 0x592b, 0x59a5, 0x5a1f, 0x5a99, 0x5b13,
  0x5b8d, 0x5c08, 0x5c82, 0x5cfc, 0x5d76, 0x5df0, 0x5e6a, 0x5ee4,
  0x5f5e, 0x5fd8, 0x6052, 0x60cc, 0x6146, 0x61c0, 0x623a, 0x62b5,
  0x632f, 0x63a9, 0x6423, 0x649d, 0x6517, 0x6591, 0x660b, 0x6685,
  0x66ff, 0x6779, 0x67f3, 0x686d, 0x68e7, 0x6962, 0x69dc, 0x6a56,
  0x6ad0, 0x6b4a, 0x6bc4, 0x6c3e, 0x6cb8, 0x6d32, 0x6dac, 0x6e26,
  0x6ea0, 0x6f1a, 0x6f94, 0x700f, 0x7089, 0x7103, 0x717d, 0x71f7,
  0x7271, 0x72eb, 0x7365, 0x73df, 0x7459, 0x74d3, 0x754d, 0x75c7,
  0x7641, 0x76bc, 0x7736, 0x77b0, 0x782a, 0x78a4, 0x791e, 0x7998
};

const long tstouslo[128] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
  0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
  0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
  0x1f, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
  0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d,
  0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x33, 0x34,
  0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c,
  0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44,
  0x45, 0x46, 0x47, 0x48, 0x48, 0x49, 0x4a, 0x4b,
  0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53,
  0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b,
  0x5c, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62,
  0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
  0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x71,
  0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79
};
