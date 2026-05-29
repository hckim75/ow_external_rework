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

// 특정 엔티티가 육체인지 영혼인지 판별하는 가상 함수
void CheckEntityType(EntityShell* entity) {
    ComponentTable* table = entity->pTable;
    
    // 1. 육체 판별 로직
    // 5번 칸(IDX_LINK)에 값이 존재하면 100% 육체입니다.
    if (table->EncryptedPointers[IDX_LINK] != 0) {
        printf("이 엔티티는 육체(ComponentParent)입니다.\n");
    }
    
    // 2. 영혼 판별 로직
    // 5번 칸(IDX_LINK)이 텅 비어있고, 10번 칸(IDX_HEROID)에 값이 있다면 100% 영혼입니다.
    else if (table->EncryptedPointers[IDX_LINK] == 0 && 
             table->EncryptedPointers[IDX_HEROID] != 0) {
        printf("이 엔티티는 영혼(LinkParent)입니다.\n");
    }
    
    // 3. 기타 잡동사니
    // 둘 다 비어있다면 물리 깡통(드럼통 등)이거나 다른 오브젝트입니다.
    else {
        printf("이 엔티티는 단순 오브젝트입니다.\n");
    }
}
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
