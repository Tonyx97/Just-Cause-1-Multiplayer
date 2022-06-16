#include "ida_base_structs.h"
// Created with ReClass.NET 1.2 by KN4CK3R

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

class Skeleton
{
public:
	char pad_0000[608]; //0x0000
	void *bone_list; //0x0260
	char pad_0264[48]; //0x0264
	class Transform (*transform_list)[20]; //0x0294
	char pad_0298[72]; //0x0298
	class Transform *head_transform; //0x02E0
	char pad_02E4[4]; //0x02E4
	class Transform *stomach_transform; //0x02E8
	char pad_02EC[8]; //0x02EC
	class Transform *pelvis_transform; //0x02F4
	char pad_02F8[328]; //0x02F8
}; //Size: 0x0440

class Transform
{
public:
	mat4 m; //0x0000
}; //Size: 0x0040

class Character
{
public:
	char pad_0000[72]; //0x0000
	float max_health; //0x0048
	float health; //0x004C
	char pad_0050[64]; //0x0050
	int32_t ammo_grenades; //0x0090
	char pad_0094[68]; //0x0094
	class WeaponBelt *weapon_belt; //0x00D8
	class Skeleton N000009AB; //0x00DC
	char pad_051C[188]; //0x051C
	class Transform N000009DB; //0x05D8
	char pad_0618[568]; //0x0618
	class hkCharacterProxy *hk_character_proxy; //0x0850
	char pad_0854[28]; //0x0854
	int32_t closest_vehicle_count; //0x0870
	char pad_0874[4]; //0x0874
	class VehicleController *vehicle_controller; //0x0878
	char pad_087C[152]; //0x087C
	float grenade_timeout; //0x0914
	float grenade_time; //0x0918
	char pad_091C[1612]; //0x091C
}; //Size: 0x0F68

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
	char pad_0028[76]; //0x0028
	class WeaponBase *slotA_weapon; //0x0074
	char pad_0078[12]; //0x0078
	class WeaponBase *N00000CCB; //0x0084
	char pad_0088[12]; //0x0088
	class WeaponBase *N00000CCF; //0x0094
	char pad_0098[12]; //0x0098
	class WeaponBase *N00000CD3; //0x00A4
	char pad_00A8[12]; //0x00A8
	class WeaponBase *N00000CD7; //0x00B4
	char pad_00B8[12]; //0x00B8
	class WeaponBase *N00000CDB; //0x00C4
	char pad_00C8[12]; //0x00C8
	class WeaponBase *N00000CDF; //0x00D4
	char pad_00D8[72]; //0x00D8
}; //Size: 0x0120

class WeaponBase
{
public:
	class Weapon *weapon; //0x0000
	char pad_0004[4]; //0x0004
}; //Size: 0x0008

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
	bool firing_internal; //0x0188
	char pad_0189[3]; //0x0189
	float last_hip_aim_time; //0x018C
	char pad_0190[12]; //0x0190
	float last_shot_time; //0x019C
	vec3 aim_hit_position; //0x01A0
	class WeaponInfo *N00000E2E; //0x01AC
	char pad_01B0[20]; //0x01B0
	int32_t total_bullets_fired; //0x01C4
	int32_t bullets_fired_while_firing; //0x01C8
	bool firing; //0x01CC
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

class hkCharacterProxy
{
public:
	char pad_0000[88]; //0x0000
	class hkSimpleShapePhantom *hk_shape; //0x0058
	char pad_005C[104]; //0x005C
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
	char pad_0040[212]; //0x0040
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
	char pad_0000[488]; //0x0000
	vec3 camera_target_offset; //0x01E8
	char pad_01F4[12]; //0x01F4
	float camera_fov; //0x0200
	char pad_0204[1092]; //0x0204
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
	char pad_0000[336]; //0x0000
}; //Size: 0x0150

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
	char pad_0020[1308]; //0x0020
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
	char pad_0038[20]; //0x0038
	class hkWorld *N00000DBF; //0x004C
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
	class LocalPlayer *N00001401; //0x0030
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
	char pad_0000[88]; //0x0000
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
	void *template_list; //0x0008
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


