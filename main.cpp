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

#include <iostream>
#include <fstream>

#include "iHexPP.hpp"

using namespace SudoMaker;

int main(int argc, char **argv) {
	std::ifstream infile(argv[1]);
	std::string line;

	iHexPP::Decoder dec;

	dec.read_callback = [](uint8_t record_type, uint32_t addr, const uint8_t *data, size_t len){
		if (record_type == iHexPP::IHEX_DATA_RECORD) {
			printf("%08x: ", addr);

			for (size_t i = 0; i < len; i++) {
				printf("%02x ", data[i]);
			}
		}
	};

	while (std::getline(infile, line)) {
		dec.read(line);
		puts("");
	}

}
