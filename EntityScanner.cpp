// Core/EntityScanner.cpp
#include "EntityScanner.hpp"
#include "Globals.hpp"
// TODO: Config.hpp, SDK.hpp 등 필요한 헤더 포함

namespace OW {
    int abletotread = 0;
    int entitytime = 0;

	inline std::vector<std::pair<uint64_t, uint64_t>> get_ow_entities()
	{
		std::vector<std::pair<uint64_t, uint64_t>> result;

		struct Entity {
			uint64_t entity;
			uint64_t pad;
		};

		uintptr_t entity_list = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_entity_base);

		MEMORY_BASIC_INFORMATION mbi{};
		VirtualQueryEx(SDK->hProcess, (LPCVOID)entity_list, &mbi, sizeof(mbi));

		SIZE_T entity_list_size = mbi.RegionSize / sizeof(Entity);
		Entity* raw_list = new Entity[entity_list_size];

		if (ReadProcessMemory(SDK->hProcess, (LPCVOID)entity_list, raw_list, mbi.RegionSize, nullptr))
		{
			for (size_t i = 0; i < entity_list_size; ++i)
			{
				uint64_t cur_entity = raw_list[i].entity;
				if (cur_entity)
				{
					uint64_t common_linker = DecryptComponent(cur_entity, TYPE_LINK);
					//printf("LINK:%llx\n", common_linker);
					if (common_linker)
					{
						uint32_t unique_id = SDK->RPM<uint32_t>(common_linker + 0xD4);
						//printf("unique_id:%llx\n", unique_id);
						for (size_t x = 0; x < entity_list_size; ++x)
						{
							uint64_t possible_common = raw_list[x].entity;
							if (possible_common && SDK->RPM<uint32_t>(possible_common + 0x138) == unique_id)
							{
								//printf("%llx", unique_id);
								result.emplace_back(possible_common, cur_entity);
								break;
							}
							else {
								uint64_t Ptr = SDK->RPM<uint64_t>(possible_common + 0x30) & 0xFFFFFFFFFFFFFFC0;
								if (Ptr < 0xFFFFFFFFFFFFFFEF) {
									uint64_t EntityID = SDK->RPM<uint64_t>(Ptr + 0x10);
									if (EntityID == 0x400000000000060 || EntityID == 0x40000000000480A || EntityID == 0x40000000000005F || EntityID == 0x400000000002533) {
										result.emplace_back(possible_common, cur_entity);
									}
								}
							}
						}
					}
				}
			}
		}
		delete[] raw_list;
		return result;
	}

    void entity_scan_thread() {
        while (Config::doingentity == 1) {
			if (abletotread == 0) {
				ow_entities_scan = get_ow_entities();
				abletotread = 1;
			}
			Sleep(10);
		}
    }

    void entity_thread() {
        // 기존 entity_thread 내부 코드 (1000줄 가량) 복사
        while (Config::doingentity == 1) {
			if (entitytime == 0) entitytime = GetTickCount();
			if (GetTickCount() - entitytime >= 100 && abletotread)
			{
				mutex.lock();
				ow_entities = ow_entities_scan;
				abletotread = 0;
				entitytime = GetTickCount();
				mutex.unlock();
			}
			/*if (!(ow_entities.size() > 0)) {
				mutex.lock();
				abletotread = 0;
				mutex.unlock();
				//entities = {};
				//local_entity.AngleBase=0;
				//entitytime = GetTickCount();
				Sleep(2000);
				continue;
			}*/
			if (!(ow_entities.size() > 0)) {
				mutex.lock();
				//abletotread = 0;
				entities = {};
				hp_dy_entities = {};
				mutex.unlock();
				//entities = {};
				//local_entity.AngleBase=0;
				//entitytime = GetTickCount();
				Sleep(1000);
				continue;
			}
			//mutex.unlock();
			std::vector<c_entity> tmp_entities{};
			std::vector<hpanddy> hpdy_entities{};
			//int howbigentitysize = ow_entities.size();
			//if (howbigentitysize > 30) howbigentitysize = 30;
			c_entity lastentity{};
			for (int i = 0; i < ow_entities.size(); i++) {
				c_entity entity{};
				if (!ow_entities[i].first || !ow_entities[i].second) continue;
				if (i > ow_entities.size()) continue;
				const auto& [ComponentParent, LinkParent] = ow_entities[i];//注意这里的&
				entity.address = ComponentParent;
				if (!entity.address) continue;
				if (!LinkParent) continue;

				uint64_t Ptr = SDK->RPM<uint64_t>(ComponentParent + 0x30) & 0xFFFFFFFFFFFFFFC0;
				if (Ptr < 0xFFFFFFFFFFFFFFEF) {
					uint64_t EntityID = SDK->RPM<uint64_t>(Ptr + 0x10);
					if (EntityID == 0x400000000000060 || EntityID == 0x40000000000480A || EntityID == 0x40000000000005F || EntityID == 0x400000000002533) {
						hpanddy hpdyentity{};
						hpdyentity.entityid = EntityID;
						hpdyentity.MeshBase = DecryptComponent(ComponentParent, TYPE_VELOCITY);
						hpdyentity.POS = SDK->RPM<XMFLOAT3>(hpdyentity.MeshBase + 0x380 + 0x50);
						hpdy_entities.push_back(hpdyentity);
						continue;
					}
				}

				entity.HealthBase = DecryptComponent(ComponentParent, TYPE_HEALTH);
				entity.LinkBase = DecryptComponent(LinkParent, TYPE_LINK);
				entity.TeamBase = DecryptComponent(ComponentParent, TYPE_TEAM);
				entity.VelocityBase = DecryptComponent(ComponentParent, TYPE_VELOCITY);
				entity.HeroBase = DecryptComponent(LinkParent, TYPE_P_HEROID);
				entity.OutlineBase = DecryptComponent(ComponentParent, TYPE_OUTLINE);
				entity.BoneBase = DecryptComponent(ComponentParent, TYPE_BONE);
				entity.RotationBase = DecryptComponent(ComponentParent, TYPE_ROTATION);
				entity.SkillBase = DecryptComponent(ComponentParent, TYPE_SKILL);
				entity.VisBase = DecryptComponent(LinkParent, TYPE_P_VISIBILITY);
				entity.AngleBase = DecryptComponent(LinkParent, TYPE_PLAYERCONTROLLER);
				entity.EnemyAngleBase = DecryptComponent(ComponentParent, TYPE_ANGLE);
				health_compo_t health_compo{};
				velocity_compo_t velo_compo{};
				hero_compo_t hero_compo{};
				vis_compo_t vis_compo{};


				//printf("%llx\n", SDK->RPM<uint16_t>(ComponentParent + 0x48));
				if (entity == lastentity) continue;
				else lastentity = entity;

				if (entity.HealthBase) {
					health_compo = SDK->RPM<health_compo_t>(entity.HealthBase);
					Vector2 healthext = SDK->RPM<Vector2>(entity.HealthBase + 0xF0);
					entity.PlayerHealth = health_compo.health + health_compo.armor + health_compo.barrier + healthext.Y;
					entity.PlayerHealthMax = health_compo.health_max + health_compo.armor_max + health_compo.barrier_max + healthext.X;
					entity.MinHealth = health_compo.health;
					entity.MaxHealth = health_compo.health_max;
					entity.MinArmorHealth = health_compo.armor;
					entity.MaxArmorHealth = health_compo.armor_max;
					entity.MinBarrierHealth = health_compo.barrier;
					entity.MaxBarrierHealth = health_compo.barrier_max;
					entity.Alive = (entity.PlayerHealth > 0.f) ? true : false;
					entity.imort = health_compo.isImmortal;
					entity.barrprot = health_compo.isBarrierProjected;
					//printf("health %f,health address:%llx\n", entity.PlayerHealth, entity.HealthBase);
				}
				else continue;
				if (entity.RotationBase) {
					entity.Rot = SDK->RPM<Vector3>(SDK->RPM<uint64_t>(entity.RotationBase + 0x7C0 + 0x10) + 0x8FC);
				}
				if (entity.VelocityBase) {
					velo_compo = SDK->RPM<velocity_compo_t>(entity.VelocityBase);

					entity.pos = Vector3(velo_compo.location.x, velo_compo.location.y - 1.f, velo_compo.location.z);
					entity.velocity = Vector3(velo_compo.velocity.x, velo_compo.velocity.y, velo_compo.velocity.z);
					int head_index = entity.GetSkel()[0];
					int neck_index = entity.GetSkel()[1];
					int chest_index = entity.GetSkel()[2];
					entity.head_pos = entity.GetBonePos(head_index);
					//if (entity.head_pos == lastpos) continue;
					//else lastpos = entity.head_pos;
					entity.neck_pos = entity.GetBonePos(neck_index);
					entity.chest_pos = entity.GetBonePos(chest_index);
				}
				if (entity.HeroBase) {
					hero_compo = SDK->RPM<hero_compo_t>(entity.HeroBase);
					entity.HeroID = hero_compo.heroid;
					if (entity.HeroID == eHero::HERO_WRECKINGBALL) {
						int head_index = entity.GetSkel()[0];
						int neck_index = entity.GetSkel()[1];
						int chest_index = entity.GetSkel()[2];
						entity.head_pos = entity.GetBonePos(head_index);
						//这里弄高了0.02
						entity.head_pos.Y += 0.02;
						//if (entity.head_pos == lastpos) continue;
						//else lastpos = entity.head_pos;
						entity.neck_pos = entity.GetBonePos(neck_index);
						entity.chest_pos = entity.GetBonePos(chest_index);
					}
					if (entity.HeroID == eHero::HERO_DVA && GetHeroEngNames(entity.HeroID, entity.LinkBase) != u8"Hana") {
						entity.imort = false;
						entity.head_pos.Y -= 0.1;
						entity.chest_pos = entity.neck_pos;
						entity.chest_pos.Y -= 0.3;
					}
					if (entity.HeroID == eHero::HERO_TRAININGBOT1 || entity.HeroID == eHero::HERO_TRAININGBOT2 || entity.HeroID == eHero::HERO_TRAININGBOT3 || entity.HeroID == eHero::HERO_TRAININGBOT4 || entity.HeroID == eHero::HERO_TRAININGBOT5 || entity.HeroID == eHero::HERO_TRAININGBOT6 || entity.HeroID == eHero::HERO_TRAININGBOT7)
						entity.chest_pos = entity.GetBonePos(83);
				}
				else {
					if (entity.MaxHealth == 225) {
						XMFLOAT3 temppos = SDK->RPM<XMFLOAT3>(entity.VelocityBase + 0x380 + 0x50);
						entity.head_pos = Vector3(temppos.x, temppos.y + 1, temppos.z);
						entity.HeroID = 0x16dd;
						entity.neck_pos = entity.head_pos;
						entity.chest_pos = entity.head_pos;
						entity.pos = entity.neck_pos;
					}
					else if (entity.MaxHealth == 30) {
						XMFLOAT3 temppos = SDK->RPM<XMFLOAT3>(entity.VelocityBase + 0x380 + 0x50);
						entity.head_pos = Vector3(temppos.x, temppos.y, temppos.z);
						entity.HeroID = 0x16ee;
						entity.neck_pos = entity.head_pos;
						entity.chest_pos = entity.head_pos;
						entity.pos = entity.neck_pos;
					}
					else if (entity.MaxHealth == 1000) {
						entity.HeroID = 0x16bb;
					}
					else continue;
				}
				if (Config::draw_info && Config::drawbattletag) {
					entity.statcombase = DecryptComponent(LinkParent, TYPE_STAT);
					if (entity.statcombase) {
						char buffer[64] = u8"";
						if (entity != local_entity) {
							uintptr_t off = SDK->RPM<uintptr_t>(entity.statcombase + 0xE0);
							SDK->read_buf(off, buffer, sizeof(char) * 64);
							entity.battletag = buffer;
						}
					}
				}
				if (entity.TeamBase) {
					entity.Team = (entity.GetTeam() == eTeam::TEAM_DEATHMATCH ||
						entity.GetTeam() != local_entity.GetTeam()) ? true : false;
				}
				if (entity.VisBase) {
					vis_compo = SDK->RPM<vis_compo_t>(entity.VisBase);
					entity.Vis = (DecryptVis(vis_compo.key1) ^ vis_compo.key2) ? true : false;
					//printf("%llx\n", SDK->RPM<uint64_t>(entity.VisBase + 0xa0));
				}
				if (entity.SkillBase) {
					entity.skill1act = IsSkillActive(entity.SkillBase + 0x40, 0, 0x28E3);
					entity.skill2act = IsSkillActive(entity.SkillBase + 0x40, 0, 0x28E9);
					entity.ultimate = readult(entity.SkillBase + 0x40, 0, 0x1e32);

					if (entity.HeroID == eHero::HERO_SOMBRA && entity.Team && !Config::Rage && !Config::fov360 && !Config::silent && !Config::fakesilent) {
						entity.Vis = (entity.Vis && !IsSkillActivate1(entity.SkillBase + 0x40, 0, 0x7C5));
					}
					//entity.skillcd1 = readskillcd(entity.SkillBase, 0, 0x189c);
					//entity.skillcd2 = readskillcd(entity.SkillBase, 0, 0x1f89);
					//std::cout << entity.skillcd1 << " " << entity.skillcd2 << std::endl;

					//if (entity.skill1act) {
					//	std::cout << GetHeroNames(entity.HeroID, entity.LinkBase).c_str() << " shift on.\n";
						//ImGui::GetForegroundDrawList()->AddText(ImVec2(WX, WY/2), ImGui::ColorConvertFloat4ToU32(ImVec4(255 / 255.0, 255 / 255.0, 255 / 255.0, 255 / 255.0)), GetHeroNames(entity.HeroID, entity.LinkBase).c_str());

					//}
					//if (entity.skill2act) {
					//	std::cout << GetHeroNames(entity.HeroID, entity.LinkBase).c_str() << " E on.\n";
					//}
					//if (entity.skill1act|| entity.skill2act) std::cout << 1 << std::endl;
					//if (IsSkillActive(entity.SkillBase, 0, 0x4BF))std::cout<<1<<std::endl;
					//else {} //std::cout << 0 << std::endl;
					//std::cout << ow_entities.size() << std::endl;
				}

				if (entity.OutlineBase)
				{
					//SetBorderLine(0x2, entity.OutlineBase);
					if (Config::externaloutline && !entity.Vis)	SetBorderLine(0x2, entity.OutlineBase);
					else SetBorderLine(0x1, entity.OutlineBase);
					if (Config::externaloutline) {
						if (entity.Team && i != Config::Targetenemyi && !Config::healthoutline && !Config::rainbowoutline) {
							//printf("%llx\n",SDK->RPM<uint32_t>(entity.OutlineBase + 0x130) );
							if (entity.Vis) {
								Config::visenemy = convertToHex(Config::enargb);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::visenemy);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::visenemy);
							}
							else if (i != Config::Targetenemyi) {
								Config::invisenemy = convertToHex(Config::invisenargb);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::invisenemy);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::invisenemy);
							}
						}
						
						else if (entity.Team && i != Config::Targetenemyi && Config::healthoutline && !Config::rainbowoutline) {
							if (entity.PlayerHealth == entity.MaxHealth) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(0.2, 0.8, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(0.2, 0.8, 0.2, 1)));
							}
							else if (entity.PlayerHealth / entity.MaxHealth >= 0.7) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(0.2, 1, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(0.2, 1, 0.2, 1)));
							}
							else if (entity.PlayerHealth / entity.MaxHealth >= 0.3 && entity.PlayerHealth / entity.MaxHealth < 0.7) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(1, 0.8, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(1, 0.8, 0.2, 1)));
							}
							else if (entity.PlayerHealth / entity.MaxHealth > 0 && entity.PlayerHealth / entity.MaxHealth < 0.3) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(1, 0, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(1, 0, 0.2, 1)));
							}
						}
						else if (entity.Team && i != Config::Targetenemyi && !Config::healthoutline && Config::rainbowoutline) {
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(Config::rainbowargb));
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(Config::rainbowargb));
							if (Config::cps1 == 0) Config::rainbowargb.x += 0.0002;
							else if (Config::cps1 == 1) Config::rainbowargb.x -= 0.0003;
							if (Config::cps2 == 0) Config::rainbowargb.y += 0.0005;
							else if (Config::cps2 == 1) Config::rainbowargb.y -= 0.0002;
							if (Config::cps3 == 0) Config::rainbowargb.z += 0.0003;
							else if (Config::cps3 == 1) Config::rainbowargb.z -= 0.0005;
							if (Config::rainbowargb.x <= 0) Config::cps1 = 0;
							else if (Config::rainbowargb.x >= 1) Config::cps1 = 1;
							if (Config::rainbowargb.y <= 0) Config::cps2 = 0;
							else if (Config::rainbowargb.y >= 1) Config::cps2 = 1;
							if (Config::rainbowargb.z <= 0) Config::cps3 = 0;
							else if (Config::rainbowargb.z >= 1) Config::cps3 = 1;
						}
						if (!entity.Team) {
							Config::Allycolor = convertToHex(Config::allyargb);
							//printf("%llx\n", Config::Allycolor);
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::Allycolor);
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::Allycolor);

						}
					}
					else if (Config::teamoutline && !entity.Team) {
						if (!entity.Vis) SetBorderLine(0x2, entity.OutlineBase);
						else SetBorderLine(0x1, entity.OutlineBase);
						Config::Allycolor = convertToHex(Config::allyargb);
						//printf("%llx\n", Config::Allycolor);
						SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::Allycolor);
						SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::Allycolor);
					}
				}

				//const auto angle_component = DecryptComponent(LinkParent, TYPE_PLAYERCONTROLLER);
				if (entity.AngleBase)
				{
					float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(entity.head_pos);
					/*printf("x:%f\n", viewMatrix_xor.get_location().x);
					printf("y:%f\n", viewMatrix_xor.get_location().y);
					printf("z:%f\n", viewMatrix_xor.get_location().z);*/
					if (dist <= 1 && GetHeroEngNames(entity.HeroID, entity.LinkBase) != skCrypt(u8"Unknown").decrypt()) {
						entity.skillcd1 = readskillcd(entity.SkillBase + 0x40, 0, 0x189c);
						//entity.skillready = cdready(entity.SkillBase+0x50, 0, 0x189c);
						entity.skillcd2 = readskillcd(entity.SkillBase + 0x40, 0, 0x1f89);
						local_entity = entity;
						Config::reloading = IsSkillActivate1(local_entity.SkillBase + 0x40, 0, 0x4BF);

						//std::cout << GetHeroNames(entity.HeroID, entity.LinkBase).c_str() << " shift CD:" << SkillCD(entity.SkillBase, 0, 0x189C) << "\n";
						//std::cout << GetHeroNames(entity.HeroID, entity.LinkBase).c_str() << " E CD." << SkillCD(entity.SkillBase, 0, 0x1F89) << "\n";
						SDK->g_player_controller = entity.AngleBase;
						//printf("%llx\n", SDK->g_player_controller);
						if (local_entity.GetTeam() == eTeam::TEAM_DEATHMATCH) entity.Team = false;
						//	printf("%f\n", SDK->RPM<float>(SDK->g_player_controller + 0x1760));
						//float spectuurtu = SDK->RPM<float>(SDK->g_player_controller + 0x178C);
						//SDK->WPM<float>(SDK->g_player_controller + 0x178C, -100);
					}
					//printf("d4a %d\n", SDK->RPM<uint16>(SDK->g_player_controller + 0xd4a));
					//printf("d4c %d\n", SDK->RPM<uint16>(SDK->g_player_controller + 0xd4c));
				}
				//printf("%llx\n", entity.HeroID);
				if (ComponentParent && LinkParent && GetHeroNames(entity.HeroID, entity.LinkBase) != skCrypt(u8"未知").decrypt())
					tmp_entities.push_back(entity);
			}
			//entities = tmp_entities;
			//mutex.lock();
			entities = tmp_entities;
			hp_dy_entities = hpdy_entities;
			//mutex.unlock();
			Sleep(3);
			//Sleep(5);
		}
    }
}
