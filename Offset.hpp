#pragma once
#include <cstdint>

/*
// 1. 컴포넌트 인덱스 (서랍장의 고정된 칸 번호)
typedef enum _COMPONENT_INDEX {
    IDX_HEALTH   = 0,
    IDX_VELOCITY = 1,
    IDX_TEAM     = 2,
    IDX_BONE     = 3,
    IDX_OUTLINE  = 4,
    IDX_LINK     = 5,   // ★ 영혼을 찾는 열쇠 (육체만 소유)
    IDX_ROTATION = 6,
    IDX_SKILL    = 7,
    
    // 뒤쪽 칸: 영혼(제어/논리) 전용 인덱스
    IDX_HEROID   = 10,  // ★ 영웅 식별 (영혼만 소유)
    IDX_VIS      = 11,
    IDX_CONTROLR = 12,

    MAX_COMPONENTS = 64 // 서랍장의 전체 칸 개수 (엔진 규격에 따라 다름)
} COMPONENT_INDEX;

// 2. 공통 규격의 컴포넌트 배열 (서랍장)
// 육체든 영혼이든 무조건 이 규격의 배열을 메모리에 할당받습니다.
typedef struct _ComponentTable {
    // 각 칸에는 '암호화된 데이터 포인터'가 들어갑니다.
    // 해당 부품이 없으면 이 자리는 0 (Null)이 됩니다.
    uint64_t EncryptedPointers[MAX_COMPONENTS]; 
} ComponentTable;

// 3. 엔티티 공통 껍데기
typedef struct _EntityShell {
    ComponentTable* pTable; // 0x00: 공통 서랍장을 가리키는 포인터
    
    uint8_t pad_08[8];      // 0x08: 패딩
    uint32_t EntityFlags;   // 0x10: 플래그
} EntityShell;
*/

namespace offset {
    struct ScanOffsets {
        uint64_t entity_base; // 기존 offset::Address_entity_base 값
        uint32_t Link_UniqueId  = 0xD4;
        uint32_t Common_UniqueId = 0x138;
        uint32_t Component_Ptr  = 0x30;
        uint32_t Entity_Id      = 0x10;
    };
}
