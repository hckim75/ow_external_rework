#pragma once
#include <cstdint>

namespace offset {
    struct ScanOffsets {
        uint64_t entity_base; // 기존 offset::Address_entity_base 값
        uint32_t Link_UniqueId  = 0xD4;
        uint32_t Common_UniqueId = 0x138;
        uint32_t Component_Ptr  = 0x30;
        uint32_t Entity_Id      = 0x10;
    };
}
