// Created with ReClass.NET 1.2 by KN4CK3R

#include "ida_base_structs.hpp"

struct ray
{
	vec3 origin,
		 direction;

	ray(const vec3& origin, const vec3& direction)
		: origin(origin)
		, direction(glm::normalize(direction))
	{
	}
};

struct ray_hit_info
{
	void* object = nullptr,
		* unk = nullptr;

	vec3 normal;

	float distance_factor;
};

class Weapon;

class _AmmoManager
{
public:
	class AmmoManager *ammo_manager; //0x0000
}; //Size: 0x0004

class AmmoManager
{
public:
	char pad_0000[112]; //0x0000
	void *bullet_list; //0x0070
	char pad_0074[16]; //0x0074
	int32_t max_ammo_slotA; //0x0084
	int32_t max_ammo_slotB; //0x0088
	int32_t max_ammo_slotC; //0x008C
	int32_t max_ammo_slotD; //0x0090
	int32_t max_ammo_slotE; //0x0094
	int32_t max_ammo_slotF; //0x0098
	int32_t max_ammo_slotG; //0x009C
	int32_t max_ammo_slotH; //0x00A0
	char pad_00A4[28]; //0x00A4
}; //Size: 0x00C0

class Bullet
{
public:
	char pad_0000[12]; //0x0000
	uint16_t damage; //0x000C
	char pad_000E[70]; //0x000E
	vec3 position; //0x0054
	char pad_0060[4]; //0x0060
	vec3 direction; //0x0064
	vec3 last_position; //0x0070
	char pad_007C[24]; //0x007C
	float max_range; //0x0094
	char pad_0098[6]; //0x0098
	bool alive; //0x009E
	char pad_009F[41]; //0x009F
}; //Size: 0x00C8

class Camera
{
public:
	char pad_0000[20]; //0x0000
	mat4 model_matrix; //0x0014
	mat4 proj_matrix; //0x0054
	mat4 view_matrix; //0x0094
	char pad_00D4[720]; //0x00D4
	int32_t width; //0x03A4
	int32_t height; //0x03A8
	char pad_03AC[268]; //0x03AC
}; //Size: 0x04B8

class _CameraManager
{
public:
	class CameraManager *camera_manager; //0x0000
}; //Size: 0x0004

class CameraManager
{
public:
	char pad_0000[8]; //0x0000
	class Camera *main_camera; //0x0008
	char pad_000C[20]; //0x000C
	mat4 model_matrix; //0x0020
	mat4 proj_matrix; //0x0060
	mat4 view_matrix; //0x00A0
	char pad_00E0[780]; //0x00E0
}; //Size: 0x03EC

class ObjectBase
{
public:
	char pad_0000[12]; //0x0000
}; //Size: 0x000C

class ObjectEventManager
{
public:
	char pad_0000[20]; //0x0000
}; //Size: 0x0014

class ObjectDestructor
{
public:
	char pad_0000[16]; //0x0000
}; //Size: 0x0010

class AliveObject
{
public:
	char pad_0000[24]; //0x0000
	float max_health; //0x0018
	float health; //0x001C
	uint16_t f1; //0x0020
	uint16_t f2; //0x0022
	uint16_t f3; //0x0024
	uint16_t alive_flags; //0x0026
}; //Size: 0x0028

class Skeleton
{
public:
	char pad_0000[608]; //0x0000
	void *bone_list; //0x0260
	char pad_0264[48]; //0x0264
	class Transform (*transform_list)[20]; //0x0294
	char pad_0298[12]; //0x0298
	class Unk *unk; //0x02A4
	class SkeletonInstance *N000011D4; //0x02A8
	class SkeletonInstance *N000011CE; //0x02AC
	class SkeletonInstance *N000011CF; //0x02B0
	class SkeletonInstance *sk; //0x02B4
	char pad_02B8[40]; //0x02B8
	class Transform *head_transform; //0x02E0
	char pad_02E4[4]; //0x02E4
	class Transform *stomach_transform; //0x02E8
	char pad_02EC[8]; //0x02EC
	class Transform *pelvis_transform; //0x02F4
	char pad_02F8[224]; //0x02F8
	float head_yaw; //0x03D8
	float head_pitch; //0x03DC
	float head_roll; //0x03E0
	float head_interpolation_factor; //0x03E4
	float head_mover2; //0x03E8
	char pad_03EC[4]; //0x03EC
	vec4 head_rotation; //0x03F0
	char pad_0400[56]; //0x0400
}; //Size: 0x0438

class BodyStanceController
{
public:
	char pad_0000[36]; //0x0000
	int32_t movement_id; //0x0024
	char pad_0028[44]; //0x0028
}; //Size: 0x0054

class ArmsStanceController
{
public:
	char pad_0000[84]; //0x0000
}; //Size: 0x0054

class Transform
{
public:
	mat4 m; //0x0000
}; //Size: 0x0040

class CharacterHandleEntry
{
public:
	void *handle; //0x0000
	uint32_t id; //0x0004
}; //Size: 0x0008

class CharacterInfo
{
public:
	char pad_0000[240]; //0x0000
}; //Size: 0x00F0

class Character : public ObjectBase
{
public:
	class ObjectEventManager event_manager; //0x000C
	class ObjectDestructor destructor; //0x0020
	class AliveObject alive_info; //0x0030
	char pad_0058[56]; //0x0058
	int32_t ammo_grenades; //0x0090
	char pad_0094[68]; //0x0094
	class WeaponBelt *weapon_belt; //0x00D8
	class Skeleton skeleton; //0x00DC
	class BodyStanceController body_stance; //0x0514
	class ArmsStanceController arms_stance; //0x0568
	char pad_05BC[28]; //0x05BC
	class Transform transform; //0x05D8
	char pad_0618[320]; //0x0618
	mat4 head_transform; //0x0758
	char pad_0798[28]; //0x0798
	class Model *model; //0x07B4
	class Model *head_attachments; //0x07B8
	char pad_07BC[16]; //0x07BC
	class Model *some_model; //0x07CC
	char pad_07D0[76]; //0x07D0
	float punch_damage; //0x081C
	float punch_time; //0x0820
	vec3 aiming_target_position; //0x0824
	char pad_0830[32]; //0x0830
	class Physical *physical; //0x0850
	char pad_0854[28]; //0x0854
	int32_t closest_vehicle_count; //0x0870
	char pad_0874[4]; //0x0874
	class VehicleController *vehicle_controller; //0x0878
	char pad_087C[8]; //0x087C
	uint32_t flags; //0x0884
	char pad_0888[92]; //0x0888
	float death_time; //0x08E4
	char pad_08E8[4]; //0x08E8
	float air_time; //0x08EC
	char pad_08F0[36]; //0x08F0
	float grenade_timeout; //0x0914
	float grenade_time; //0x0918
	class CharacterHandleEntry handle_entry; //0x091C
	class CharacterInfo info; //0x0924
	float time_left_to_pray; //0x0A14
	float pray_interval; //0x0A18
	vec3 N00000AA2; //0x0A1C
	vec4 rotation_clamp; //0x0A28
	char pad_0A38[148]; //0x0A38
}; //Size: 0x0ACC

class WeaponSlot
{
public:
	char pad_0000[4]; //0x0000
	ref<Weapon> *weapon_ref; //0x0004
	char pad_0008[8]; //0x0008
}; //Size: 0x0010

class WeaponBelt
{
public:
	char pad_0000[8]; //0x0000
	int32_t ammo_slotA; //0x0008
	int32_t ammo_slotB; //0x000C
	int32_t ammo_slotC; //0x0010
	int32_t ammo_slotD; //0x0014
	int32_t ammo_slotE; //0x0018
	int32_t ammo_slotF; //0x001C
	int32_t ammo_slotG; //0x0020
	int32_t ammo_slotH; //0x0024
	char pad_0028[40]; //0x0028
	class WeaponSlot slot0; //0x0050
	class WeaponSlot slot1; //0x0060
	class WeaponSlot slot2; //0x0070
	class WeaponSlot slot3; //0x0080
	class WeaponSlot slot4; //0x0090
	class WeaponSlot slot5; //0x00A0
	class WeaponSlot slot6; //0x00B0
	class WeaponSlot slot7; //0x00C0
	class WeaponSlot slot8; //0x00D0
	class WeaponSlot slot9; //0x00E0
	uint16_t current_weapon_slot; //0x00F0
	char pad_00F2[2]; //0x00F2
	uint16_t draw_weapon_slot; //0x00F4
	uint16_t N00001DDC; //0x00F6
	char pad_00F8[4]; //0x00F8
	uint16_t N00000CE9; //0x00FC
	uint16_t N00001E09; //0x00FE
	uint16_t N00000CEA; //0x0100
	uint16_t N00001E0D; //0x0102
	uint16_t N00000CEB; //0x0104
	uint16_t N00001E0F; //0x0106
	uint16_t N000011F0; //0x0108
	uint16_t N00001E19; //0x010A
	uint16_t N000011F1; //0x010C
	uint16_t N00001E1B; //0x010E
	uint16_t N00000CEC; //0x0110
	uint16_t N00001E1D; //0x0112
	uint16_t N00000CED; //0x0114
	uint16_t N00001E22; //0x0116
	uint16_t N00000CEE; //0x0118
	uint16_t N00001E24; //0x011A
	uint16_t N00000CEF; //0x011C
	uint16_t N00001E26; //0x011E
	uint16_t N00000CF0; //0x0120
	uint16_t N00001E28; //0x0122
	char pad_0124[4]; //0x0124
}; //Size: 0x0128

class Weapon
{
public:
	char pad_0000[56]; //0x0000
	class Transform grip_transform; //0x0038
	class Character *character_owner; //0x0078
	char pad_007C[44]; //0x007C
	float N00000E1C; //0x00A8
	char pad_00AC[16]; //0x00AC
	void *fire_component; //0x00BC
	class Transform last_muzzle_transform; //0x00C0
	class Transform muzzle_transform; //0x0100
	class Transform last_ejector_transform; //0x0140
	char pad_0180[4]; //0x0180
	int32_t ammo; //0x0184
	bool can_fire; //0x0188
	bool N00000EB7; //0x0189
	bool reloading; //0x018A
	bool N00001E37; //0x018B
	float last_shot_time2; //0x018C
	char pad_0190[12]; //0x0190
	float last_shot_time; //0x019C
	vec3 aim_hit_position; //0x01A0
	class WeaponInfo *info; //0x01AC
	class Weapon *second_weapon; //0x01B0
	char pad_01B4[16]; //0x01B4
	int32_t total_bullets_fired; //0x01C4
	int32_t bullets_fired_while_firing; //0x01C8
	bool trigger_pulled; //0x01CC
	char pad_01CD[15]; //0x01CD
}; //Size: 0x01DC

class WeaponInfo
{
public:
	int32_t id; //0x0000
	int32_t icon_index; //0x0004
	int32_t anim_type; //0x0008
	int32_t type; //0x000C
	int32_t bullet_type; //0x0010
	int32_t model; //0x0014
	char pad_0018[24]; //0x0018
	float firerate; //0x0030
	float reload_time; //0x0034
	int32_t magazine_size; //0x0038
	float min_spread_angle; //0x003C
	float max_spread_angle; //0x0040
	int32_t bullets_per_shot; //0x0044
	float accuracy_ai; //0x0048
	float accuracy_player; //0x004C
	char pad_0050[56]; //0x0050
	int32_t master_weapon; //0x0088
	char pad_008C[24]; //0x008C
	vec3 fire_effect_position; //0x00A4
	char pad_00B0[32]; //0x00B0
	vec3 shell_effect_position; //0x00D0
	char pad_00DC[4]; //0x00DC
	int32_t shell_effect_id; //0x00E0
	char pad_00E4[24]; //0x00E4
	int32_t ammo_explosion; //0x00FC
	char pad_0100[24]; //0x0100
	int32_t weapon_sound_id; //0x0118
	int32_t fire_sound_id; //0x011C
	int32_t click_sound_id; //0x0120
	int32_t reload_sound_id; //0x0124
	float ammo_velocity; //0x0128
	float ammo_max_range; //0x012C
	float ammo_max_damage_ai; //0x0130
	float ammo_max_damage; //0x0134
	float ammo_area_damage_radius; //0x0138
	float max_range_ai; //0x013C
	float max_range; //0x0140
	float miss_min_distance; //0x0144
	float miss_max_distance; //0x0148
	float miss_shrink_dist; //0x014C
	float min_burst_bullet; //0x0150
	float max_burst_bullet; //0x0154
	float min_burst_delay; //0x0158
	float max_burst_delay; //0x015C
	float min_click_delay; //0x0160
	float max_click_delay; //0x0164
	float scope_min_fov; //0x0168
	float scope_max_fov; //0x016C
	float scope_shake_effect; //0x0170
	bool click_fire; //0x0174
	bool use_against_vehicles; //0x0175
	bool eject_shells_on_fire; //0x0176
	bool scope; //0x0177
	bool usable_in_vehicle; //0x0178
	bool silencer; //0x0179
	bool infinite_ammo; //0x017A
	bool play_recoil_on_release; //0x017B
}; //Size: 0x017C

class Physical
{
public:
	char pad_0000[12]; //0x0000
	class Character *userdata; //0x000C
	char pad_0010[20]; //0x0010
	float N00001338; //0x0024
	char pad_0028[8]; //0x0028
	vec3 velocity; //0x0030
	char pad_003C[4]; //0x003C
	vec4 N0000133F; //0x0040
	class hkCharacterProxy *proxy; //0x0050
	char pad_0054[4]; //0x0054
	class hkSimpleShapePhantom *shape_phantom; //0x0058
	char pad_005C[84]; //0x005C
	class hkCharacterVelocities *velocities; //0x00B0
	char pad_00B4[16]; //0x00B4
	float mass; //0x00C4
	char pad_00C8[8]; //0x00C8
}; //Size: 0x00D0

class hkMotionState
{
public:
	vec4 rot_col1; //0x0000
	vec4 rot_col2; //0x0010
	vec4 rot_col3; //0x0020
	vec4 translation; //0x0030
	char pad_0040[112]; //0x0040
	vec3 N00001600; //0x00B0
	char pad_00BC[4]; //0x00BC
	vec3 N00001603; //0x00C0
	char pad_00CC[72]; //0x00CC
}; //Size: 0x0114

class hkSimpleShapePhantom
{
public:
	char pad_0000[8]; //0x0000
	class hkWorld *hk_world; //0x0008
	char pad_000C[32]; //0x000C
	class hkShape *this_shape; //0x002C
	char pad_0030[80]; //0x0030
	class hkMotionState motion_state; //0x0080
	char pad_0194[448]; //0x0194
}; //Size: 0x0354

class hkWorld
{
public:
	char pad_0000[224]; //0x0000
	vec4 gravity; //0x00E0
	char pad_00F0[96]; //0x00F0
}; //Size: 0x0150

class hkShape
{
public:
	char pad_0000[4]; //0x0000
}; //Size: 0x0004

class VehicleController
{
public:
	char pad_0000[4]; //0x0000
	mat4 seat_transform; //0x0004
	char pad_0044[8]; //0x0044
	class Vehicle *current_vehicle; //0x004C
	char pad_0050[60]; //0x0050
}; //Size: 0x008C

class Vehicle
{
public:
	char pad_0000[72]; //0x0000
	float max_health; //0x0048
	float health; //0x004C
	char pad_0050[80]; //0x0050
	class VehicleSeat *driver_seat; //0x00A0
	class VehicleSeat *passenger_seat; //0x00A4
	char pad_00A8[4]; //0x00A8
	class VehicleSeat *special_seat; //0x00AC
	char pad_00B0[304]; //0x00B0
	float N0000173C; //0x01E0
	float N0000173D; //0x01E4
	vec3 camera_target_offset; //0x01E8
	char pad_01F4[12]; //0x01F4
	float camera_fov; //0x0200
	uint16_t N00001745; //0x0204
	uint16_t N000018E3; //0x0206
	char pad_0208[36]; //0x0208
	bool N0000174F; //0x022C
	bool N000018E6; //0x022D
	bool N000018E9; //0x022E
	bool N000018E7; //0x022F
	bool N00001750; //0x0230
	bool N00001992; //0x0231
	char pad_0232[446]; //0x0232
	class PfxInstance *pfx; //0x03F0
	char pad_03F4[212]; //0x03F4
	float N000017F7; //0x04C8
	char pad_04CC[4]; //0x04CC
	float N0000143A; //0x04D0
	char pad_04D4[20]; //0x04D4
	float N00001440; //0x04E8
	float N00001441; //0x04EC
	char pad_04F0[344]; //0x04F0
}; //Size: 0x0648

class _DayCycle
{
public:
	class DayCycle *day_cycle; //0x0000
}; //Size: 0x0004

class DayCycle
{
public:
	char pad_0000[4]; //0x0000
	int32_t hours_per_day; //0x0004
	int32_t days_per_year; //0x0008
	int32_t months_per_year; //0x000C
	int32_t year; //0x0010
	int32_t day; //0x0014
	float hour; //0x0018
	bool enabled; //0x001C
	char pad_001D[3]; //0x001D
}; //Size: 0x0020

class _Dropoff
{
public:
	class Dropoff *dropoff; //0x0000
}; //Size: 0x0004

class Dropoff
{
public:
	char pad_0000[208]; //0x0000
}; //Size: 0x00D0

class _GameControl
{
public:
	class GameControl *game_control; //0x0000
}; //Size: 0x0004

class GameControl
{
public:
	char pad_0000[328]; //0x0000
}; //Size: 0x0148

class _LightSystem
{
public:
	class LightSystem *light_system; //0x0000
}; //Size: 0x0004

class LightSystem
{
public:
	char pad_0000[16]; //0x0000
	void *light_list; //0x0010
	char pad_0014[120]; //0x0014
}; //Size: 0x008C

class LocalPlayer
{
public:
	char pad_0000[28]; //0x0000
	class Character *character; //0x001C
	char pad_0020[216]; //0x0020
	float right; //0x00F8
	float forward; //0x00FC
	char pad_0100[1084]; //0x0100
}; //Size: 0x053C

class _ParticleSystem
{
public:
	class ParticleSystem *particle_system; //0x0000
}; //Size: 0x0004

class ParticleSystem
{
public:
	char pad_0000[32]; //0x0000
	void *particle_list; //0x0020
	char pad_0024[224]; //0x0024
}; //Size: 0x0104

class _PhysicsSystem
{
public:
	class PhysicsSystem *physics_system; //0x0000
}; //Size: 0x0004

class PhysicsSystem
{
public:
	char pad_0000[44]; //0x0000
	vec3 world_position; //0x002C
	char pad_0038[12]; //0x0038
	void *collision_cache; //0x0044
	char pad_0048[4]; //0x0048
	class hkWorld *hk_world; //0x004C
	char pad_0050[188]; //0x0050
}; //Size: 0x010C

class _Renderer
{
public:
	class Renderer *renderer; //0x0000
}; //Size: 0x0004

class Renderer
{
public:
	char pad_0000[1096]; //0x0000
	void *dx9_device; //0x0448
	char pad_044C[184]; //0x044C
}; //Size: 0x0504

class _SettingsManager
{
public:
	class SettingsManager *settings_manager; //0x0000
}; //Size: 0x0004

class SettingsManager
{
public:
	char pad_0000[452]; //0x0000
	float fx_volume; //0x01C4
	float music_volume; //0x01C8
	float dialogue_volume; //0x01CC
	uint32_t motion_blur_enabled; //0x01D0
	uint32_t action_camera; //0x01D4
	uint32_t subtitles; //0x01D8
	uint32_t invert_up_down; //0x01DC
	char pad_01E0[4]; //0x01E0
	uint32_t show_health_bar; //0x01E4
	uint32_t show_weapon_selector; //0x01E8
	uint32_t show_objective_info; //0x01EC
	uint32_t show_minimap; //0x01F0
	uint32_t show_action_icons; //0x01F4
	char pad_01F8[8]; //0x01F8
	uint32_t screen_resolution; //0x0200
	uint32_t heat_haze_enabled; //0x0204
	uint32_t water_quality; //0x0208
	uint32_t terrain_detail_quality; //0x020C
	uint32_t scene_complexity; //0x0210
	uint32_t texture_resolution; //0x0214
	uint32_t pfx_enabled; //0x0218
	char pad_021C[384]; //0x021C
}; //Size: 0x039C

class _SoundSystem
{
public:
	class SoundSystem *sound_system; //0x0000
}; //Size: 0x0004

class SoundSystem
{
public:
	char pad_0000[64]; //0x0000
	class SoundBank *hud_bank; //0x0040
	char pad_0044[12]; //0x0044
	class SoundBank *ambience_bank; //0x0050
	class SoundBank *explosion_bank; //0x0054
	class SoundBank *weapon_bank; //0x0058
	char pad_005C[84]; //0x005C
}; //Size: 0x00B0

class SoundBank
{
public:
	char pad_0000[40]; //0x0000
}; //Size: 0x0028

class _SpawnManager
{
public:
	class SpawnManager *spawn_manager; //0x0000
}; //Size: 0x0004

class SpawnManager
{
public:
	char pad_0000[60]; //0x0000
	int16_t max_characters; //0x003C
	int16_t max_vehicles; //0x003E
	char pad_0040[164]; //0x0040
}; //Size: 0x00E4

class _StatSystem
{
public:
	class StatSystem *stat_system; //0x0000
}; //Size: 0x0004

class StatSystem
{
public:
	char pad_0000[112]; //0x0000
	uint32_t civilians_killed; //0x0070
	char pad_0074[8]; //0x0074
	uint32_t soldiers_killed; //0x007C
	uint32_t police_killed; //0x0080
	uint32_t guerrilla_killed; //0x0084
	uint32_t blackhand_killed; //0x0088
	uint32_t montano_killed; //0x008C
	char pad_0090[28]; //0x0090
	uint32_t total_shots_fired; //0x00AC
	uint32_t story_missions_completed; //0x00B0
	uint32_t side_missions_completed; //0x00B4
	uint32_t roof_to_roof_stunts; //0x00B8
	uint32_t parachute_to_roof_stunts; //0x00BC
	uint32_t rioja_settlements; //0x00C0
	uint32_t guerrilla_settlements; //0x00C4
	char pad_00C8[4]; //0x00C8
	float highest_base_jump; //0x00CC
	uint32_t total_game_time; //0x00D0
	uint32_t session_time; //0x00D4
	uint32_t agency_safehouses; //0x00D8
	uint32_t guerrillaa_safehouses; //0x00DC
	uint32_t rioja_safehouses; //0x00E0
	char pad_00E4[12]; //0x00E4
}; //Size: 0x00F0

class _TimeSystem
{
public:
	class TimeSystem *time_system; //0x0000
}; //Size: 0x0004

class TimeInfo
{
public:
	char pad_0000[8]; //0x0000
	float fps; //0x0008
	float delta; //0x000C
	float last_fps; //0x0010
	float last_delta; //0x0014
	float time_scale; //0x0018
}; //Size: 0x001C

class TimeSystem
{
public:
	char pad_0000[4]; //0x0000
	class TimeInfo info; //0x0004
	char pad_0020[48]; //0x0020
}; //Size: 0x0050

class _VehicleManager
{
public:
	class VehicleManager *vehicle_manager; //0x0000
}; //Size: 0x0004

class VehicleManager
{
public:
	char pad_0000[16]; //0x0000
	void *vehicle_list; //0x0010
	char pad_0014[24]; //0x0014
}; //Size: 0x002C

class _World
{
public:
	class World *world; //0x0000
}; //Size: 0x0004

class World
{
public:
	char pad_0000[32]; //0x0000
	void *character_list; //0x0020
	char pad_0024[12]; //0x0024
	class LocalPlayer *localplayer; //0x0030
	char pad_0034[4]; //0x0034
}; //Size: 0x0038

class ForcePulse
{
public:
	char pad_0000[48]; //0x0000
	vec3 position; //0x0030
	vec3 radius; //0x003C
	mat4 transform; //0x0048
	char pad_0088[28]; //0x0088
	float force; //0x00A4
	float damage; //0x00A8
	char pad_00AC[4]; //0x00AC
}; //Size: 0x00B0

class _CameraShake
{
public:
	class CameraShake *camera_shake; //0x0000
}; //Size: 0x0004

class CameraShake
{
public:
	char pad_0000[4]; //0x0000
	uint32_t type; //0x0004
	char pad_0008[28]; //0x0008
	bool cinematic; //0x0024
	bool update; //0x0025
	char pad_0026[14]; //0x0026
	class CameraTODO1 *some_weird_cam; //0x0034
	char pad_0038[32]; //0x0038
}; //Size: 0x0058

class _WeaponSystem
{
public:
	class WeaponSystem *weapon_system; //0x0000
}; //Size: 0x0004

class WeaponSystem
{
public:
	char pad_0000[8]; //0x0000
	class WeaponTemplateList *template_list; //0x0008
	char pad_000C[8]; //0x000C
}; //Size: 0x0014

class WeakPtr
{
public:
	void *obj; //0x0000
	class WeakPtrReference *ref; //0x0004
}; //Size: 0x0008

class WeakPtrReference
{
public:
	char pad_0000[4]; //0x0000
	int32_t count; //0x0004
	int32_t count2; //0x0008
	char pad_000C[4]; //0x000C
	void *obj; //0x0010
	char pad_0014[4]; //0x0014
}; //Size: 0x0018

class _GrenadeList
{
public:
	class PlayerGrenade *first_grenade; //0x0000
	char pad_0004[36]; //0x0004
}; //Size: 0x0028

class PlayerGrenade
{
public:
	char pad_0000[88]; //0x0000
	mat4 transform; //0x0058
	char pad_0098[216]; //0x0098
	class PfxInstance *pfx; //0x0170
	char pad_0174[296]; //0x0174
	float current_time; //0x029C
	char pad_02A0[4]; //0x02A0
	class Character *character_owner; //0x02A4
	char pad_02A8[4]; //0x02A8
	void *smoke_fx; //0x02AC
	char pad_02B0[32]; //0x02B0
}; //Size: 0x02D0

class PfxInstance
{
public:
	char pad_0000[16]; //0x0000
	class pfxFloatingRigidBody *rigidbody; //0x0010
	char pad_0014[164]; //0x0014
	class hkDefaultAnalogDriverInput *analog_input; //0x00B8
	char pad_00BC[44]; //0x00BC
}; //Size: 0x00E8

class pfxFloatingRigidBody
{
public:
	char pad_0000[60]; //0x0000
	class hkMotionStateTODO *motion_state; //0x003C
	char pad_0040[132]; //0x0040
	float N00001981; //0x00C4
	char pad_00C8[60]; //0x00C8
}; //Size: 0x0104

class CharacterHandleBase
{
public:
	char pad_0000[172]; //0x0000
}; //Size: 0x00AC

class CharacterHandle
{
public:
	char pad_0000[20]; //0x0000
	class CharacterHandleBase base; //0x0014
	char pad_00C0[92]; //0x00C0
	class Character *character; //0x011C
	char pad_0120[118]; //0x0120
	bool pending_to_destroy; //0x0196
	char pad_0197[1]; //0x0197
	bool any_npc_close; //0x0198
	char pad_0199[15]; //0x0199
}; //Size: 0x01A8

class Model
{
public:
	class ModelInstance *instance; //0x0000
	char pad_0004[4]; //0x0004
	vec3 N00001475; //0x0008
	vec3 N00001476; //0x0014
	char pad_0020[4]; //0x0020
	uint8_t some_flags; //0x0024
	char pad_0025[3]; //0x0025
}; //Size: 0x0028

class hkCharacterProxy
{
public:
	char pad_0000[16]; //0x0000
	vec3 N00001534; //0x0010
	char pad_001C[4]; //0x001C
	vec3 N00001538; //0x0020
	char pad_002C[4]; //0x002C
	class hkSimpleShapePhantom *shape_phantom; //0x0030
	char pad_0034[16]; //0x0034
}; //Size: 0x0044

class Unk
{
public:
	char pad_0000[132]; //0x0000
}; //Size: 0x0084

class _AiCore
{
public:
	class AiCore *ai_core; //0x0000
}; //Size: 0x0004

class AiCore
{
public:
	char pad_0000[1536]; //0x0000
	class AiCoreActiveHandles *active_handles; //0x0600
	char pad_0604[16]; //0x0604
	void *dead_handles; //0x0614
	char pad_0618[24]; //0x0618
}; //Size: 0x0630

class ModelInstance
{
public:
	char pad_0000[68]; //0x0000
}; //Size: 0x0044

class _ResourceStreamer
{
public:
	class ResourceStreamer *asset_loader; //0x0000
}; //Size: 0x0004

class ResourceStreamer
{
public:
	char pad_0000[216]; //0x0000
	bool dispatch; //0x00D8
	bool N00001659; //0x00D9
	char pad_00DA[38]; //0x00DA
}; //Size: 0x0100

class bref
{
public:
	void *vt; //0x0000
	void *object; //0x0004
	void *counter; //0x0008
	void *unk1; //0x000C
	void *unk2; //0x0010
	void *unk3; //0x0014
}; //Size: 0x0018

class DamageableObject
{
public:
	char pad_0000[72]; //0x0000
	float max_hp; //0x0048
	float hp; //0x004C
	char pad_0050[8]; //0x0050
	mat4 transform; //0x0058
	mat4 N00002209; //0x0098
	std::string whole_model_name; //0x00D8
	std::string broken_model_name; //0x00F4
	std::string whole_pfx_name; //0x0110
	std::string broken_pfx_name; //0x012C
	std::string name; //0x0148
	class Model *whole_model; //0x0164
	class Model *broken_model; //0x0168
	class bref whole_pfx; //0x016C
	class bref broken_pfx; //0x0184
	int32_t broken; //0x019C
	float last_hp; //0x01A0
	std::string some_string; //0x01A4
	char pad_01C0[4]; //0x01C0
	vec3 unk1; //0x01C4
	char pad_01D0[104]; //0x01D0
	std::string fx_on_break_name; //0x0238
	char pad_0254[36]; //0x0254
	float pulse_force; //0x0278
	uint16_t flags1; //0x027C
	uint16_t some_force; //0x027E
	uint16_t force; //0x0280
	uint16_t force_damage; //0x0282
	uint16_t N00004F7C; //0x0284
	uint16_t N00004FFE; //0x0286
	void *transform_shape; //0x0288
	char pad_028C[12]; //0x028C
}; //Size: 0x0298

class _GameStatus
{
public:
	class GameStatus *game_status; //0x0000
}; //Size: 0x0004

class GameStatus
{
public:
	char pad_0000[32]; //0x0000
	uint32_t status; //0x0020
	char pad_0024[28]; //0x0024
	bool skip_intro; //0x0040
	char pad_0041[3]; //0x0041
}; //Size: 0x0044

class GameInfo
{
public:
	void *hwnd; //0x0000
	char pad_0004[4]; //0x0004
	int32_t width; //0x0008
	int32_t height; //0x000C
	char pad_0010[36]; //0x0010
	bool skip_intro; //0x0034
	char pad_0035[7]; //0x0035
}; //Size: 0x003C

class hkDefaultAnalogDriverInput
{
public:
	char pad_0000[24]; //0x0000
	bool fully_stopped; //0x0018
	char pad_0019[3]; //0x0019
	float x; //0x001C
	float y; //0x0020
	char pad_0024[20]; //0x0024
	float dead_zone; //0x0038
	char pad_003C[8]; //0x003C
}; //Size: 0x0044

class hkMotionStateTODO
{
public:
	char pad_0000[44]; //0x0000
	float drag; //0x002C
	char pad_0030[16]; //0x0030
	vec3 velocity; //0x0040
	char pad_004C[100]; //0x004C
	vec3 position; //0x00B0
	char pad_00BC[116]; //0x00BC
}; //Size: 0x0130

class CameraTODO1
{
public:
	char pad_0000[64]; //0x0000
	bool N000015CF; //0x0040
	bool N000016A6; //0x0041
	bool N000016A9; //0x0042
	char pad_0043[17]; //0x0043
	vec3 N000015D4; //0x0054
	char pad_0060[32]; //0x0060
	float N000015DD; //0x0080
	char pad_0084[4]; //0x0084
	vec3 N000015DF; //0x0088
	char pad_0094[128]; //0x0094
}; //Size: 0x0114

class MountedGun
{
public:
	char pad_0000[116]; //0x0000
	bool being_used; //0x0074
	char pad_0075[3]; //0x0075
	class MountedController *controller; //0x0078
	char pad_007C[4]; //0x007C
	mat4 transform; //0x0080
	char pad_00C0[80]; //0x00C0
}; //Size: 0x0110

class MountedController
{
public:
	char pad_0000[4]; //0x0000
	mat4 N00001559; //0x0004
	char pad_0044[12]; //0x0044
	mat4 N00001771; //0x0050
	mat4 N00001772; //0x0090
	mat4 N00001773; //0x00D0
	mat4 N00001774; //0x0110
	char pad_0150[44]; //0x0150
	mat4 N00001780; //0x017C
	mat4 N00001781; //0x01BC
	mat4 N00001782; //0x01FC
	float move_speed_x; //0x023C
	float move_speed_y; //0x0240
	char pad_0244[156]; //0x0244
	vec3 gun_offset; //0x02E0
	vec3 base_offset; //0x02EC
	char pad_02F8[16]; //0x02F8
	mat4 N00001685; //0x0308
	mat4 N00001686; //0x0348
	std::string base_model_name; //0x0388
	class Model *base_model; //0x03A4
	char pad_03A8[104]; //0x03A8
	class Model *model; //0x0410
	std::string mount_model; //0x0414
	char pad_0430[20]; //0x0430
}; //Size: 0x0444

class VehicleSpawnPoint
{
public:
	char pad_0000[132]; //0x0000
	std::string spawn_object; //0x0084
	char pad_00A0[148]; //0x00A0
	class VehicleType *vehicle_type; //0x0134
	char pad_0138[32]; //0x0138
}; //Size: 0x0158

class VehicleType
{
public:
	char pad_0000[468]; //0x0000
	bool N00001689; //0x01D4
	bool loaded; //0x01D5
	bool N000016DE; //0x01D6
	bool N000016DC; //0x01D7
	char pad_01D8[32]; //0x01D8
}; //Size: 0x01F8

class ExportedEntityResource
{
public:
	char pad_0000[280]; //0x0000
	class ExportedEntity *exported_entity; //0x0118
	char pad_011C[4]; //0x011C
}; //Size: 0x0120

class ExportedEntity
{
public:
	char pad_0000[68]; //0x0000
}; //Size: 0x0044

class SkeletonInstance
{
public:
	char pad_0000[56]; //0x0000
	class SkeletonTODO1 *idk; //0x0038
	char pad_003C[44]; //0x003C
}; //Size: 0x0068

class SkeletonTODO1
{
public:
	char pad_0000[20]; //0x0000
}; //Size: 0x0014

class NPCVariant
{
public:
	char pad_0000[1356]; //0x0000
	bool b1; //0x054C
	bool b2; //0x054D
	bool b3; //0x054E
	bool b4; //0x054F
	bool b5; //0x0550
	bool b6; //0x0551
	bool b7; //0x0552
	bool b8; //0x0553
}; //Size: 0x0554

class Ladder
{
public:
	void *vtable; //0x0000
	char pad_0004[28]; //0x0004
	class transform *TransformPtr; //0x0020
	char pad_0024[28]; //0x0024
	std::string pfx_name; //0x0040
	std::string name; //0x005C
	char pad_0078[32]; //0x0078
	class bref pfx; //0x0098
	char pad_00B0[60]; //0x00B0
	uint16_t N00001DCB; //0x00EC
	uint16_t N00001E05; //0x00EE
	uint16_t N00001812; //0x00F0
	uint16_t N00001DA5; //0x00F2
	uint16_t N00001813; //0x00F4
	uint16_t N00001DA8; //0x00F6
}; //Size: 0x00F8

class transform
{
public:
	mat4 N00001D53; //0x0000
}; //Size: 0x0040

class ItemPickup
{
public:
	char pad_0000[32]; //0x0000
	mat4 transform; //0x0020
	char pad_0060[28]; //0x0060
	uint16_t respawn_time; //0x007C
	char pad_007E[2]; //0x007E
	uint16_t time_left_to_spawn; //0x0080
	char pad_0082[62]; //0x0082
	uint16_t idk; //0x00C0
	char pad_00C2[2]; //0x00C2
	class ItemPickupAnimator *animator; //0x00C4
	char pad_00C8[12]; //0x00C8
	uint16_t flags; //0x00D4
	char pad_00D6[254]; //0x00D6
}; //Size: 0x01D4

class WeaponTemplateList
{
public:
	char pad_0000[172]; //0x0000
}; //Size: 0x00AC

class ItemPickupAnimator
{
public:
	char pad_0000[4]; //0x0000
	int16_t N00002107; //0x0004
	char pad_0006[2]; //0x0006
	uint16_t N00002108; //0x0008
	uint16_t N00002172; //0x000A
	uint16_t N00002109; //0x000C
	uint16_t N00002174; //0x000E
	uint16_t N0000210A; //0x0010
	uint16_t N00002176; //0x0012
	char pad_0014[60]; //0x0014
	mat4 transform; //0x0050
	class EmissiveFlare *flare; //0x0090
	char pad_0094[4]; //0x0094
	class TransformPtr *transform_ptr; //0x0098
	char pad_009C[64]; //0x009C
	bool N0000212E; //0x00DC
	char pad_00DD[3]; //0x00DD
}; //Size: 0x00E0

class TransformPtr
{
public:
	mat4 N000021FC; //0x0000
}; //Size: 0x0040

class EmissiveFlare
{
public:
	char pad_0000[872]; //0x0000
}; //Size: 0x0368

class _SafeHouseList
{
public:
	class SafeHouseList *safehouse_list; //0x0000
}; //Size: 0x0004

class SafeHouseList
{
public:
	char pad_0000[136]; //0x0000
	vec4 position; //0x0088
	char pad_0098[1220]; //0x0098
}; //Size: 0x055C

class AiCoreActiveHandles
{
public:
	char pad_0000[68]; //0x0000
}; //Size: 0x0044

class AnimatedRigidObject
{
public:
	char pad_0000[644]; //0x0000
	class EventList *open_event; //0x0284
	class EventList *close_event; //0x0288
	char pad_028C[16]; //0x028C
}; //Size: 0x029C

class EventList
{
public:
	char pad_0000[4]; //0x0000
	class EventListFirst *first; //0x0004
	char pad_0008[4]; //0x0008
}; //Size: 0x000C

class EventListFirst
{
public:
	class Event *event; //0x0000
	char pad_0004[4]; //0x0004
}; //Size: 0x0008

class Vector
{
public:
	char pad_0000[4]; //0x0000
	class VectorFirst *first; //0x0004
	class VectorLast *last; //0x0008
	char pad_000C[4]; //0x000C
}; //Size: 0x0010

class Event
{
public:
	char pad_0000[8]; //0x0000
	class Vector data; //0x0008
	char pad_0018[28]; //0x0018
}; //Size: 0x0034

class VectorFirst
{
public:
	char pad_0000[24]; //0x0000
}; //Size: 0x0018

class VectorLast
{
public:
	char pad_0000[24]; //0x0000
}; //Size: 0x0018

class SoundGameObject
{
public:
	char pad_0000[28]; //0x0000
	std::string sob; //0x001C
	std::string sab; //0x0038
	char pad_0054[8]; //0x0054
	uint16_t max_distance; //0x005C
	char pad_005E[2]; //0x005E
	mat4 transform; //0x0060
	uint32_t N00001C56; //0x00A0
	uint16_t N00001C57; //0x00A4
	uint16_t N00001C64; //0x00A6
	char pad_00A8[48]; //0x00A8
}; //Size: 0x00D8

class hkMoveComponent
{
public:
	char pad_0000[4]; //0x0000
	class _TODO1 *idk; //0x0004
	char pad_0008[196]; //0x0008
}; //Size: 0x00CC

class _TODO1
{
public:
	char pad_0000[68]; //0x0000
}; //Size: 0x0044

class CharacterTest : public Character
{
public:
	char pad_0ACC[60]; //0x0ACC
}; //Size: 0x0B08

class hkCharacterVelocities
{
public:
	char pad_0000[4]; //0x0000
	class _TODO2 *idk; //0x0004
	char pad_0008[24]; //0x0008
	vec4 N00001D41; //0x0020
	char pad_0030[32]; //0x0030
}; //Size: 0x0050

class _TODO2
{
public:
	char pad_0000[16]; //0x0000
	vec4 N00001D7C; //0x0010
	char pad_0020[48]; //0x0020
}; //Size: 0x0050

class UIMapIcon
{
public:
	char pad_0000[56]; //0x0000
	uint16_t x; //0x0038
	uint16_t y; //0x003A
	uint16_t z; //0x003C
	char pad_003E[38]; //0x003E
}; //Size: 0x0064

class Objective
{
public:
	char pad_0000[88]; //0x0000
	vec3 position; //0x0058
	char pad_0064[64]; //0x0064
	class UITargetIndicator *target_indicator; //0x00A4
	char pad_00A8[132]; //0x00A8
}; //Size: 0x012C

class UITargetIndicator
{
public:
	char pad_0000[12]; //0x0000
	mat4 N00002277; //0x000C
	char pad_004C[16]; //0x004C
	float N0000227C; //0x005C
	float N0000227D; //0x0060
	float N0000227E; //0x0064
	float N0000227F; //0x0068
	uint8_t r; //0x006C
	uint8_t g; //0x006D
	uint8_t b; //0x006E
	uint8_t a; //0x006F
	char pad_0070[520]; //0x0070
}; //Size: 0x0278

class PlayerGlobalInfo
{
public:
	class _PlayerGlobalInfo *player_global_info; //0x0000
}; //Size: 0x0004

class _PlayerGlobalInfo
{
public:
	char pad_0000[4]; //0x0000
	class CharacterHandleBase *localplayer_handle_base; //0x0004
	char pad_0008[28]; //0x0008
}; //Size: 0x0024

class VehicleSeat
{
public:
	char pad_0000[72]; //0x0000
	class Character *character; //0x0048
	char pad_004C[348]; //0x004C
}; //Size: 0x01A8

using weapon_ref = ref<Weapon>;