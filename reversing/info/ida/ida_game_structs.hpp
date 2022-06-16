#include "ida_base_structs.hpp"
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
	char pad_0298[12]; //0x0298
	class Unk *unk; //0x02A4
	char pad_02A8[56]; //0x02A8
	class Transform *head_transform; //0x02E0
	char pad_02E4[4]; //0x02E4
	class Transform *stomach_transform; //0x02E8
	char pad_02EC[8]; //0x02EC
	class Transform *pelvis_transform; //0x02F4
	char pad_02F8[224]; //0x02F8
	float head_yaw; //0x03D8
	float head_pitch; //0x03DC
	float head_roll; //0x03E0
	float head_mover; //0x03E4
	float head_mover2; //0x03E8
	char pad_03EC[4]; //0x03EC
	vec4 head_rotation; //0x03F0
	char pad_0400[56]; //0x0400
}; //Size: 0x0438

class StanceController
{
public:
	char pad_0000[36]; //0x0000
	int32_t movement_id; //0x0024
	char pad_0028[44]; //0x0028
}; //Size: 0x0054

class Transform
{
public:
	mat4 m; //0x0000
}; //Size: 0x0040

class CharacterInfo
{
public:
	char pad_0000[240]; //0x0000
}; //Size: 0x00F0

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
	class Skeleton skeleton; //0x00DC
	class StanceController stance_controller; //0x0514
	char pad_0568[112]; //0x0568
	class Transform N000009DB; //0x05D8
	char pad_0618[320]; //0x0618
	mat4 head_transform; //0x0758
	char pad_0798[28]; //0x0798
	class Model *model; //0x07B4
	class Model *head_attachments; //0x07B8
	char pad_07BC[16]; //0x07BC
	class Model *some_model; //0x07CC
	char pad_07D0[128]; //0x07D0
	class Physical *physical; //0x0850
	char pad_0854[28]; //0x0854
	int32_t closest_vehicle_count; //0x0870
	char pad_0874[4]; //0x0874
	class VehicleController *vehicle_controller; //0x0878
	char pad_087C[8]; //0x087C
	uint32_t flags; //0x0884
	char pad_0888[92]; //0x0888
	float death_time; //0x08E4
	char pad_08E8[44]; //0x08E8
	float grenade_timeout; //0x0914
	float grenade_time; //0x0918
	char pad_091C[8]; //0x091C
	class CharacterInfo info; //0x0924
	char pad_0A14[184]; //0x0A14
}; //Size: 0x0ACC

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
	char pad_00D8[80]; //0x00D8
}; //Size: 0x0128

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

class Physical
{
public:
	char pad_0000[12]; //0x0000
	class Character *userdata; //0x000C
	char pad_0010[64]; //0x0010
	class hkCharacterProxy *proxy; //0x0050
	char pad_0054[4]; //0x0054
	class hkSimpleShapePhantom *shape_phantom; //0x0058
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
	char pad_0000[72]; //0x0000
	float max_health; //0x0048
	float health; //0x004C
	char pad_0050[408]; //0x0050
	vec3 camera_target_offset; //0x01E8
	char pad_01F4[12]; //0x01F4
	float camera_fov; //0x0200
	char pad_0204[716]; //0x0204
	float N0000143A; //0x04D0
	char pad_04D4[372]; //0x04D4
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
	char pad_0038[12]; //0x0038
	void *collision_cache; //0x0044
	char pad_0048[4]; //0x0048
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
	char pad_0040[160]; //0x0040
}; //Size: 0x00E0

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
	char pad_0014[20]; //0x0014
}; //Size: 0x0028

class pfxFloatingRigidBody
{
public:
	char pad_0000[260]; //0x0000
}; //Size: 0x0104

class CharacterHandle
{
public:
	char pad_0000[284]; //0x0000
	class Character *character; //0x011C
	char pad_0120[118]; //0x0120
	bool pending_to_destroy; //0x0196
	char pad_0197[1]; //0x0197
	bool any_npc_close; //0x0198
	char pad_0199[163]; //0x0199
}; //Size: 0x023C

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
	char pad_0000[48]; //0x0000
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
	void *active_handles; //0x0600
	char pad_0604[16]; //0x0604
	void *dead_handles; //0x0614
	char pad_0618[24]; //0x0618
}; //Size: 0x0630

class ModelInstance
{
public:
	char pad_0000[68]; //0x0000
}; //Size: 0x0044

class _AssetLoader
{
public:
	class AssetLoader *asset_loader; //0x0000
}; //Size: 0x0004

class AssetLoader
{
public:
	char pad_0000[256]; //0x0000
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
	char N0000222D[0x10]; //0x01D0
	char pad_01E0[88]; //0x01E0
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

class ref
{
public:
	void *object; //0x0000
	class ref_count *counter; //0x0004
}; //Size: 0x0008

class ref_count
{
public:
	void *vt; //0x0000
	int32_t uses; //0x0004
	int32_t weaks; //0x0008
	int32_t unk1; //0x000C
	void *object; //0x0010
	int32_t unk2; //0x0014
}; //Size: 0x0018

