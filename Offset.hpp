#pragma once
#include <cstdint>

/*
  [ cur_entity / ComponentParent ] (Memory Region)
  +--------------------------------------------------------+
  | +[OFFSET_COMPONENT_TABLE] ---> [ Component Array ]     |
  +--------------------------+    +-----------------------+
  | +[OFFSET_ENTITY_FLAGS]   |    | [0] TYPE_HEALTH       |---> (Decrypt) ---> [ c_entity::HealthBase ]
  +--------------------------+    +-----------------------+
  | +[OFFSET_PTR_30] ------+ |    | [1] TYPE_VELOCITY     |---> (Decrypt) ---> [ c_entity::VelocityBase ]
  +------------------------| |    +-----------------------+
  | +[OFFSET_ENTITY_TYPE]  | |    | [2] TYPE_TEAM         |---> (Decrypt) ---> [ c_entity::TeamBase ]
  +------------------------| |    +-----------------------+
                           | |    | [3] TYPE_BONE         |---> (Decrypt) ---> [ c_entity::BoneBase ]
                           | |    +-----------------------+
  +------------------------+ |    | [4] TYPE_OUTLINE      |---> (Decrypt) ---> [ c_entity::OutlineBase ]
  |                          |    +-----------------------+
  v [ Ptr_30 Data Block ]    |    | [5] TYPE_LINK         |---> (Decrypt) ---> [ c_entity::LinkBase ]
  +-----------------------+  |    +-----------------------+
  | +[OFFSET_ENTITY_ID]   |  |    | [6] TYPE_ROTATION     |---> (Decrypt) ---> [ c_entity::RotationBase ]
  | (Object Resource Hash)|  |    +-----------------------+
  +-----------------------+  |    | [7] TYPE_SKILL        |---> (Decrypt) ---> [ c_entity::SkillBase ]
                             |    +-----------------------+
  +--------------------------+
  v
  [ LinkParent ] (Memory Region)
  +--------------------------------------------------------+
  | +[OFFSET_COMPONENT_TABLE] ---> [ Link Component Array ]|
  +--------------------------+    +-----------------------+
  | ...                      |    | [x] TYPE_P_HEROID     |---> (Decrypt) ---> [ c_entity::HeroBase ]
  +--------------------------+    +-----------------------+
                                  | [y] TYPE_P_VISIBILITY |---> (Decrypt) ---> [ c_entity::VisBase ]
                                  +-----------------------+
                                  | [z] PLAYERCONTROLLER  |---> (Decrypt) ---> [ c_entity::AngleBase ]
                                  +-----------------------+

  [ c_entity Class Instance Layout ]
  +-------------------------------------------------------------------------------+
  | .address       ---> ComponentParent Address (Raw Entity Pointer)              |
  +-------------------------------------------------------------------------------+
  | .HealthBase    ---> [HEALTH]   ---RPM---> .PlayerHealth / .PlayerHealthMax     |
  |                                           (isImmortal / isBarrierProjected)   |
  +-------------------------------------------------------------------------------+
  | .VelocityBase  ---> [VELOCITY] ---RPM---> .pos (3D Coord) / .velocity (Speed)  |
  +-------------------------------------------------------------------------------+
  | .BoneBase      ---> [BONE]     ---RPM---> .head_pos / .neck_pos / .chest_pos   |
  +-------------------------------------------------------------------------------+
  | .HeroBase      ---> [HEROID]   ---RPM---> .HeroID (Wreckingball, D.va, etc)   |
  +-------------------------------------------------------------------------------+
  | .LinkBase      ---> [LINK]     ---RPM---> .unique_id (Network Matching ID)    |
  +-------------------------------------------------------------------------------+
  | .TeamBase      ---> [TEAM]     ---RPM---> .Team (Enemy/Ally Bool Flag)        |
  +-------------------------------------------------------------------------------+
  | .VisBase       ---> [VIS]      ---RPM---> .Vis (Visible / Wall check)         |
  +-------------------------------------------------------------------------------+
  | .SkillBase     ---> [SKILL]    ---RPM---> .skill1act / .ultimate (Ult Gauge)  |
  +-------------------------------------------------------------------------------+
  | .OutlineBase   ---> [OUTLINE]  ---WPM---> Ingame Glow / Border (Rainbow/HP)   |
  +-------------------------------------------------------------------------------+
  | .AngleBase     ---> [ANGLES]   ---WPM---> (Local Only) ViewAngle / NoRecoil   |
  +-------------------------------------------------------------------------------+
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
