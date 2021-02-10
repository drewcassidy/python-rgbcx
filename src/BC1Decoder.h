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

#include "BlockDecoder.h"
#include "blocks.h"
#include "interpolator.h"
namespace rgbcx {
class BC1Decoder : public BlockDecoder<BC1Block, 4, 4> {
   public:
    BC1Decoder(const Interpolator &interpolator) : _interpolator(interpolator) {}

    BC1Decoder() : BC1Decoder(Interpolator()) {}

    void DecodeBlock(const Color4x4 *dest, const BC1Block *block) override;

   private:
    const Interpolator &_interpolator;
};
}  // namespace rgbcx
