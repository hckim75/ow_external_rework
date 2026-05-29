#pragma once
#include <cstdint>

/*
[최상위 엔티티 베이스 주소] (RawEntity*)
   │
   ├──> 1. [0x00] ComponentTablePtr 
   │         │
   │         └──> [TYPE_LINK 인덱스] ──> (DecryptComponent 복호화 연산)
   │                                           │
   │                                           └──> [CommonLinker* 주소 변환]
   │                                                   │
   │                                                   └──> [0xD4] NetworkUniqueID (값 추출)
   │
   v
[2차 스캔 대상을 시드로 수색] (LinkParentEntity*)
   │
   └──> 2. [0x138] CommonUniqueID (값 추출)
             │
             └──> [대조 작업]: NetworkUniqueID == CommonUniqueID 인가?
                       │
                       └──> (TRUE 일 때): 해당 스캔 대상을 [LinkParent]로 확정
                                 │
                                 └──> [0x00] LinkComponentTablePtr 접근 가능
                                           │
                                           ├──> [TYPE_P_HEROID] ──> 복호화 ──> HeroBase
                                           └──> [CONTROLLER]     ──> 복호화 ──> AngleBase

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
