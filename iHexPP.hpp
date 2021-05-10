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

#pragma once

#include <functional>

#include <cstring>
#include <cinttypes>

namespace SudoMaker {
	namespace iHexPP {
		struct iHexState {
			uint32_t address;
			uint16_t segment;
			uint8_t flags;
			uint8_t line_length;
			uint8_t length;
			uint8_t data[256];
		};

		enum iHexRecordType {
			IHEX_DATA_RECORD,
			IHEX_END_OF_FILE_RECORD,
			IHEX_EXTENDED_SEGMENT_ADDRESS_RECORD,
			IHEX_START_SEGMENT_ADDRESS_RECORD,
			IHEX_EXTENDED_LINEAR_ADDRESS_RECORD,
			IHEX_START_LINEAR_ADDRESS_RECORD
		};

		class Decoder {
		protected:
			enum {
				IHEX_START = ':',
				ADDRESS_HIGH_MASK = 0xFFFF0000U,

				IHEX_READ_RECORD_TYPE_MASK = 0x07,
				IHEX_READ_STATE_MASK = 0x78,
				IHEX_READ_STATE_OFFSET = 3
			};

			enum iHexReadState {
				READ_WAIT_FOR_START = 0,
				READ_COUNT_HIGH = 1,
				READ_COUNT_LOW,
				READ_ADDRESS_MSB_HIGH,
				READ_ADDRESS_MSB_LOW,
				READ_ADDRESS_LSB_HIGH,
				READ_ADDRESS_LSB_LOW,
				READ_RECORD_TYPE_HIGH,
				READ_RECORD_TYPE_LOW,
				READ_DATA_HIGH,
				READ_DATA_LOW
			};

			iHexState ihex{};

			void end_read();

		public:
			Decoder() = default;

			std::function<void(uint8_t record_type, uint32_t addr, const uint8_t *data, uint8_t len)> read_callback;

			void reset() noexcept;

			void read(uint8_t byte);

			template <typename T>
			void read(const T& data) {
				for (const auto &it : data) {
					read((uint8_t)it);
				}
			}

			void read(uint8_t *data, size_t len) {
				for (size_t i=0; i<len; i++) {
					read(data[i]);
				}
			}
		};
	}
}