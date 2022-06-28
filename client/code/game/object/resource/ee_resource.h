#pragma once

#include "resource.h"

namespace jc::ee_resource
{
	static constexpr uint32_t EXPORTED_ENTITY_RESOURCE = 0x118;

	static constexpr uint32_t INSTANCE_SIZE = 0x120;

	namespace fn
	{
		static constexpr uint32_t SETUP					= 0x761700;
		static constexpr uint32_t SET_FLAG				= 0x5C23C0;
		static constexpr uint32_t CHECK					= 0x5C17B0;
		static constexpr uint32_t FINISH_LOAD			= 0x7618A0;
		static constexpr uint32_t MARK_AS_LOADED		= 0x761840;
		static constexpr uint32_t LOADING_ERROR			= 0x761880;
	}

	namespace vars
	{
		inline std::unordered_map<uint32_t, std::string> exported_entities =
		{
			// key characters

			{ 0,	"key_caramicas.ee" },
			{ 1,	"key_carmona.ee" },
			{ 2,	"key_chiefvelasco.ee" },
			{ 3,	"key_ezperanza.ee" },
			{ 4,	"key_immaculada.ee" },
			{ 5,	"key_immaculada_dress.ee" },
			{ 6,	"key_kane.ee" },
			{ 7,	"key_kane_bikini.ee" },
			{ 8,	"key_kleiner.ee" },
			{ 9,	"key_montalban.ee" },
			{ 10,	"key_sheldon.ee" },
			{ 11,	"key_benito.ee" },
			{ 12,	"key_coloneldurango_woman.ee" },
			{ 13,	"key_presidents_son1.ee" },
			{ 14,	"key_presidents_son2.ee" },
			{ 15,	"key_roberto.ee" },

			// factions

			{ 16,	"blk_pilot.ee" },
			{ 17,	"blk_specialforce.ee" },
			{ 18,	"blk_squadleader.ee" },
			{ 19,	"blk_tankdriver.ee" },
			{ 20,	"blk_trooper.ee" },
			{ 21,	"blk_trooper2.ee" },
			{ 22,	"gur_leader.ee" },
			{ 23,	"gur_pilot.ee" },
			{ 24,	"gur_sniper.ee" },
			{ 25,	"gur_soldier.ee" },
			{ 26,	"lib_gur_grenadier.ee" },
			{ 27,	"lib_gur_leader.ee" },
			{ 28,	"lib_gur_soldier.ee" },
			{ 29,	"lib_gur_veteran.ee" },
			{ 30,	"mil_drunk_soldier.ee" },
			{ 31,	"mil_elitesoldier.ee" },
			{ 32,	"mil_heavysoldier.ee" },
			{ 33,	"mil_lightsoldier.ee" },
			{ 34,	"mil_mountedgunner.ee" },
			{ 35,	"mil_sniper.ee" },
			{ 36,	"mil_supersoldier.ee" },
			{ 37,	"mon_bodyguard.ee" },
			{ 38,	"mon_cleaner.ee" },
			{ 39,	"mon_druglord.ee" },
			{ 40,	"mon_hitman.ee" },
			{ 41,	"mon_member.ee" },
			{ 42,	"mon_quarterboss.ee" },
			{ 43,	"mon_rpg.ee" },
			{ 44,	"pol_policeman.ee" },
			{ 45,	"pol_prisonguard.ee" },
			{ 46,	"pol_rangercommissioner.ee" },
			{ 47,	"pol_rangerpolice.ee" },
			{ 48,	"pol_swatpolice.ee" },
			{ 49,	"rio_bodyguard.ee" },
			{ 50,	"rio_hitman.ee" },
			{ 51,	"rio_quarterboss.ee" },

			// races

			{ 52,	"rac_bookmaker_01.ee" },
			{ 53,	"rac_bookmaker_03.ee" },
			{ 54,	"rac_bookmaker_05.ee" },

			// missions

			{ 55,	"a2m07_ezperanza_crossroads.ee" },
			{ 56,	"a2m07_gunrunners_grenadier.ee" },
			{ 57,	"a2m07_gunrunners_grenadier2.ee" },
			{ 58,	"a2m07_gunrunners_shooter.ee" },
			{ 59,	"a2m07_sanchez.ee" },
			{ 60,	"a2m07_shitface.ee" },
			{ 61,	"a2m08_mon_cleaner.ee" },
			{ 62,	"m17_engineer.ee" },
			{ 63,	"m19_blk_fighterpilot.ee" },
		};
	}
}

class ExportedEntity;

class ExportedEntityResource : public Resource
{
public:

	static ExportedEntityResource* CREATE();

	static constexpr auto SIZE() { return jc::ee_resource::INSTANCE_SIZE; }

	void set_flag(uint32_t flag);

	bool check() const;
	bool is_loaded() const;

	ExportedEntity* get_exported_entity() const;
};