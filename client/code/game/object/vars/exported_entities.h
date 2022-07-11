#pragma once

namespace jc::vars
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
		{ 21,	"blk_bosstank.ee" },
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
		{ 44,	"mon_supertroop.ee" },
		{ 45,	"mon_biketrooper.ee" },
		{ 46,	"pol_policeman.ee" },
		{ 47,	"pol_motorcycle_police.ee" },
		{ 48,	"pol_pilot.ee" },
		{ 49,	"pol_prisonguard.ee" },
		{ 50,	"pol_rangercommissioner.ee" },
		{ 51,	"pol_rangerpolice.ee" },
		{ 52,	"pol_swatpolice.ee" },
		{ 53,	"rio_bodyguard.ee" },
		{ 54,	"rio_hitman.ee" },
		{ 55,	"rio_quarterboss.ee" },
		{ 56,	"rio_cleaner.ee" },

		// civilians

		{ 57,	"civ_beachgirlprostitute.ee" },
		{ 58,	"civ_beachgirlprostitute2.ee" },
		{ 59,	"civ_beachman.ee" },
		{ 60,	"civ_beachshowoff.ee" },
		{ 61,	"civ_bigmoderncivilian.ee" },
		{ 62,	"civ_bigtownlationocivilian.ee" },
		{ 63,	"civ_bikinigirl.ee" },
		{ 64,	"civ_bikinigirl2.ee" },
		{ 65,	"civ_bum.ee" },
		{ 66,	"civ_drunk.ee" },
		{ 67,	"civ_fisherman.ee" },
		{ 68,	"civ_guerillacivilian.ee" },
		{ 69,	"civ_indian.ee" },
		{ 70,	"civ_jogger.ee" },
		{ 71,	"civ_limodriver.ee" },
		{ 72,	"civ_mariachi.ee" },
		{ 73,	"civ_mecanic.ee" },
		{ 74,	"civ_medic.ee" },
		{ 75,	"civ_military_prostitute.ee" },
		{ 76,	"civ_moderncivilianagent.ee" },
		{ 77,	"civ_motorbiker.ee" },
		{ 78,	"civ_nun.ee" },
		{ 79,	"civ_oldcivilianfemale.ee" },
		{ 80,	"civ_oldcivilianmale.ee" },
		{ 81,	"civ_party1.ee" },
		{ 82,	"civ_party2.ee" },
		{ 83,	"civ_party3.ee" },
		{ 84,	"civ_party4.ee" },
		{ 85,	"civ_party5.ee" },
		{ 86,	"civ_party6.ee" },
		{ 87,	"civ_pilot.ee" },
		{ 88,	"civ_redneck1.ee" },
		{ 89,	"civ_redneck2.ee" },
		{ 90,	"civ_redneck3.ee" },
		{ 91,	"civ_redneck4.ee" },
		{ 92,	"civ_redneck5.ee" },
		{ 93,	"civ_redneck6.ee" },
		{ 94,	"civ_redneck7.ee" },
		{ 95,	"civ_riot1.ee" },
		{ 96,	"civ_riot2.ee" },
		{ 97,	"civ_robber.ee" },
		{ 98,	"civ_salesman.ee" },
		{ 99,	"civ_tourist.ee" },
		{ 100,	"civ_towncivilian.ee" },
		{ 101,	"civ_towncivilian2.ee" },
		{ 102,	"civ_urbancivilian.ee" },
		{ 103,	"civ_villagecivilian.ee" },
		{ 104,	"civ_villagecivilian2.ee" },
		{ 105,	"civ_waiter.ee" },
		{ 106,	"civ_worker.ee" },
		{ 107,	"civ_busdriver.ee" },
		{ 108,	"civ_harvester_coca.ee" },
		{ 109,	"civ_prisoner.ee" },
		{ 110,	"civ_mopeder.ee" },

		// missions

		{ 111,	"m04_mon_gunner.ee" },
		{ 112,	"m05_boathunters_captain.ee" },
		{ 113,	"m09_helicopter_mon.ee" },
		{ 114,	"m1_cia_fighterpilot.ee" },
		{ 115,	"m1_mil_mountedgunner.ee" },
		{ 116,	"m1_truckDriver.ee" },
		{ 117,	"m1_truckDriver2.ee" },
		{ 118,	"m1_boatdriver_mil_heavysoldier.ee" },
		{ 119,	"m1_helicopter_mil_heavysoldier.ee" },
		{ 121,	"m1_jeepdriver_mil_heavysoldier.ee" },
		{ 121,	"a1m06_convoy_driver_limo.ee" },
		{ 123,	"a1m06_convoy_firstcar_driver.ee" },
		{ 124,	"a1m06_convoy_lastcar_driver.ee" },
		{ 125,	"a2m07_chaser.ee" },
		{ 126,	"a2m07_ezperanza.ee" },
		{ 127,	"a2m07_gunrunners_driver.ee" },
		{ 128,	"a2m07_mountedgunner.ee" },
		{ 129,	"a2m08_blk_pilot_bad_shooter.ee" },
		{ 130,	"a2m14_driver_first.ee" },
		{ 131,	"a2m14_driver_last.ee" },
		{ 132,	"a2m14_mg_first.ee" },
		{ 133,	"a2m14_mg_last.ee" },
		{ 134,	"m08_blk_boatcaptain.ee" },
		{ 135,	"m08_boatchaser_blk_heavysoldier.ee" },
		{ 136,	"m08_boatgunner_blk_heavysoldier.ee" },
		{ 137,	"m10_lightsoldier_driver.ee" },
		{ 138,	"m11_boat_captain.ee" },
		{ 139,	"m12_mil_pilot_bad.ee" },
		{ 140,	"a2m07_ezperanza_crossroads.ee" },
		{ 141,	"a2m07_gunrunners_grenadier.ee" },
		{ 142,	"a2m07_gunrunners_grenadier2.ee" },
		{ 143,	"a2m07_gunrunners_shooter.ee" },
		{ 144,	"a2m07_sanchez.ee" },
		{ 145,	"a2m07_shitface.ee" },
		{ 146,	"a2m08_mon_cleaner.ee" },
		{ 147,	"m17_engineer.ee" },
		{ 148,	"m19_blk_fighterpilot.ee" },

		// races

		{ 149,	"rac_bookmaker_01.ee" },
		{ 150,	"rac_bookmaker_02.ee" },
		{ 151,	"rac_bookmaker_03.ee" },
		{ 152,	"rac_bookmaker_04.ee" },
		{ 153,	"rac_bookmaker_05.ee" },
	};

	inline std::unordered_map<uint32_t, std::string> exported_entities_vehicles =
	{
		{ 0, R"(Exported\Vehicles\Land_Vehicles\LAVE_001_Compact_Car_1.ee)" }, // Baxter Petit
		{ 1, R"(Exported\Vehicles\Land_Vehicles\LAVE_002_normal_car_1.ee)" }, // Chevalier Streetbird de luxe
		{ 2, R"(Exported\Vehicles\Land_Vehicles\LAVE_003_Normal_Car_2.ee)" }, // Scando 700 Sedan
		{ 3, R"(Exported\Vehicles\Land_Vehicles\LAVE_004_Van.ee)" }, // Cutler Bullhorn 
		{ 4, R"(Exported\Vehicles\Land_Vehicles\lave_004_van_black.ee)" }, // Cutler Bullhorn
		{ 5, R"(Exported\Vehicles\Land_Vehicles\LAVE_005_Luxuary_Car_1.ee)" }, // Rossi 866 Corral 		
		{ 6, R"(Exported\Vehicles\Land_Vehicles\LAVE_006_BigJeep.ee)" }, // Emvee Armadillo
		{ 7, R"(Exported\Vehicles\Land_Vehicles\A1M1\Sheldons_Jeep.ee)" }, // Emvee Armadillo
		{ 8, R"(Exported\Vehicles\Land_Vehicles\LAVE_006_BigJeep_1_MG_front.ee)" }, // Emvee Armadillo
		{ 9, R"(Exported\Vehicles\Land_Vehicles\LAVE_006_BigJeep_1_MG_back.ee)" }, // Emvee Armadillo
		{ 10, R"(Exported\Vehicles\Land_Vehicles\LAVE_007_Jeep2.ee)" }, // Huerta Mesa
		{ 11, R"(Exported\Vehicles\Land_Vehicles\LAVE_008_Pickup.ee)" }, // Shimizu Tumbleweed
		{ 12, R"(Exported\Vehicles\Land_Vehicles\LAVE_008_Pickup_FMG_G.ee)" }, // Shimizu Tumbleweed
		{ 13, R"(Exported\Vehicles\Land_Vehicles\LAVE_008_Pickup_BMG_G.ee)" }, // Shimizu Tumbleweed
		{ 14, R"(Exported\Vehicles\Land_Vehicles\LAVE_009_truck_medium_1.ee)" }, // Vanderbildt Route 66
		{ 15, R"(Exported\Vehicles\Land_Vehicles\LAVE_010_ATV.ee)" }, // Fukuda Buckskin Trapper
		{ 16, R"(Exported\Vehicles\Land_Vehicles\LAVE_011_Motorbike.ee)" }, // Yamada 37 14 Vaquero
		{ 17, R"(Exported\Vehicles\Land_Vehicles\lave_011_motorbikecivil.ee)" }, // Yamada 37 14 Vaquero
		{ 18, R"(Exported\Vehicles\Land_Vehicles\LAVE_012_Motorcycle_2.ee)" }, // Victory Bellevue
		{ 19, R"(Exported\Vehicles\Land_Vehicles\LAVE_013_Armored_transport.ee)" }, // Meister ATV 4
		{ 20, R"(Exported\Vehicles\Land_Vehicles\lave_013_sidem_transport.ee)" }, // Meister ATV 4
		{ 21, R"(Exported\Vehicles\Land_Vehicles\LAVE_014_tank.ee)" }, // Ballard M5B1 Scout
		{ 22, R"(Exported\Vehicles\Land_Vehicles\LAVE_014_tank_AA.ee)" }, // Ballard Centronel AAWV-21 
		{ 23, R"(Exported\Vehicles\Land_Vehicles\LAVE_014_tank_Rocket.ee)" }, // Ballard Sentry STRL-14 
		{ 24, R"(Exported\Vehicles\Land_Vehicles\LAVE_017_normal_car_4.ee)" }, // Potomac Silverbullet
		{ 25, R"(Exported\Vehicles\Land_Vehicles\LAVE_018_normal_car_5.ee)" }, // Cutler Randall Arrowhead
		{ 26, R"(Exported\Vehicles\Land_Vehicles\LAVE_019_limosine.ee)" }, // Fusilier Commander
		{ 27, R"(Exported\Vehicles\Land_Vehicles\LAVE_020_pickup_2.ee)" }, // Europ
		{ 28, R"(Exported\Vehicles\Land_Vehicles\LAVE_021_Truck_medium_2.ee)" }, // Battaille GPT 6
		{ 29, R"(Exported\Vehicles\Land_Vehicles\LAVE_022_Bulldozer.ee)" }, // Scando Track Loader Extreme
		{ 30, R"(Exported\Vehicles\Land_Vehicles\LAVE_024_compactcar_2.ee)" }, // Reichsmobil Compact Mark II
		{ 31, R"(Exported\Vehicles\Land_Vehicles\lave_025_normalcar_6.ee)" }, // Yamada Redwood SUV
		{ 32, R"(Exported\Vehicles\Land_Vehicles\LAVE_026_Normal_Car_7.ee)" }, // Hurst Dagger
		{ 33, R"(Exported\Vehicles\Land_Vehicles\LAVE_027_luxuary_car_2.ee)" }, // Chevalier Supernova
		{ 34, R"(Exported\Vehicles\Land_Vehicles\LAVE_028_Jeep_3.ee)" }, // Wallys GP
		{ 35, R"(Exported\Vehicles\Land_Vehicles\LAVE_028_Jeep_3MGF.ee)" }, // Wallys GP
		{ 36, R"(Exported\Vehicles\Land_Vehicles\LAVE_028_Jeep_3_MGB.ee)" }, // Wallys GP
		{ 37, R"(Exported\Vehicles\Land_Vehicles\LAVE_029_Compact_Car_3.ee)" }, // Squire Synco
		{ 38, R"(Exported\Vehicles\Land_Vehicles\LAVE_030_Pickup_3.ee)" }, // Reichsmobil Transport
		{ 39, R"(Exported\Vehicles\Land_Vehicles\LAVE_031_Pickup_4.ee)" }, // Hurst Buckaroo
		{ 40, R"(Exported\Vehicles\Land_Vehicles\LAVE_031_Pickup_4_FMG.ee)" }, // Hurst Buckaroo
		{ 41, R"(Exported\Vehicles\Land_Vehicles\LAVE_031_Pickup_4_BMG.ee)" }, // Hurst Buckaroo
		{ 41, R"(Exported\Vehicles\Land_Vehicles\LAVE_032_Truck_medium_3.ee)" }, // Meyer Compact T
		{ 42, R"(Exported\Vehicles\Land_Vehicles\LAVE_032_Truck_medium_3_MG.ee)" }, // Meyer Compact T
		{ 43, R"(Exported\Vehicles\Land_Vehicles\LAVE_033_Motorcycle_3.ee)" }, // Schulz Las Vegas
		{ 44, R"(Exported\Vehicles\Land_Vehicles\LAVE_034_motorcycle_4.ee)" }, // Mosca 125 Rallye
		{ 45, R"(Exported\Vehicles\Land_Vehicles\LAVE_035_Normal_car_8.ee)" }, // Cutler Randall Conquistador
		{ 46, R"(Exported\Vehicles\Land_Vehicles\LAVE_037_Military_truck.ee)" }, // Meister LAV 4
		{ 47, R"(Exported\Vehicles\Land_Vehicles\LAVE_039_Police_car.ee)" }, // Vaultier Sedan Patrol Compact
		{ 48, R"(Exported\Vehicles\Land_Vehicles\LAVE_040_police_van.ee)" }, // Vaultier ALEV Patrol Special
		{ 49, R"(Exported\Vehicles\Land_Vehicles\LAVE_041_ambulance.ee)" }, // MacNamara Emergency III
		{ 50, R"(Exported\Vehicles\Land_Vehicles\LAVE_042_Bus.ee)" }, // Pankhurst Bus Tap Tap
		{ 51, R"(Exported\Vehicles\Land_Vehicles\LAVE_043_Rally_Car.ee)" }, // Fukuda Tournament
		{ 52, R"(Exported\Vehicles\Land_Vehicles\LAVE_044_Monster_Truck.ee)" }, // The Vampire
		{ 53, R"(Exported\Vehicles\Land_Vehicles\LAVE_044_Monster_Truck_MG.ee)" }, // The Vampire
		{ 54, R"(Exported\Vehicles\Land_Vehicles\LAVE_045_James_Bond_Car.ee)" }, // Garret Paladin
		{ 55, R"(Exported\Vehicles\Land_Vehicles\LAVE_046_Military_buggy.ee)" }, // Stinger Buggy
		{ 56, R"(Exported\Vehicles\Land_Vehicles\LAVE_046_Civilian_buggy.ee)" }, // Stinger Buggy
		{ 57, R"(Exported\Vehicles\Land_Vehicles\LAVE_048_Taxi.ee)" }, // Taxi
		{ 58, R"(Exported\Vehicles\Land_Vehicles\LAVE_050_service_vehicle.ee)" }, // AVIA Airport service vehicle
		{ 59, R"(Exported\Vehicles\Land_Vehicles\LAVE 051 LAVE_051_Military_offroad_bike.ee)" }, // Apache Army Model 842 
		{ 60, R"(Exported\Vehicles\Land_Vehicles\LAVE_052_military_6_wheel_ATV.ee)" }, // Rotor industries ATRV
		{ 61, R"(Exported\Vehicles\Land_Vehicles\LAVE_053_Racingbike.ee)" }, // Fukuda Bluebolt
		{ 62, R"(Exported\Vehicles\Land_Vehicles\LAVE_054_Agency_Jeep.ee)" }, // GP Thunder Extreme Prototype
		{ 63, R"(Exported\Vehicles\Land_Vehicles\LAVE_055_Tank_3_Rocket.ee)" }, // Harlan OTWV 2 TOW
		{ 64, R"(Exported\Vehicles\Land_Vehicles\LAVE_055_tank_AA.ee)" }, // Harlan OTWV 2 AAA
		{ 65, R"(Exported\Vehicles\Land_Vehicles\LAVE_055_Tank_3.ee)" }, // Harlan OTWV 2 CAN
		{ 66, R"(Exported\Vehicles\Land_Vehicles\LAVE_056_Bus_2.ee)" }, // Vanderbildt Streamliner
		{ 67, R"(exported\vehicles\air_vehicles\arve_001_civilchopper)" }, // Mullen H45 Dragonfly
		{ 68, R"(exported\vehicles\air_vehicles\arve_002_attack_helicopter.ee)" }, // Jackson Z-19 Skreemer
		{ 69, R"(exported\vehicles\air_vehicles\arve_003_transport_helicopter.ee)" }, // HH 22 Savior
		{ 69, R"(exported\vehicles\air_vehicles\arve_003_rocket_helicopter.ee)" }, // HH 22 Savior
		{ 70, R"(exported\vehicles\air_vehicles\arve_005_fighter_jet.ee)" }, // Rage Johnston F6 Comet
		{ 71, R"(exported\vehicles\air_vehicles\arve_006_propeller_plane.ee)" }, // Huerta SPA Ocelot
		{ 72, R"(exported\vehicles\air_vehicles\ARVE_006_CropDuster.ee)" }, // Huerta SPA Ocelot
		{ 73, R"(exported\vehicles\air_vehicles\arve_007_military_helicopter.ee)" }, // Delta 5H4 Boxhead
		{ 74, R"(exported\vehicles\air_vehicles\arve_008_cargo_plane.ee)" }, // Alexander AX 14
		{ 75, R"(exported\vehicles\air_vehicles\arve_009_millitary_heli_3.ee)" }, // Jackson JC 2 Alamo
		{ 76, R"(exported\vehicles\air_vehicles\arve_010_learjet.ee)" }, // Stirling Jet Executive
		{ 77, R"(exported\vehicles\air_vehicles\ARVE_012_police_helicopter.ee)" }, // Huertaz PAS1 Aztek
		{ 78, R"(exported\vehicles\air_vehicles\arve_013_fighter_jet_2.ee)" }, // Rage Johnston F10 Wraith
		{ 79, R"(exported\vehicles\air_vehicles\ARVE_014_Civilian_Plane.ee)" }, // Stirling STL 3 Octavio
		{ 80, R"(exported\vehicles\air_vehicles\ARVE_015_Military_Heli_2.ee)" }, // Fukuda MH Ronin
		{ 81, R"(exported\vehicles\air_vehicles\arve_016_large_attack_helikopter.ee)" }, // Novak Walker AH 16 Hammerbolt
		{ 82, R"(exported\vehicles\air_vehicles\ARVE_017_presidential_plane.ee)" }, // The Excelsior, El Grande
		{ 83, R"(exported\vehicles\air_vehicles\arve_018_propellerplane.ee)" }, // Walker PA Twin Fin
		{ 84, R"(exported\vehicles\air_vehicles\arve_020_special_attack_heli.ee)" }, // Delta MAH 15 Chimaera
		{ 85, R"(exported\vehicles\air_vehicles\arve_022_old_school_fighter.ee)" }, // McKenzie Fergusson F2 Victor
		{ 86, R"(Exported\Vehicles\Air_Vehicles\ARVE_023_Gyrocopter.ee)" }, // Whiptail Gyrocopter
		{ 87, R"(exported\vehicles\air_vehicles\arve_025_fighter_jet_3.ee)" }, // Ulysses-McCoy Redcloud
		{ 88, R"(exported\vehicles\sea_vehicles\SEVE_001_Water_Scooter.ee)" }, // Shimizu Mach 2
		{ 89, R"(exported\vehicles\sea_vehicles\seve_002_patrolboat_medium.ee)" }, // Triton Patroller
		{ 90, R"(exported\vehicles\sea_vehicles\seve_004_kittyhawk.ee)" }, // Triton Hornet
		{ 91, R"(exported\vehicles\sea_vehicles\seve_005_speedboat.ee)" }, // Triton - G3 Taiphoon
		{ 92, R"(exported\vehicles\sea_vehicles\SEVE_006_Yacht.ee)" }, // Miami Sport Wave +
		{ 93, R"(exported\vehicles\sea_vehicles\SEVE_007_underwater_scooter.ee)" }, // Shimizu Scuba boy
		{ 94, R"(exported\vehicles\sea_vehicles\SEVE_008_patrol_boat_large.ee)" }, // Triton Broadsider
		{ 95, R"(exported\vehicles\sea_vehicles\SEVE_008_patrol_boat_largeFGL.ee)" }, // Triton Broadsider
		{ 96, R"(exported\vehicles\sea_vehicles\SEVE_008_patrol_boatlargeBMG.ee)" }, // Triton Broadsider
		{ 97, R"(exported\vehicles\sea_vehicles\SEVE_010_water_scooter.ee)" }, // Shimizu Nayad 700			
		{ 98, R"(exported\vehicles\sea_vehicles\seve_011_patrolboat_medbmg_2.ee)" }, // Pequod Harpoon PC 350
		{ 99, R"(exported\vehicles\sea_vehicles\seve_011_patrolboat_mediumBI_2.ee)" }, // Pequod Harpoon PC 350
		{ 100, R"(exported\vehicles\sea_vehicles\SEVE_011_patrolboat_medium_2.ee)" }, // Pequod Harpoon PC 350
		{ 101, R"(exported\vehicles\sea_vehicles\seve_013_speedboat1.ee)" }, // Viper boatworks Seaserpent III +
		{ 102, R"(exported\vehicles\sea_vehicles\seve_014_speedboat_2.ee)" }, // Triereme - RC Athena +
		{ 103, R"(exported\vehicles\sea_vehicles\SEVE_014_speedboat_2MG.ee)" }, // Triereme - RC Athena
		{ 104, R"(exported\vehicles\sea_vehicles\seve_015_yacht2.ee)" }, // Royal 500 - great white +
		{ 105, R"(exported\vehicles\sea_vehicles\seve_018_sailing_boat_2.ee)" }, // Bosphorus boatworks - Patna exclusive +
		{ 106, R"(exported\vehicles\sea_vehicles\seve_020_zodiac.ee)" }, // Jaeger 5FJ 7
		{ 107, R"(exported\vehicles\sea_vehicles\seve_021_fishingboat_special.ee)" }, // Fishing boat
		{ 108, R"(exported\vehicles\sea_vehicles\seve_022_fishing_boat.ee)" }, // Fishing boat
		{ 109, R"(Exported\Vehicles\Sea_Vehicles\SEVE_023_speedboat_special.ee)" }, // Bald Eagle Persuader agency boat at Agency 2 safehouse
	};
}