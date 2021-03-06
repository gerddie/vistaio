/*
 *  Copyright 1993, 1994 University of British Columbia
 *  Copyright (c) Leipzig, Madrid 2004 - 2012
 *  Max-Planck-Institute for Human Cognitive and Brain Science	
 *  Max-Planck-Institute for Evolutionary Anthropology 
 *  BIT, ETSI Telecomunicacion, UPM
 *
 *  Permission to use, copy, modify, distribute, and sell this software and its
 *  documentation for any purpose is hereby granted without fee, provided that
 *  the above copyright notice appears in all copies and that both that
 *  copyright notice and this permission notice appear in supporting
 *  documentation. UBC makes no representations about the suitability of this
 *  software for any purpose. It is provided "as is" without express or
 *  implied warranty.
 *
 *  Author: Arthur Pope, UBC Laboratory for Computational Intelligence
 *  Maintainance and Fixes: Gert Wollny, UPM 
 */

#include "vistaio/vistaio.h"

/* Later in this file: */
static VistaIOPackOrder MachineByteOrder (void);
static void SwapBytes (int64_t, uint64_t, char *);


/*! \brief Convert an array of data elements from unpacked to packed form.
 *
 *  The elements to be packed are integers or floating point numbers, as
 *  specified by repn. Each element's unpacked size is unpacked_elsize
 *  bits, and its packed size is packed_elsize bits. There are nels of
 *  them, beginning at unpacked. Packed_order specifies whether they are
 *  to be packed from MSB to LSB (VistaIOMSBFirst), or vice versa (VistaIOLSBFirst).
 *
 *  To pack into a buffer already allocated:
 *
 *	length = length of buffer;
 *	packed = address of buffer;
 *	VistaIOPackData ( ..., & length, & packed, NULL);
 *
 *  and on return length will be set to the length of the packed data.
 *
 *  To pack into a buffer supplied by VistaIOPackData:
 *
 *	VistaIOPackData ( ..., & length, & packed, & alloced);
 *
 *  and on return length will be set to the length of the packed data,
 *  packed will be set to point to it, and alloced will be TRUE if
 *  the storage packed points to is a buffer that was allocated just for
 *  the packed data. If alloced is FALSE, packed is pointing to the same
 *  place as the unpacked data because the packed and unpacked forms are
 *  identical.
 *
 *  These assumptions are made:
 *    - packed_elsize is either 1 or a multiple of 8
 *    - if packed_elsize is 1, then the unpacked data elements are VistaIOBits
 *    - unpacked_elsize >= packed_elsize
 *
 *  \param  repn
 *  \param  nels
 *  \param  unpacked
 *  \param  packed_order
 *  \param  length
 *  \param  packed
 *  \param  alloced
 *  \return  VistaIOBoolean
 */

VistaIOBoolean VistaIOPackData (VistaIORepnKind repn,
		    int64_t nels, VistaIOPointer unpacked, VistaIOPackOrder packed_order,
		    int64_t * length, VistaIOPointer * packed, VistaIOBoolean * alloced)
{
	VistaIOPackOrder unpacked_order;
	int64_t unpacked_elsize = VistaIORepnSize (repn) * CHAR_BIT;
	int64_t packed_elsize = VistaIORepnPrecision (repn);
	int64_t packed_length = (nels * packed_elsize + 7) / 8;

	/* If space for the packed data was supplied, ensure there's
	   enough of it: */
	if (!alloced && packed_length > *length) {
		VistaIOWarning ("VistaIOPackData: Insufficient space for packed data");
		return FALSE;
	}
	*length = packed_length;

	/* Determine the present machine's internal byte order: */
	unpacked_order = MachineByteOrder ();

	/* If the desired byte order matches that of the present machine's, and
	   the unpacked and packed data element sizes are identical,
	   just return the unpacked data: */
	if (unpacked_order == packed_order
	    && unpacked_elsize == packed_elsize) {
		if (alloced) {
			*packed = unpacked;
			*alloced = FALSE;
		} else if (unpacked != packed)
			memcpy (*packed, unpacked, packed_length);
		return TRUE;
	}

	/* Allocate a buffer for the packed data if none was provided: */
	if (alloced) {
		*packed = VistaIOMalloc (packed_length);
		*alloced = TRUE;
	}

	/* Pack data elements into the buffer: */
	if (unpacked_elsize == packed_elsize) {

		/* If the packed and unpacked are the same size, do a straight copy: */
		if (unpacked != *packed)
			memcpy (*packed, unpacked, packed_length);

		/* Swap bytes if necessary: */
		if (packed_order != unpacked_order && packed_elsize > 8)
			SwapBytes (nels, packed_elsize / 8, (char *)*packed);

	} else if (packed_elsize == 1) {

		/* If the elements are VistaIOBits, this packs them: */
		VistaIOPackBits (nels, packed_order, (VistaIOBit *) unpacked,
			   (char *)*packed);

	} else
		/* Packing multi-byte integers or floats is currently not supported: */
		VistaIOError ("VistaIOPackData: Packing %s from %d to %d bits is not supported", VistaIORepnName (repn), unpacked_elsize, packed_elsize);

	return TRUE;
}


/*! \brief Convert an array of data elements from packed to unpacked form.
 *
 *  The elements to be packed are integers or floating point numbers, as
 *  specified by repn. Each element's unpacked size is unpacked_elsize
 *  bits, and its packed size is packed_elsize bits. There are nels of
 *  them, beginning at packed. Packed_order specifies whether they are
 *  to be unpacked from MSB to LSB (VistaIOBigEndian), or vice versa (VistaIOLittleEndian).
 *
 *  To unpack into a buffer already allocated:
 *
 *	length = length of buffer;
 *	unpacked = address of buffer;
 *	VistaIOUnpackData ( ..., & length, & unpacked, NULL);
 *
 *  and on return length will be set to the length of the packed data.
 *
 *  To unpack into a buffer supplied by VistaIOUnpackData:
 *
 *	VistaIOUnpackData ( ..., & length, & unpacked, & alloced);
 *
 *  and on return length will be set to the length of the unpacked data,
 *  unpacked will be set to point to it, and alloced will be TRUE if
 *  the storage unpacked points to is a buffer that was allocated just for
 *  the unpacked data. If alloced is FALSE, unpacked is pointing to the same
 *  place as the packed data because the packed and unpacked forms are
 *  identical.
 *
 *  These assumptions are made:
 *    - packed_elsize is either 1 or a multiple of 8
 *    - if packed_elsize is 1, then the unpacked data elements are VistaIOBits
 *    - unpacked_elsize >= packed_elsize
 *
 *  \param  repn
 *  \param  nels
 *  \param  packed
 *  \param  packed_order
 *  \param  length
 *  \param  unpacked
 *  \param  alloced
 *  \return VistaIOBoolean
 */

VistaIOBoolean VistaIOUnpackData (VistaIORepnKind repn,
		      int64_t nels, VistaIOPointer packed, VistaIOPackOrder packed_order,
		      int64_t * length, VistaIOPointer * unpacked,
		      VistaIOBoolean * alloced)
{
	VistaIOPackOrder unpacked_order;
	int64_t unpacked_elsize = VistaIORepnSize (repn) * CHAR_BIT;
	int64_t packed_elsize = VistaIORepnPrecision (repn);
	int64_t unpacked_length = nels * VistaIORepnSize (repn);

	/* If a space for the unpacked data was supplied, ensure there's
	   enough of it: */
	if (!alloced && unpacked_length > *length) {
		VistaIOWarning ("VistaIOUnpackData: Insufficient space for unpacked data");
		return FALSE;
	}
	*length = unpacked_length;

	/* Determine the present machine's internal byte order: */
	unpacked_order = MachineByteOrder ();

	/* If the desired byte order matches that of the present machine's, and
	   the unpacked and packed data element sizes are identical,
	   just return the packed data: */
	if (unpacked_order == packed_order
	    && unpacked_elsize == packed_elsize) {
		if (alloced) {
			*unpacked = packed;
			*alloced = FALSE;
		} else if (packed != *unpacked)
			memcpy (*unpacked, packed, unpacked_length);
		return TRUE;
	}

	/* Unpack data elements into the buffer: */
	if (packed_elsize == unpacked_elsize) {

		/* If the packed and unpacked are the same size, do a straight copy: */
		if (packed != *unpacked)
			memcpy (*unpacked, packed, unpacked_length);

		/* Swap bytes if necessary: */
		if (packed_order != unpacked_order && packed_elsize > 8)
			SwapBytes (nels, packed_elsize / 8,
				   (char *)*unpacked);

	} else if (packed_elsize == 1) {

		/* If the elements are VistaIOBits, this unpacks them: */
		VistaIOUnpackBits (nels, packed_order, (char *)packed,
			     (char *)*unpacked);

	} else
		/* Unpacking multi-byte integers or floats is currently not
		   supported: */
		VistaIOError ("VistaIOUnpackData: "
			"Unpacking %s from %d to %d bits is not supported",
			VistaIORepnName (repn), packed_elsize, unpacked_elsize);

	return TRUE;
}


/*
 *  MachineByteOrder
 *
 *  Returns VistaIOLittleEndian if the first byte of a word is a low-order byte, 
 *  VistaIOBigEndian if it's a high-order byte.
 */

static VistaIOPackOrder MachineByteOrder ()
{
	union {
		short s;
		char c[sizeof (short)];
	} u;

	u.s = 1;
	if (u.c[0] == 1)
		return VistaIOLsbFirst;
	if (u.c[sizeof (short) - 1] != 1)
		VistaIOError ("VistaIOPackImage or VistaIOUnpackImage: Byte order not recognized");
	return VistaIOMsbFirst;
}


/*! \brief Pack the low order bits of consecutive VistaIOBit data elements.
 *  
 *  unpacked and packed can point to the same place.
 *
 *  \param  nels
 *  \param  packed_order
 *  \param  unpacked
 *  \param  packed
 */

void VistaIOPackBits (int64_t nels, VistaIOPackOrder packed_order, VistaIOBit * unpacked,
		      char *packed)
{
	int bit;
	char byte;

	if (packed_order == VistaIOLsbFirst)
		while (nels > 0) {
			byte = 0;
			for (bit = 0; bit < 8 && nels > 0; nels--, bit++)
				if (*unpacked++)
					byte |= (1 << bit);
			*packed++ = byte;
	} else
		while (nels > 0) {
			byte = 0;
			for (bit = 7; bit >= 0 && nels > 0; nels--, bit--)
				if (*unpacked++)
					byte |= (1 << bit);
			*packed++ = byte;
		}
}


/*! \brief Unpack into the low order bits of consecutive VistaIOBit data elements.
 *  
 *  packed and unpacked can point to the same place.
 *
 *  \param  nels
 *  \param  packed_order
 *  \param  packed
 *  \param  unpacked
 */

void VistaIOUnpackBits (int64_t nels, VistaIOPackOrder packed_order, char *packed,
		  VistaIOBit * unpacked)
{
	int bit;
	char byte;

	/* Compute the position of the first bit to be unpacked, which is the
	   last bit of the vector: */
	bit = (nels + 7) % 8;
	if (packed_order == VistaIOMsbFirst)
		bit = 7 - bit;

	/* Unpack bits from last to first. For each byte to be unpacked: */
	packed += (nels + 7) / 8;
	unpacked += nels;
	if (packed_order == VistaIOLsbFirst)
		while (nels > 0) {
			byte = *--packed;

			/* For each bit to be unpacked within that byte: */
			for (; bit >= 0 && nels > 0; nels--, bit--) {

				/* Unpack a bit: */
				*--unpacked = (byte >> bit) & 1;
			}
			bit = 7;
	} else
		while (nels > 0) {
			byte = *--packed;

			/* For each bit to be unpacked within that byte: */
			for (; bit < 8 && nels > 0; nels--, bit++) {

				/* Unpack a bit: */
				*--unpacked = (byte >> bit) & 1;
			}
			bit = 0;
		}
}


/*! \brief Reverses the byte order of nels elements, each of elsize bytes, 
 *         stored at data.
 *  
 *  \param  nels
 *  \param  elsize
 *  \param  data
 */

static void SwapBytes (int64_t nels, uint64_t elsize, char *data)
{
	int64_t i;
	char *pl, *pu, byte;

	for (i = 0; i < nels; i++, data += elsize)
		for (pl = data, pu = data + elsize - 1; pl < pu; pl++, pu--) {
			byte = *pl;
			*pl = *pu;
			*pu = byte;
		}
}
