/*
    This file is part of iHexPP.
    Copyright (C) 2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

/* This library uses code from github.com/arkku/ihex, which is MIT licensed */

#include "iHexPP.hpp"

using namespace SudoMaker::iHexPP;

void Decoder::read(uint8_t byte) {
	uint_fast8_t b = (uint_fast8_t) byte;
	uint_fast8_t len = ihex.length;
	uint_fast8_t state = (ihex.flags & IHEX_READ_STATE_MASK);
	ihex.flags ^= state; // turn off the old state
	state >>= IHEX_READ_STATE_OFFSET;

	if (b >= '0' && b <= '9') {
		b -= '0';
	} else if (b >= 'A' && b <= 'F') {
		b -= 'A' - 10;
	} else if (b >= 'a' && b <= 'f') {
		b -= 'a' - 10;
	} else if (b == IHEX_START) {
		// sync to a new record at any state
		state = READ_COUNT_HIGH;
		goto end_read;
	} else {
		// ignore unknown characters (e.g., extra whitespace)
		goto save_read_state;
	}

	if (!(++state & 1)) {
		// high nybble, store temporarily at end of data:
		b <<= 4;
		ihex.data[len] = b;
	} else {
		// low nybble, combine with stored high nybble:
		b = (ihex.data[len] |= b);
		// We already know the lowest bit of `state`, dropping it may produce
		// smaller code, hence the `>> 1` in switch and its cases.
		switch (state >> 1) {
			default:
				// remain in initial state while waiting for :
				return;
			case (READ_COUNT_LOW >> 1):
				// data length
				ihex.line_length = b;
				break;
			case (READ_ADDRESS_MSB_LOW >> 1):
				// high byte of 16-bit address
				ihex.address &= ADDRESS_HIGH_MASK; // clear the 16-bit address
				ihex.address |= ((uint32_t) b) << 8U;
				break;
			case (READ_ADDRESS_LSB_LOW >> 1):
				// low byte of 16-bit address
				ihex.address |= (uint32_t) b;
				break;
			case (READ_RECORD_TYPE_LOW >> 1):
				// record type
				if (b & ~IHEX_READ_RECORD_TYPE_MASK) {
					// skip unknown record types silently
					return;
				}
				ihex.flags = (ihex.flags & ~IHEX_READ_RECORD_TYPE_MASK) | b;
				break;
			case (READ_DATA_LOW >> 1):
				if (len < ihex.line_length) {
					// data byte
					ihex.length = len + 1;
					state = READ_DATA_HIGH;
					goto save_read_state;
				}
				// end of line (last "data" byte is checksum)
				state = READ_WAIT_FOR_START;
			end_read:
				end_read();
		}
	}
save_read_state:
	ihex.flags |= state << IHEX_READ_STATE_OFFSET;
}

void Decoder::end_read() {
	uint_fast8_t type = ihex.flags & IHEX_READ_RECORD_TYPE_MASK;
	uint_fast8_t sum;
	if ((sum = ihex.length) == 0 && type == IHEX_DATA_RECORD) {
		return;
	}
	{
		// compute and validate checksum
		const uint8_t * const eptr = ihex.data + sum;
		const uint8_t *r = ihex.data;
		sum += type + (ihex.address & 0xFFU) + ((ihex.address >> 8) & 0xFFU);
		while (r != eptr) {
			sum += *r++;
		}
		sum = (~sum + 1U) ^ *eptr; // *eptr is the received checksum
	}

	read_callback(type, ihex.address, ihex.data, ihex.length);

	if (type == IHEX_EXTENDED_LINEAR_ADDRESS_RECORD) {
		ihex.address &= 0xFFFFU;
		ihex.address |= (((uint32_t) ihex.data[0]) << 24) |
				(((uint32_t) ihex.data[1]) << 16);
	}

	ihex.length = 0;
	ihex.flags = 0;
}

void Decoder::reset() noexcept {
	memset(&ihex, 0, sizeof(iHexState));
}
