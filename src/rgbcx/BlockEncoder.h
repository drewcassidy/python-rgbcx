/*  Python-rgbcx Texture Compression Library
    Copyright (C) 2021 Andrew Cassidy <drewcassidy@me.com>
    Partially derived from rgbcx.h written by Richard Geldreich 2020 <richgel99@gmail.com>
    and licenced under the public domain

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <climits>
#include <cstdint>
#include <memory>
#include <string>

#include "BlockView.h"

namespace rgbcx {

class BlockEncoder {
   public:
    using EncoderPtr = std::shared_ptr<BlockEncoder>;

    virtual ~BlockEncoder() = default;
    virtual void EncodeImage(uint8_t *encoded, Color *decoded, unsigned image_width, unsigned image_height) const = 0;
    virtual size_t BlockSize() const = 0;
    virtual size_t BlockWidth() const = 0;
    virtual size_t BlockHeight() const = 0;

    static EncoderPtr MakeEncoder(std::string fourcc);
};

template <class B, size_t M, size_t N> class BlockEncoderTemplate : public BlockEncoder {
   public:
    using DecodedBlock = ColorBlockView<M, N>;
    using EncodedBlock = B;

    BlockEncoderTemplate() noexcept = default;
    virtual ~BlockEncoderTemplate() noexcept = default;

    virtual void EncodeBlock(DecodedBlock pixels, EncodedBlock *dest) const = 0;

    virtual void EncodeImage(uint8_t *encoded, Color *decoded, unsigned image_width, unsigned image_height) const override {
        assert(image_width % N == 0);
        assert(image_width % M == 0);

        unsigned block_width = image_width / N;
        unsigned block_height = image_height / M;

        auto blocks = reinterpret_cast<B *>(encoded);

        // from experimentation, multithreading this using OpenMP actually makes decoding slower
        // due to thread creation/teardown taking longer than the decoding process itself.
        // As a result, this is left as a serial operation despite being embarassingly parallelizable
        for (unsigned y = 0; y < block_height; y++) {
            for (unsigned x = 0; x < block_width; x++) {
                unsigned pixel_x = x * N;
                unsigned pixel_y = y * M;

                assert(pixel_x >= 0);
                assert(pixel_y >= 0);
                assert(pixel_y + M <= image_height);
                assert(pixel_x + N <= image_width);

                unsigned top_left = pixel_x + (pixel_y * image_width);
                auto src = DecodedBlock(&decoded[top_left], (int)image_width);

                EncodeBlock(src, &blocks[x + block_width * y]);
            }
        }
    }

    virtual size_t BlockSize() const override { return sizeof(B); }
    virtual size_t BlockWidth() const override { return N; }
    virtual size_t BlockHeight() const override { return M; }
};
}  // namespace rgbcx