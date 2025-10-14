#ifndef BITIO_HPP
#define BITIO_HPP

#include <ostream>
#include <istream>
#include <cstdint>

struct BitWriter {
    std::ostream &out;
    uint8_t buffer = 0;
    int nbits = 0; 

    BitWriter(std::ostream &o): out(o) {}

    void write_bit(int b) {
        buffer = (buffer << 1) | (b & 1);
        nbits++;
        if (nbits == 8) flush();
    }

    void write_bits(const std::string &bits) {
        for (char c : bits) write_bit(c == '1');
    }

    void flush() {
        if (nbits == 0) return;
        buffer <<= (8 - nbits);
        out.put(static_cast<char>(buffer));
        buffer = 0; nbits = 0;
    }
};

struct BitReader {
    std::istream &in;
    uint8_t buffer = 0;
    int nbits = 0;

    BitReader(std::istream &i): in(i) {}

    int read_bit() {
        if (nbits == 0) {
            int c = in.get();
            if (c == EOF) return -1;
            buffer = static_cast<uint8_t>(c);
            nbits = 8;
        }
        int bit = (buffer >> 7) & 1;
        buffer <<= 1;
        nbits--;
        return bit;
    }
};

#endif
