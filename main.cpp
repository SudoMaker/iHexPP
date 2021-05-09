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

#include <vector>

#include "iHexPP.hpp"

using namespace SudoMaker;

int main(int argc, char **argv) {
	std::ifstream infile(argv[1]);
	std::string line;

	iHexPP::Decoder dec;

	std::vector<uint8_t> buf;
	size_t max_size = 0;

	dec.read_callback = [&](uint32_t addr, const uint8_t *data, size_t len){
		size_t required_size = addr + len;

		if (required_size > max_size) {
			max_size = required_size;
			buf.resize(max_size);
		}

		memcpy(buf.data()+addr, data, len);


	};

	while (std::getline(infile, line)) {
		dec.read(line);
		puts("");
	}

	for (size_t i=0; i<buf.size(); i++) {
		if (i % 16 == 0)
			printf("\n%08x: ", i);

		printf("%02x ", buf[i]);
	}
}
