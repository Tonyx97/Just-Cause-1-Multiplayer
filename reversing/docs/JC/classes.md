```cpp
// Classes and members:

template <class T>
struct vector
{
	static_assert(std::is_pointer_v<T>, 'This vector class must handle pointers only');

	char pad[0x4];

	T first,
		last;

	size_t size() const		{ return last - first; }

	auto begin() const		{ return (T*)first; }
	auto end() const		{ return (T*)last; }
};

struct AmmoManager (D84EC4)
{
	0x70          - vector<Bullet*> bullet_list;
	0x84 to 0xA0  - int32_t max_ammo_slotA to H;
};

struct StatSystem (D84F20)
{
	0x70 - uint civilians_killed;
	0x7C - uint soldiers_killed;
	0x80 - uint police_killed;
	0x84 - uint guerrilla_killed;
	0x88 - uint blackhand_killed;
	0x8C - uint montano_killed;
	0xAC - uint total_shots_fired;
	0xB0 - uint story_missions_completed;
	0xB4 - uint side_missions_completed;
	0xB8 - uint roof_to_roof_stunts;
	0xBC - uint parachute_to_roof_stunts;
	0xC0 - uint rioja_settlements;
	0xC4 - uint guerrilla_settlements;
	0xCC - float highest_base_jump;
	0xD0 - uint total_game_time;
	0xD4 - uint session_time;
	0xD8 - uint agency_safehouses;
	0xDC - uint guerrillaa_safehouses;
	0xE0 - uint rioja_safehouses;
};

struct DayCycle (D33030)
{
	0x4 = float hours_per_day;
	0x8 = float days_per_year;
	0xC = float months_per_year;
	0x10 = float year;
	0x14 = float day;
	0x18 = float hour;
	0x1C = bool enabled;
};

struct Bullet
{
	0xC 	- float damage_or_force;
	0x54 	- vec3 position;
	0x64 	- vec3 direction;
	0x70 	- vec3 last_position;
	0x94 	- float max_range;
	0x9E 	- bool alive;
};

struct Camera
{
	0x54 	- mat4 projection_matrix;
	0x94 	- mat4 view_matrix;
	0x3A4 	- int width;
	0x3A8 	- int height;
};

struct CameraManager (AF23DC)
{
	0x8 	- Camera* main_camera;
	0x60 	- mat4 current_projection_matrix;
	0xA0 	- mat4 current_projection_matrix;
};

struct hkWorld
{
	0xE0 - vec4 gravity;
};

struct LocalPlayer
{
	0x1C - Character* character;
};

struct MemoryManager (AF1E98)
{
	0x23C - HANDLE semaphore;
	0x240 - uint flags;
};

struct PhysicsSystem (D37340)
{
	0x2C - vec3 world_position;
	0x4C - hkWorld* hk_world;
};

struct Renderer
{
	0x448 - IDirect3DDevice9* device;
};

struct SettingsManager (D32EE4)
{
	0x1C4 - float fx_volume;
	0x1C8 - float music_volume;
	0x1CC - float dialogue_volume;
	0x1D0 - int motion_blur_enabled;
	0x1D4 - int action_camera_enabled;
	0x1D8 - int subtitles_enabled;
	0x1DC - int invert_up_down_enabled;
	0x1E4 - int show_health_bar;
	0x1E8 - int show_weapon_selector;
	0x1EC - int show_objective_info;
	0x1F0 - int show_minimap;
	0x1F4 - int show_action_icons;
	0x200 - int screen_resolution;
	0x204 - int heat_haze_enabled;
	0x208 - int water_quality;
	0x20C - int terrain_detail_quality;
	0x210 - int scene_complexity;
	0x214 - int texture_resolution;
	0x218 - int pfx_enabled;
};

struct SoundBank
{
};

struct SoundSystem (D32ADC)
{
	0x40 - SoundBank* hud_bank;
	0x50 - SoundBank* ambience_bank;
	0x54 - SoundBank* explosion_bank;
	0x58 - SoundBank* weapon_bank;
};

struct SpawnManager (D32C54)
{
	0x3C - int16 max_characters_to_spawn;
	0x3E - int16 max_vehicles_to_spawn;
};

struct TimeSystem (AF2368)
{
	0xC 	- float fps;
	0x10 	- float delta;
	0x14 	- float last_fps;
	0x18 	- float last_delta;
	0x1C 	- float time_scale;
};

struct Transform
{
	0x0 - mat4 matrix;
};

struct Vehicle
{
	0x1E8 - vec3 camera_target_offset;
	0x200 - float camera_fov;
};

struct VehicleController
{
	0x4     - Transform transform;
	0x4C    - Vehicle* current_vehicle;
};

template <typename T>
struct LinkedList
{
	struct Node
	{
		0x0 - Node* next;
		0x8 - T* vehicle;
	};

	0x0 - Node* root;
};

struct VehicleManager (D84F88)
{
	0x14 - LinkedList<Vehicke*>* vehicles;
	0x18 - int vehicles_count
};

struct World (D33238)
{
	0x20 - vector<Character*> characters;
	0x30 - LocalPlayer* localplayer;
};

struct Skeleton
{
	0x260 - vector<Bone*> bone_list;
	0x294 - Transform** transform_list;
	0x2E0 - Transform* head_transform;
	0x2E8 - Transform* stomach_transform;
	0x2F4 - Transform* pelvis_transform;
};

struct hkCharacterProxy
{
	0x58 - hkSimpleShapePhantom* hk_shape
	0x30 - vec3 velocity
	0xC4 - float mass
};

struct Character
{
	0x48 	- float max_health;
	0x4C 	- float health;
	0x90 	- int ammo_grenades;
	0xD8 	- WeaponBelt* weapon_belt;
	0xDC 	- Skeleton skeleton;
	0x5D8 	- Transform transform;
	0x80C 	- quat target_rotation;
	0x850   - hkCharacterProxy* hk_proxy
	0x870   - int closest_vehicles_count;
	0x878   - VehicleController* vehicle_controller;
	0x914   - float grenade_timeout;
	0x918   - float grenade_time;
};

struct WeaponInfo
{
	0x10 	- int slot;
	0x30    - float firerate
	0x38 	- int max_magazine_ammo;
	0x44 	- int bullets_to_fire;
	0x54 	- char[28] type_name;
	0x70 	- char[28] name;
	0xA4 	- vec3 muzzle_offset;
	0x118 	- int fire_sound_id;
	0x130	- float bullet_force_damage1; 
	0x134	- float bullet_force_damage2; 
	0x176	- bool can_create_shells;
};

struct Weapon
{
	0x38 	- Transform grip_transform;
	0x78 	- Character* owner;
	0xC0 	- Transform last_muzzle_transform;
	0x100 	- Transform muzzle_transform;
	0x140 	- Transform last_ejector_transform;
	0x184 	- int ammo;
	0x188 	- bool is_firing_internal_usage;
	0x18C	- float last_hip_aim_time;
	0x19C	- float last_shot_time;
	0x1AC 	- WeaponInfo* info;
};

struct WeaponBase
{
	0x0 - Weapon* weapon;
};

struct WeaponBelt*
{
	0x8 to 0x24 		- int ammo_slotA to H;
	0x74 + (i * 0x10)	- WeaponBase* weapon_slot_i;
	0xF0 				- int current_weapon_id;
};

struct GameControlSystem (D37380)
{

};
```