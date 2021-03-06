// SPDX-License-Identifier: GPL-3.0-only

#ifndef INVADER__PARSER__COMPILE__HUD_INTERFACE_HPP
#define INVADER__PARSER__COMPILE__HUD_INTERFACE_HPP

#include <cstddef>
#include <invader/tag/parser/parser.hpp>

namespace Invader::Parser {
    void get_sequence_data(const Invader::BuildWorkload &workload, const HEK::TagID &tag_id, std::size_t &sequence_count, const BitmapGroupSequence::struct_little *&sequences, char *bitmap_tag_path, std::size_t bitmap_tag_path_size, HEK::BitmapType &bitmap_type);
}

#endif
