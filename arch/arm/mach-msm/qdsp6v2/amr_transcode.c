/************************************************************************************************
**
**    AUDIO MVS INTERFACE FOR SKY
**
**    FILE
**        amr_trans.c
**
**    DESCRIPTION
**        This file contains mvs service for SKY.
**          
**    Copyright (c) 2010 by PANTECH Incorporated.  All Rights Reserved.
*************************************************************************************************/


/************************************************************************************************
** Includes
*************************************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>

#include "amr_transcode.h"

/* IF1 <-> IF2 transcode*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           LOCAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/* This table maps octets to what they become when bit-reversed. */
static const uint8 BIT_REVERSE [] = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
    0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
    0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
    0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
    0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
    0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
    0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
    0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
    0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
    0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
    0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
    0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
    0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
    0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
    0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
    0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
    0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

/* <EJECT> */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================

FUNCTION GET_PACKET_SIZE

DESCRIPTION
  This function determines the total number of octets needed to store an AMR
  packet of the given frame type in the IF1 (RFC 3267 variety) and IF2
  formats.

PARAMETERS
  frame_type:  This is the 4-bit frame type field from the AMR packet.
  if1:  This points to a variable in which to store the number of octets
        needed to store an AMR packet with the given frame type in IF1 (RFC
        3267 variety) format.
  if2:  This points to a variable in which to store the number of octets
        needed to store an AMR packet with the given frame type in IF2
        format.

DEPENDENCIES
  FEATURE_QVPHONE is defined.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void get_packet_size
(
  uint8  frame_type,
  uint16    *if1,
  uint16    *if2
)
{
    switch (frame_type)
    {
    case 0: /* AMR 4.75 kbps */
        *if1 = *if2 = 13;
        break;
    case 1: /* AMR 5.15 kbps */
        *if1 = *if2 = 14;
        break;
    case 2: /* AMR 5.90 kbps */
        *if1 = *if2 = 16;
        break;
    case 3: /* AMR 6.70 kbps (PDC-EFR) */
        *if1 = *if2 = 18;
        break;
    case 4: /* AMR 7.40 kbps (TDMA-EFR) */
        *if1 = 20, *if2 = 19;
        break;
    case 5: /* AMR 7.95 kbps */
        *if1 = *if2 = 21;
        break;
    case 6: /* AMR 10.2 kbps */
        *if1 = 27, *if2 = 26;
        break;
    case 7: /* AMR 12.2 kbps (GSM-EFR) */
        *if1 = 32, *if2 = 31;
        break;
    case 8: /* AMR SID */
        *if1 = *if2 = 6;
        break;
    case 9: /* GSM-EFR SID */
    case 10: /* TDMA-EFR SID */
    case 11: /* PDC-EFR SID */
    case 12: /* for future use */
    case 13: /* for future use */
    case 14: /* for future use */
    case 15: /* No data (No transmission/No reception) */
        *if1 = *if2 = 1;
        break;
    default:
        *if1 = *if2 = 0;
    }
}

/* <EJECT> */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================

FUNCTION QVP_IF1_TO_IF2

DESCRIPTION
  This function converts AMR vocoder packets from IF1 (RFC 3267 variety) to
  IF2 format.

PARAMETERS
  if1:  This points to a buffer containing AMR vocoder packets in IF1 (RFC
        3267 variety) format to be converted to IF2 format.
  octets_in:  This is the number of octets in the input buffer to be
              converted.
  if2:  This points to a buffer where converted IF2 format AMR vocoder
        packets are to be stored.
  max_octets_out:  This is the maximum number of octets which can be stored
                   in the output buffer pointed to by if2.
  octet_count:  If not NULL, this points to a variable in which to store a
                count of the number of octets actually used in the output
                buffer.
  packet_count:  If not NULL, this points to a variable in which to store a
                 count of the number of AMR packets actually converted.

DEPENDENCIES
  FEATURE_QVPHONE is defined.

RETURN VALUE
  The number of octets of the input buffer actually used is returned.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 qvp_if1_to_if2
(
  const void  *if1,
  uint16      octets_in,
  void        *if2,
  uint16      max_octets_out,
  uint16      *octet_count,
  uint32      *packet_count
)
{
    uint16 if1_size, if2_size;
    uint16 if1_index, if2_index;
    uint16 octet;
    uint8 frame_type;
    const uint8 *if1_octets;
    uint8 *if2_octets;
    uint16 input_count = 0;

    /* Type-cast frame buffer pointers to usable types. */
    if1_octets = (const uint8 *) if1;
    if2_octets = (uint8 *) if2;

    /* Zero out octet and packet counts, if given. */
    if (octet_count)
    {
        *octet_count = 0;
    }
    if (packet_count)
    {
        *packet_count = 0;
    }

    /* Repeat until we run out of input or space for output. */
    if1_index = if2_index = 0;
    while (!((if1_index >= octets_in) && (if2_index >= max_octets_out)))
    {
        /* Extract next frame's frame type, and determine packet sizes
        ** for IF1 and IF2.
        */
        frame_type = (if1_octets [if1_index] >> 3) & 15;
        get_packet_size (frame_type, &if1_size, &if2_size);

        /* Break out if not enough data available from input to get whole
        ** frame, or not enough space left in output for whole frame.
        */
        if ((if1_index + if1_size > octets_in)
            || (if2_index + if2_size > max_octets_out))
        {
            break;
        }

        /* Store frame type, along with first four vocoder bits, in first IF2
        ** packet octet.
        */
        if2_octets [if2_index] = frame_type;
        if (if1_size > 1)
        {
            if2_octets [if2_index]
                |= (uint8)(BIT_REVERSE [if1_octets [if1_index + 1]] << 4);
        }

        /* Bit-reverse and store octets from into IF2 from IF1, starting at
        ** second octet in IF2 (corresponds to low-order nibble in second IF1
        ** octet plus high-order nibble from third IF1 octet).
        */
        for (octet = 1; octet < if2_size; ++octet)
        {
            if2_octets [if2_index + octet]
                = BIT_REVERSE [if1_octets [if1_index + octet]] >> 4;
            if (octet + 1 < if1_size)
            {
                if2_octets [if2_index + octet]
                    |= (uint8)(BIT_REVERSE [if1_octets [if1_index + octet + 1]] << 4);
            }
        }

        /* Count octets converted, and advance pointers. */
        if (octet_count)
        {
            *octet_count += if2_size;
        }
        if (packet_count)
        {
            ++(*packet_count);
        }
        input_count += if1_size;
        if1_index += if1_size;
        if2_index += if2_size;
    }

    /* Return number of octets actually used from input. */
    return input_count;
}

/* <EJECT> */
/*===========================================================================

FUNCTION QVP_IF2_TO_IF1

DESCRIPTION
  This function converts AMR vocoder packets from IF2 to IF1 (RFC 3267
  variety) format.

PARAMETERS
  if2:  This points to a buffer containing AMR vocoder packets in IF2 format
        to be converted to IF2 format.
  octets_in:  This is the number of octets in the input buffer to be
              converted.
  if1:  This points to a buffer where converted IF1 (RFC 3267 variety)
        format AMR vocoder packets are to be stored.
  max_octets_out:  This is the maximum number of octets which can be stored
                   in the output buffer pointed to by if1.
  octet_count:  If not NULL, this points to a variable in which to store a
                count of the number of octets actually used in the output
                buffer.
  packet_count:  If not NULL, this points to a variable in which to store a
                 count of the number of AMR packets actually converted.

DEPENDENCIES
  FEATURE_QVPHONE is defined.

RETURN VALUE
  The number of octets of the input buffer actually used is returned.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 qvp_if2_to_if1
(
  const void  *if2,
  uint16      octets_in,
  void        *if1,
  uint16      max_octets_out,
  uint16      *octet_count,
  uint32      *packet_count
)
{
    uint16 if1_size, if2_size;
    uint16 if1_index, if2_index;
    uint16 octet;
    uint8 frame_type;
    uint8 *if1_octets;
    const uint8 *if2_octets;
    uint16 input_count = 0;

    /* Type-cast frame buffer pointers to usable types. */
    if1_octets = (uint8 *) if1;
    if2_octets = (const uint8 *) if2;

    /* Zero out octet and packet counts, if given. */
    if (octet_count)
    {
        *octet_count = 0;
    }
    if (packet_count)
    {
        *packet_count = 0;
    }

    /* Repeat until we run out of input or space for output. */
    if1_index = if2_index = 0;
    while (!((if2_index >= octets_in) && (if1_index >= max_octets_out)))
    {
        /* Extract next frame's frame type, and determine packet sizes
        ** for IF1 and IF2.
        */
        frame_type = if2_octets [if2_index] & 15;
        get_packet_size (frame_type, &if1_size, &if2_size);

        /*add some error checking to account for over the air data
          corruption
        */
        if (frame_type > 8)
		{
          if1_octets [0] = 0x7C;
	      if (octet_count)
		  {
          *octet_count = 1;
		  }
          input_count = 1;
		  break;
		}
        /* Break out if not enough data available from input to get whole
        ** frame, or not enough space left in output for whole frame.
        */
        if ((if2_index + if2_size > octets_in)
            || (if1_index + if1_size > max_octets_out))
        {
            break;
        }

        /* Store frame type, along with "good" quality indicator, in
        ** first IF1 packet octet.
        */
        if1_octets [if1_index] = (uint8) ((frame_type << 3) | 4);

        /* Bit-reverse and store octets from into IF1 from IF2, starting at
        ** second octet in IF1 (corresponds to high-order nibble in first IF2
        ** octet plus low-order nibble from second IF2 octet).
        */
        for (octet = 1; octet < if1_size; ++octet)
        {
            if1_octets [if1_index + octet]
                = (uint8) (BIT_REVERSE [if2_octets [if2_index + octet - 1]] << 4);
            if (octet < if2_size)
            {
                if1_octets [if1_index + octet]
                    |= BIT_REVERSE [if2_octets [if2_index + octet]] >> 4;
            }
        }

        input_count += if2_size;
        if1_index += if1_size;
        if2_index += if2_size;

        /* Count octets converted, and advance pointers. */
        if (octet_count)
        {
            *octet_count += if1_size;
        }
        if (packet_count)
        {
            ++(*packet_count);
        }
        else
        {
            /* breaking the loop; we expect only one packet right now */
          break;
        }
    }

    /* Return number of octets actually used from input. */
    return input_count;
}
