// SPDX-License-Identifier: GPL-3.0-only

#include "invader/hek/map.hpp"
#include "invader/tag/hek/class/bitmap.hpp"
#include "invader/tag/hek/class/gbxmodel.hpp"
#include "invader/tag/hek/class/scenario.hpp"
#include "invader/tag/hek/class/sound.hpp"
#include "invader/resource/hek/resource_map.hpp"
#include "invader/compress/compression.hpp"

#include "invader/map/map.hpp"

namespace Invader {
    Map Map::map_with_copy(const std::byte *data, std::size_t data_size,
                           const std::byte *bitmaps_data, std::size_t bitmaps_data_size,
                           const std::byte *loc_data, std::size_t loc_data_size,
                           const std::byte *sounds_data, std::size_t sounds_data_size) {
        Map map;
        if(!map.decompress_if_needed(data, data_size)) {
            map.data_m.insert(map.data_m.end(), data, data + data_size);
        }
        map.data = map.data_m.data();
        map.data_length = map.data_m.size();
        map.bitmap_data_m.insert(map.bitmap_data_m.end(), bitmaps_data, bitmaps_data + bitmaps_data_size);
        map.bitmap_data = map.bitmap_data_m.data();
        map.bitmap_data_length = bitmaps_data_size;
        map.sound_data_m.insert(map.sound_data_m.end(), sounds_data, sounds_data + sounds_data_size);
        map.sound_data = map.sound_data_m.data();
        map.sound_data_length = sounds_data_size;
        map.loc_data_m.insert(map.loc_data_m.end(), loc_data, loc_data + loc_data_size);
        map.loc_data = map.loc_data_m.data();
        map.loc_data_length = loc_data_size;
        map.load_map();
        return map;
    }

    Map Map::map_with_pointer(std::byte *data, std::size_t data_size,
                              std::byte *bitmaps_data, std::size_t bitmaps_data_size,
                              std::byte *loc_data, std::size_t loc_data_size,
                              std::byte *sounds_data, std::size_t sounds_data_size) {
        Map map;
        map.data = data;
        map.data_length = data_size;
        map.bitmap_data = bitmaps_data;
        map.bitmap_data_length = bitmaps_data_size;
        map.loc_data = loc_data;
        map.loc_data_length = loc_data_size;
        map.sound_data = sounds_data;
        map.sound_data_length = sounds_data_size;
        map.load_map();
        return map;
    }

    bool Map::decompress_if_needed(const std::byte *data, std::size_t data_size) {
        const auto *potential_header = reinterpret_cast<const HEK::CacheFileHeader *>(data);
        bool needs_decompressed = false;
        if(data_size > sizeof(*potential_header) && potential_header->valid()) {
            // Check if it needs decompressed
            switch(potential_header->engine.read()) {
                case HEK::CacheFileEngine::CACHE_FILE_DARK_CIRCLET:
                    if(potential_header->decompressed_file_size != 0) {
                        needs_decompressed = true;
                    }
                    break;
                case HEK::CacheFileEngine::CACHE_FILE_RETAIL_COMPRESSED:
                case HEK::CacheFileEngine::CACHE_FILE_CUSTOM_EDITION_COMPRESSED:
                case HEK::CacheFileEngine::CACHE_FILE_DEMO_COMPRESSED:
                    needs_decompressed = true;
                default:
                    break;
            }

            // If so, decompress it
            if(needs_decompressed) {
                this->data_m = Compression::decompress_map_data(data, data_size);
            }
        }

        return needs_decompressed;
    }

    std::byte *Map::get_data_at_offset(std::size_t offset, std::size_t minimum_size, DataMapType map_type) {
        std::size_t max_length;
        std::byte *data_ptr;
        switch(map_type) {
            case DATA_MAP_CACHE:
                max_length = this->data_length;
                data_ptr = this->data;
                break;

            case DATA_MAP_BITMAP:
                max_length = this->bitmap_data_length;
                data_ptr = this->bitmap_data;
                break;

            case DATA_MAP_SOUND:
                max_length = this->sound_data_length;
                data_ptr = this->sound_data;
                break;

            case DATA_MAP_LOC:
                max_length = this->loc_data_length;
                data_ptr = this->loc_data;
                break;

            default:
                throw OutOfBoundsException();
        }

        if(offset >= max_length || offset + minimum_size > max_length) {
            throw OutOfBoundsException();
        }
        else {
            return data_ptr + offset;
        }
    }

    std::byte *Map::get_data() noexcept {
        return this->data;
    }

    std::size_t Map::get_data_length() const noexcept {
        return this->data_length;
    }

    const std::byte *Map::get_data_at_offset(std::size_t offset, std::size_t minimum_size, DataMapType map_type) const {
        return const_cast<Map *>(this)->get_data_at_offset(offset, minimum_size, map_type);
    }

    std::byte *Map::get_tag_data_at_offset(std::size_t offset, std::size_t minimum_size) {
        if(offset >= this->tag_data_length || offset + minimum_size > this->tag_data_length) {
            throw OutOfBoundsException();
        }
        else {
            return this->tag_data + offset;
        }
    }

    const std::byte *Map::get_tag_data_at_offset(std::size_t offset, std::size_t minimum_size) const {
        return const_cast<Map *>(this)->get_tag_data_at_offset(offset, minimum_size);
    }

    std::byte *Map::resolve_tag_data_pointer(HEK::Pointer pointer, std::size_t minimum_size) {
        return this->get_tag_data_at_offset(pointer - this->base_memory_address, minimum_size);
    }

    const std::byte *Map::resolve_tag_data_pointer(HEK::Pointer pointer, std::size_t minimum_size) const {
        return const_cast<Map *>(this)->resolve_tag_data_pointer(pointer, minimum_size);
    }

    std::size_t Map::tag_count() const noexcept {
        return this->tags.size();
    }

    Tag &Map::get_tag(std::size_t index) {
        if(index >= this->tag_count()) {
            throw OutOfBoundsException();
        }
        else {
            return this->tags[index];
        }
    }

    const Tag &Map::get_tag(std::size_t index) const {
        return const_cast<Map *>(this)->get_tag(index);
    }

    std::size_t Map::get_scenario_tag_id() const noexcept {
        return this->scenario_tag_id;
    }

    HEK::CacheFileTagDataHeader &Map::get_tag_data_header() noexcept {
        return *reinterpret_cast<HEK::CacheFileTagDataHeader *>(this->tag_data);
    }

    const HEK::CacheFileTagDataHeader &Map::get_tag_data_header() const noexcept {
        return const_cast<Map *>(this)->get_tag_data_header();
    }

    const HEK::CacheFileHeader &Map::get_cache_file_header() const noexcept {
        return this->header;
    }

    void Map::load_map() {
        using namespace Invader::HEK;

        // Get header
        auto *header_maybe = reinterpret_cast<const CacheFileHeader *>(this->get_data_at_offset(0, sizeof(CacheFileHeader)));

        // Check if the literals are invalid
        if(header_maybe->head_literal != CACHE_FILE_HEAD || header_maybe->foot_literal != CACHE_FILE_FOOT) {
            // Maybe it's a demo map?
            auto *demo_header_maybe = reinterpret_cast<const CacheFileDemoHeader *>(this->get_data_at_offset(0, sizeof(CacheFileDemoHeader)));
            if(demo_header_maybe->head_literal != CACHE_FILE_HEAD_DEMO || demo_header_maybe->foot_literal != CACHE_FILE_FOOT_DEMO) {
                throw InvalidMapException();
            }
            this->header = *demo_header_maybe;
        }
        else {
            this->header = *header_maybe;
        }

        // If we don't know the type of engine, bail
        switch(this->header.engine) {
            case CacheFileEngine::CACHE_FILE_DEMO:
            case CacheFileEngine::CACHE_FILE_RETAIL:
            case CacheFileEngine::CACHE_FILE_CUSTOM_EDITION:
                break;
            case CacheFileEngine::CACHE_FILE_DARK_CIRCLET:
                if(this->header.decompressed_file_size != 0) {
                    throw MapNeedsDecompressedException();
                }
                break;
            case CacheFileEngine::CACHE_FILE_RETAIL_COMPRESSED:
            case CacheFileEngine::CACHE_FILE_CUSTOM_EDITION_COMPRESSED:
            case CacheFileEngine::CACHE_FILE_DEMO_COMPRESSED:
                throw MapNeedsDecompressedException();
            default:
                throw UnsupportedMapEngineException();
        }

        // Check if any overflowing occurs
        if(this->header.decompressed_file_size > this->data_length || this->header.build.overflows() || this->header.name.overflows()) {
            throw InvalidMapException();
        }

        // Get tag data
        if(this->header.engine == CacheFileEngine::CACHE_FILE_DARK_CIRCLET) {
            this->base_memory_address = HEK::CACHE_FILE_DARK_CIRCLET_BASE_MEMORY_ADDRESS;
        }
        else if(this->header.engine == CacheFileEngine::CACHE_FILE_DEMO) {
            this->base_memory_address = HEK::CACHE_FILE_DEMO_BASE_MEMORY_ADDRESS;
        }

        this->tag_data_length = this->header.tag_data_size;
        this->tag_data = this->get_data_at_offset(this->header.tag_data_offset, this->tag_data_length);

        this->populate_tag_array();
    }

    void Map::populate_tag_array() {
        using namespace Invader::HEK;
        const auto &header = *reinterpret_cast<const CacheFileTagDataHeaderPC *>(this->get_tag_data_at_offset(0, sizeof(CacheFileTagDataHeaderPC)));

        std::size_t tag_count = header.tag_count;
        this->tags.reserve(tag_count);

        this->scenario_tag_id = header.scenario_tag.read().index;
        if(this->scenario_tag_id >= tag_count) {
            throw OutOfBoundsException();
        }

        // First get the tags
        const auto *tags = reinterpret_cast<const CacheFileTagDataTag *>(this->resolve_tag_data_pointer(header.tag_array_address, sizeof(CacheFileTagDataTag) * tag_count));

        // Have a pointer for the end of the tag data so we can check to make sure things aren't null terminated
        const char *tag_data_end = reinterpret_cast<const char *>(this->tag_data) + this->tag_data_length;

        for(std::size_t i = 0; i < tag_count; i++) {
            this->tags.push_back(Tag(*this));
            auto &tag = this->tags[i];
            tag.p_tag_class_int = tags[i].primary_class;

            try {
                auto *path = reinterpret_cast<const char *>(this->resolve_tag_data_pointer(tags[i].tag_path));

                // Make sure the path is null-terminated
                bool zeroed = false;
                for(auto *path_test = path; path < tag_data_end; path_test++) {
                    if(*path_test == 0) {
                        zeroed = true;
                        break;
                    }
                }

                // If it was null terminated, use it. Otherwise, don't.
                if(zeroed) {
                    tag.p_path = path;
                }
            }
            catch (Invader::OutOfBoundsException &) {}

            if(tag.p_tag_class_int == TagClassInt::TAG_CLASS_SCENARIO_STRUCTURE_BSP) {
                continue;
            }
            else if(tags[i].indexed) {
                tag.indexed = true;

                // Find where it's located
                DataMapType type;
                switch(tag.p_tag_class_int) {
                    case TagClassInt::TAG_CLASS_BITMAP:
                        type = DataMapType::DATA_MAP_BITMAP;
                        tag.base_struct_pointer = 0;
                        break;
                    case TagClassInt::TAG_CLASS_SOUND:
                        type = DataMapType::DATA_MAP_SOUND;
                        tag.base_struct_pointer = ~static_cast<HEK::Pointer>(sizeof(HEK::Sound<LittleEndian>));
                        break;
                    default:
                        type = DataMapType::DATA_MAP_LOC;
                        tag.base_struct_pointer = 0;
                        break;
                }

                // Next, check if we have that
                if(type == DataMapType::DATA_MAP_BITMAP && this->bitmap_data_length == 0) {
                    continue;
                }
                else if(type == DataMapType::DATA_MAP_SOUND && this->sound_data_length == 0) {
                    continue;
                }
                else if(type == DataMapType::DATA_MAP_LOC && this->loc_data_length == 0) {
                    continue;
                }

                // Let's begin.
                auto &header = *reinterpret_cast<ResourceMapHeader *>(this->get_data_at_offset(0, sizeof(ResourceMapHeader), type));
                auto count = header.resource_count.read();
                auto *indices = reinterpret_cast<ResourceMapResource *>(this->get_data_at_offset(header.resources, count * sizeof(ResourceMapResource), type));
                std::uint32_t resource_index = static_cast<std::uint32_t>(~0);

                // Find that index
                if(type == DataMapType::DATA_MAP_SOUND) {
                    auto *paths = reinterpret_cast<const char *>(this->get_data_at_offset(header.paths, 0, type));
                    for(std::uint32_t i = 1; i < count; i+=2) {
                        auto *path = paths + indices[i].path_offset;
                        if(tag.p_path == path) {
                            resource_index = i;
                            break;
                        }
                    }
                }
                else {
                    resource_index = tags[i].tag_data;
                }

                // Make sure it's valid
                if(resource_index >= count) {
                    throw OutOfBoundsException();
                }

                // Set it all
                auto &index = indices[resource_index];
                tag.tag_data_size = index.size;
                tag.base_struct_offset = index.data_offset;
            }
            else {
                tag.base_struct_pointer = tags[i].tag_data;
            }
        }

        this->get_bsps();
    }

    void Map::get_bsps() {
        using namespace Invader::HEK;

        auto &scenario_tag = this->tags[this->scenario_tag_id];
        auto &tag = scenario_tag.get_base_struct<Scenario>();
        std::size_t bsp_count = tag.structure_bsps.count;
        auto *bsps = scenario_tag.resolve_reflexive(tag.structure_bsps);

        for(std::size_t i = 0; i < bsp_count; i++) {
            auto &bsp = bsps[i];
            std::size_t bsp_id = bsp.structure_bsp.tag_id.read().index;
            if(bsp_id >= this->tags.size()) {
                throw OutOfBoundsException();
            }

            // Add the BSP stuff here
            auto &bsp_tag = this->tags[bsp_id];
            bsp_tag.tag_data_size = bsp.bsp_size;
            bsp_tag.base_struct_offset = bsp.bsp_start;
            bsp_tag.base_struct_pointer = bsp.bsp_address;
        }
    }
}
