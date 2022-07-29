#pragma once

using WeaponID = uint8_t;

enum WeaponSlot
{
	WeaponSlot_A,
	WeaponSlot_B,
	WeaponSlot_C,
	WeaponSlot_D,
	WeaponSlot_E,
	WeaponSlot_F,
	WeaponSlot_G,
	WeaponSlot_H,
	WeaponSlot_I,
	WeaponSlot_J,
};

enum WeaponID_ : WeaponID
{
	Weapon_None,
	Weapon_Pistol = 1, // MORETTI P-94 | Pistol
	Weapon_Assault_Rifle = 2, // ADLER FF M-72 | Assault Rifle
	Weapon_Shotgun_pump_action = 3, // MORETTI CCW CENTURION | Shotgun (pump action)
	Weapon_M60 = 4, // M60 | M60
	Weapon_Mounted_M60 = 5, // MOUNTED M60 | Mounted M60
	Weapon_Rocket_Launcher = 6, // MAKO OCRA | Rocket Launcher
	Weapon_Helicopter_gun = 7, // HELICOPTER GUN | Helicopter gun
	Weapon_Tank_Cannon = 8, // TANK CANNON | Tank Cannon
	Weapon_Signature_Gun = 9, // HOLDT R4 PITBULLS | Signature Gun
	Weapon_Master_Signature_Gun = 10, // HOLDT R4 PITBULLS | Master Signature Gun
	Weapon_Silenced_Pistol = 11, // GALLINI M39 "HUSHER" | Silenced Pistol
	Weapon_Revolver = 12, // HARKER 357 SAWBACK | Revolver
	Weapon_1H_SMG = 13, // AVIV SCARAB | 1H SMG
	Weapon_Silenced_SMG = 14, // NOVA 9 | Silenced SMG
	Weapon_2H_SMG = 15, // AVIV PANTHERA | 2H SMG
	Weapon_Sawed_off_shotgun = 16, // DAWSON 610 | Sawed off shotgun
	Weapon_Grenade_Launcher = 17, // MAKO HAMMERHEAD | Grenade Launcher
	Weapon_Disposable_RPG = 18, // RPG M-112 | Disposable RPG
	Weapon_Sniper_rifle = 19, // BARCLAY PHANTOM | Sniper rifle
	Weapon_Assault_rifle_sniper = 20, // LANCE FDL | Assault rifle (sniper)
	Weapon_Assault_Rifle_heavy = 21, // FLEISHER AR-5 BLIZZARD | Assault Rifle (heavy)
	Weapon_Assault_Rifle_high_tech = 22, // HASWELL GEN2 VINDICATOR | Assault Rifle (high tech)
	Weapon_Shotgun_automatic = 23, // MORETTI CCW PRAETORIAN | Shotgun (automatic)
	Weapon_Boat_Gun = 24, // BOAT GUN | Boat Gun
	Weapon_AAGun = 25, // AAGUN | AAGun
	Weapon_LAVE_055_Tank_Cannon = 26, // LAVE_055 TANK CANNON | LAVE_055 Tank Cannon
	Weapon_LAVE_014_Tank_Cannon = 27, // LAVE_014 TANK CANNON | LAVE_014 Tank Cannon
	Weapon_FFAR_Rockets = 28, // FFAR ROCKETS | FFAR Rockets
	Weapon_Hellfire = 29, // HELLFIRE | Hellfire
	Weapon_SIDEWINDER = 30, // SIDEWINDER | SIDEWINDER
	Weapon_AIRPLANE_GUNS = 31, // AIRPLANE_GUNS | AIRPLANE_GUNS
	Weapon_LAVE_055_AAGUN = 32, // LAVE_055_AAGUN | LAVE_055_AAGUN
	Weapon_LAVE_055_ROCKETS = 33, // LAVE_055_ROCKETS | LAVE_055_ROCKETS
	Weapon_Sheldons_M60 = 34, // SHELDONS M60 | Sheldons M60
	Weapon_LAVE_014_ROCKETS = 35, // LAVE_014_ROCKETS | LAVE_014_ROCKETS
	Weapon_LAVE_014_AAGUN = 36, // LAVE_014_AAGUN | LAVE_014_AAGUN
	Weapon_Grapplinghook = 37, // PROTEC GRAPPLER G3 | Grapplinghook
	Weapon_Car_guns = 38, // CAR GUNS | Car guns
	Weapon_Boat_cannon = 39, // BOAT CANNON | Boat cannon
	Weapon_Mounted_GL = 40, // MOUNTED GL | Mounted GL
	Weapon_Timed_Explosive = 41, // TIMED EXPLOSIVES | Timed Explosive
	Weapon_Triggered_Explosive = 42, // TRIGGERED EXPLOSIVE | Triggered Explosive
	Weapon_Remote_Trigger = 43, // REMOTE TRIGGER | Remote Trigger
};

namespace jc::vars
{
	inline const std::unordered_map<WeaponID, std::string> weapons_id_to_type_name =
	{
		{ Weapon_Pistol, "Pistol" },
		{ Weapon_Assault_Rifle, "Assault Rifle" },
		{ Weapon_Shotgun_pump_action, "Shotgun (pump action)" },
		{ Weapon_M60, "M60" },
		{ Weapon_Mounted_M60, "Mounted M60" },
		{ Weapon_Rocket_Launcher, "Rocket Launcher" },
		{ Weapon_Helicopter_gun, "Helicopter gun" },
		{ Weapon_Tank_Cannon, "Tank Cannon" },
		{ Weapon_Signature_Gun, "Signature Gun" },
		{ Weapon_Master_Signature_Gun, "Master Signature Gun" },
		{ Weapon_Silenced_Pistol, "Silenced Pistol" },
		{ Weapon_Revolver, "Revolver" },
		{ Weapon_1H_SMG, "1H SMG" },
		{ Weapon_Silenced_SMG, "Silenced SMG" },
		{ Weapon_2H_SMG, "2H SMG" },
		{ Weapon_Sawed_off_shotgun, "Sawed off shotgun" },
		{ Weapon_Grenade_Launcher, "Grenade Launcher" },
		{ Weapon_Disposable_RPG, "Disposable RPG" },
		{ Weapon_Sniper_rifle, "Sniper rifle" },
		{ Weapon_Assault_rifle_sniper, "Assault rifle (sniper)" },
		{ Weapon_Assault_Rifle_heavy, "Assault Rifle (heavy)" },
		{ Weapon_Assault_Rifle_high_tech, "Assault Rifle (high tech)" },
		{ Weapon_Shotgun_automatic, "Shotgun (automatic)" },
		{ Weapon_Boat_Gun, "Boat Gun" },
		{ Weapon_AAGun, "AAGun" },
		{ Weapon_LAVE_055_Tank_Cannon, "LAVE_055 Tank Cannon" },
		{ Weapon_LAVE_014_Tank_Cannon, "LAVE_014 Tank Cannon" },
		{ Weapon_FFAR_Rockets, "FFAR Rockets" },
		{ Weapon_Hellfire, "Hellfire" },
		{ Weapon_SIDEWINDER, "SIDEWINDER" },
		{ Weapon_AIRPLANE_GUNS, "AIRPLANE_GUNS" },
		{ Weapon_LAVE_055_AAGUN, "LAVE_055_AAGUN" },
		{ Weapon_LAVE_055_ROCKETS, "LAVE_055_ROCKETS" },
		{ Weapon_Sheldons_M60, "Sheldons M60" },
		{ Weapon_LAVE_014_ROCKETS, "LAVE_014_ROCKETS" },
		{ Weapon_LAVE_014_AAGUN, "LAVE_014_AAGUN" },
		{ Weapon_Grapplinghook, "Grapplinghook" },
		{ Weapon_Car_guns, "Car guns" },
		{ Weapon_Boat_cannon, "Boat cannon" },
		{ Weapon_Mounted_GL, "Mounted GL" },
		{ Weapon_Timed_Explosive, "Timed Explosive" },
		{ Weapon_Triggered_Explosive, "Triggered Explosive" },
		{ Weapon_Remote_Trigger, "Remote Trigger" },
	};

	inline const std::unordered_map<WeaponID, std::string> weapons_id_to_model_name =
	{
		{ Weapon_Pistol, R"(Weapons\WEAP_026_lod4.rbm)" },
		{ Weapon_Assault_Rifle, R"(Weapons\WEAP_006_lod4.rbm)" },
		{ Weapon_Shotgun_pump_action, R"(Weapons\WEAP_009_lod4.rbm)" },
		{ Weapon_Rocket_Launcher, R"(Weapons\WEAP_014_lod4.rbm)" },
		{ Weapon_Signature_Gun, R"(Weapons\WEAP_001_lod4.rbm)" },
		{ Weapon_Master_Signature_Gun, R"(Weapons\WEAP_001_lod4.rbm)" },
		{ Weapon_Silenced_Pistol, R"(Weapons\WEAP_027_lod4.rbm)" },
		{ Weapon_Revolver, R"(Weapons\WEAP_020_lod4.rbm)" },
		{ Weapon_1H_SMG, R"(Weapons\WEAP_028_lod4.rbm)" },
		{ Weapon_Silenced_SMG, R"(Weapons\WEAP_002_lod4.rbm)" },
		{ Weapon_2H_SMG, R"(Weapons\WEAP_022_lod4.rbm)" },
		{ Weapon_Sawed_off_shotgun, R"(Weapons\WEAP_016_lod4.rbm)" },
		{ Weapon_Grenade_Launcher, R"(Weapons\WEAP_017_lod4.rbm)" },
		{ Weapon_Disposable_RPG, R"(Weapons\WEAP_015_lod4.rbm)" },
		{ Weapon_Sniper_rifle, R"(Weapons\WEAP_012_lod4.rbm)" },
		{ Weapon_Assault_rifle_sniper, R"(Weapons\WEAP_008_lod4.rbm)" },
		{ Weapon_Assault_Rifle_heavy, R"(Weapons\WEAP_013_lod4.rbm)" },
		{ Weapon_Assault_Rifle_high_tech, R"(Weapons\WEAP_007_lod4.rbm)" },
		{ Weapon_Shotgun_automatic, R"(Weapons\WEAP_010_lod4.rbm)" },
		{ Weapon_Grapplinghook, R"(Weapons\WEAP_000_lod4.rbm)" },
		{ Weapon_Timed_Explosive, R"(weapons\weap_040.lod)" },
		{ Weapon_Triggered_Explosive, R"(weapons\weap_041.lod)" },
		{ Weapon_Remote_Trigger, R"(Weapons\WEAP_042.rbm)" },
	};

	inline const std::unordered_map<WeaponID, int32_t> weapons_id_to_type =
	{
		{ Weapon_Pistol, 0 },
		{ Weapon_Assault_Rifle, 2 },
		{ Weapon_Shotgun_pump_action, 2 },
		{ Weapon_M60, 4 },
		{ Weapon_Mounted_M60, 4 },
		{ Weapon_Rocket_Launcher, 2 },
		{ Weapon_Helicopter_gun, 4 },
		{ Weapon_Tank_Cannon, 4 },
		{ Weapon_Signature_Gun, 3 },
		{ Weapon_Master_Signature_Gun, 3 },
		{ Weapon_Silenced_Pistol, 0 },
		{ Weapon_Revolver, 0 },
		{ Weapon_1H_SMG, 1 },
		{ Weapon_Silenced_SMG, 1 },
		{ Weapon_2H_SMG, 1 },
		{ Weapon_Sawed_off_shotgun, 1 },
		{ Weapon_Grenade_Launcher, 2 },
		{ Weapon_Disposable_RPG, 1 },
		{ Weapon_Sniper_rifle, 2 },
		{ Weapon_Assault_rifle_sniper, 2 },
		{ Weapon_Assault_Rifle_heavy, 2 },
		{ Weapon_Assault_Rifle_high_tech, 2 },
		{ Weapon_Shotgun_automatic, 2 },
		{ Weapon_Boat_Gun, 4 },
		{ Weapon_AAGun, 4 },
		{ Weapon_LAVE_055_Tank_Cannon, 4 },
		{ Weapon_LAVE_014_Tank_Cannon, 4 },
		{ Weapon_FFAR_Rockets, 4 },
		{ Weapon_Hellfire, 4 },
		{ Weapon_SIDEWINDER, 4 },
		{ Weapon_AIRPLANE_GUNS, 4 },
		{ Weapon_LAVE_055_AAGUN, 4 },
		{ Weapon_LAVE_055_ROCKETS, 4 },
		{ Weapon_Sheldons_M60, 4 },
		{ Weapon_LAVE_014_ROCKETS, 4 },
		{ Weapon_LAVE_014_AAGUN, 4 },
		{ Weapon_Grapplinghook, 9 },
		{ Weapon_Car_guns, 4 },
		{ Weapon_Boat_cannon, 4 },
		{ Weapon_Mounted_GL, 2 },
		{ Weapon_Timed_Explosive, 6 },
		{ Weapon_Triggered_Explosive, 7 },
		{ Weapon_Remote_Trigger, 8 },
	};
}