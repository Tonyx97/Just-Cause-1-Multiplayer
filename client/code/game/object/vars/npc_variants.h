#pragma once

namespace jc::vars::npc_variants
{
	inline std::unordered_map<uint32_t, std::string> head_skins =
	{
		// male heads

		{ 0,	"m_npc_head2.dds" },
		{ 1,	"m_npc_head4.dds" },
		{ 2,	"m_npc_head5.dds" },
		{ 3,	"m_npc_head6.dds" },
		{ 4,	"m_npc_head7.dds" },
		{ 5,	"m_npc_head8.dds" },

		// female heads

		{ 6,	"f_npc_head2.dds" },
		{ 7,	"f_npc_head3.dds" },
		{ 8,	"f_npc_head4.dds" },
	};

	inline std::unordered_map<uint32_t, std::string> cloth_skins =
	{
		// male cloth skins

		{ 0,	"m1_npc_021_gursoldier.dds" },
		{ 1,	"m1_npc_027_urbciv.dds" },
		{ 2,	"m1_npc_028_vilciv.dds" },
		{ 3,	"m1_npc_033_police.dds" },
		{ 4,	"m1_npc_039_gurciv.dds" },
		{ 5,	"m1_npc_043_waiter.dds" },
		{ 6,	"m1_npc_044_worker.dds" },
		{ 7,	"m2_npc_005_rioja.dds" },
		{ 8,	"m2_npc_029_modcivagent.dds" },
		{ 9,	"m2_npc_045_soldier.dds" },
		{ 10,	"m2_npc_046_mariachi.dds" },
		{ 11,	"m3_npc_030_bigmodciv.dds" },
		{ 12,	"m3_npc_047_medic.dds" },
		{ 13,	"m3_npc_048_tourist.dds" },
		{ 14,	"m4_npc_051_beachman.dds" },
		{ 15,	"m4_npc_052_nativeindian.dds" },
		{ 16,	"m5_npc_009_blackhand.dds" },
		{ 17,	"m5_npc_018_swat.dds" },
		{ 18,	"m5_npc_055_motorcycler.dds" },
		{ 19,	"m5_npc_056_elitesoldier.dds" },
		{ 20,	"npc_montano.dds" },
		{ 21,	"npc_rioja.dds" },

		// female cloth skins

		{ 22,	"f1_npc_031_prostitute.dds" },
		{ 23,	"f1_npc_038_bikinigirl.dds" },
		{ 24,	"f2_npc_034_townciv.dds" },
		{ 25,	"f2_npc_049_nun.dds" },
		{ 26,	"f2_npc_050_indian.dds" },
	};

	inline std::unordered_map<uint32_t, std::string> props =
	{
		{ 0,	"prop_afrohair.lod" },
		{ 1,	"prop_bandana.lod" },
		{ 2,	"prop_beachglasses.lod" },
		{ 3,	"prop_beltbag.lod" },
		{ 4,	"prop_beret.lod" },
		{ 5,	"prop_blackgun.lod" },
		{ 6,	"prop_blackhelmet.lod" },
		{ 7,	"prop_blacklegbag.lod" },
		{ 8,	"prop_blacksunglasses.lod" },
		{ 9,	"prop_blindfold.lod" },
		{ 10,	"prop_bottle.lod" },
		{ 11,	"prop_briefcasecloth.lod" },
		{ 12,	"prop_brownglasses.lod" },
		{ 13,	"prop_browngun.lod" },
		{ 14,	"prop_bumhat.lod" },
		{ 15,	"prop_camera.lod" },
		{ 16,	"prop_camohat.lod" },
		{ 17,	"prop_capbackward.lod" },
		{ 18,	"prop_capcamo.lod" },
		{ 19,	"prop_capgreen.lod" },
		{ 20,	"prop_capred.lod" },
		{ 21,	"prop_cigar.lod" },
		{ 22,	"prop_cigarette.lod" },
		{ 23,	"prop_civilianbackpack.lod" },
		{ 24,	"prop_clearglasses.lod" },
		{ 25,	"prop_cowboyhat.lod" },
		{ 26,	"prop_elitebackpack.lod" },
		{ 27,	"prop_fishingrod.lod" },
		{ 28,	"prop_gangstahat.lod" },
		{ 29,	"prop_greengun.lod" },
		{ 30,	"prop_greenlegbag.lod" },
		{ 31,	"prop_hairball.lod" },
		{ 32,	"prop_handcigarette.lod" },
		{ 33,	"prop_headphones.lod" },
		{ 34,	"prop_headset.lod" },
		{ 35,	"prop_indianbackpack.lod" },
		{ 36,	"prop_indianmenshat.lod" },
		{ 37,	"prop_indianwomenshat.lod" },
		{ 38,	"prop_leatherbackpack.lod" },
		{ 39,	"prop_machete.lod" },
		{ 40,	"prop_mariachihat.lod" },
		{ 41,	"prop_mask.lod" },
		{ 42,	"prop_militarybackpack.lod" },
		{ 43,	"prop_militaryhelmet.lod" },
		{ 44,	"prop_mohawk.lod" },
		{ 45,	"prop_mopedhelmetblue.lod" },
		{ 46,	"prop_mopedhelmetred.lod" },
		{ 47,	"prop_mopedhelmetyellow.lod" },
		{ 48,	"prop_motorcyclehelmet.lod" },
		{ 49,	"prop_nosebone.lod" },
		{ 50,	"prop_nunhat.lod" },
		{ 51,	"prop_officershat.lod" },
		{ 52,	"prop_orangeglasses.lod" },
		{ 53,	"prop_pilotglasses.lod" },
		{ 54,	"prop_pilothelmet.lod" },
		{ 55,	"prop_policehat.lod" },
		{ 56,	"prop_policehelmet.lod" },
		{ 57,	"prop_ponytailblack.lod" },
		{ 58,	"prop_ponytailgrey.lod" },
		{ 59,	"prop_rangerhat.lod" },
		{ 60,	"prop_riothelmet.lod" },
		{ 61,	"prop_scarf.lod" },
		{ 62,	"prop_strawhat.lod" },
		{ 63,	"prop_tatooleftlow.lod" },
		{ 64,	"prop_tatooleftup.lod" },
		{ 65,	"prop_tatoorightlow.lod" },
		{ 66,	"prop_tatoorightup.lod" },
		{ 67,	"prop_tofsblond.lod" },
		{ 68,	"prop_tofsbrown.lod" },
		{ 69,	"prop_utilitybagleather.lod" },
		{ 70,	"prop_utilitybeltbeige.lod" },
		{ 71,	"prop_utilitybeltbig.lod" },
		{ 72,	"prop_utilitybeltgreen.lod" },
		{ 73,	"prop_waistbottle.lod" },
		{ 74,	"prop_workerhelmet.lod" },
	};

	inline std::unordered_map<uint32_t, std::string> prop_locs =
	{
		{ 0,	"left_ankle" },
		{ 1,	"right_ankle" },
		{ 2,	"left_wrist" },
		{ 3,	"right_wrist" },
		{ 4,	"left_elbow" },
		{ 5,	"right_elbow" },
		{ 6,	"left_upperarm" },
		{ 7,	"right_upperarm" },
		{ 8,	"left_clavicle" },
		{ 9,	"right_clavicle" },
		{ 10,	"pelvis" },
		{ 11,	"spine_1" },
		{ 12,	"neck" },
		{ 13,	"head" },
		{ 14,	"spine_2" },
		{ 15,	"back_2" },
		{ 16,	"a_2" },
		{ 17,	"a_1" },
		{ 18,	"attach_left" },
		{ 19,	"attach_right" },
	};

	inline std::unordered_map<uint32_t, std::string> cloth_color =
	{
		{ 0,	"yellow.bmp" },
		{ 1,	"guerilla_green.bmp" },
	};
}