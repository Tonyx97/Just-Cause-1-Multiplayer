#include <stdint.h>
#include <string>

struct vec3
{
	float x, y, z;
};

struct vec4
{
	float x, y, z, w;
};

struct quat
{
	float x, w, z, y;
};

struct mat4 {
    union {
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

        };
        float m[4][4];
    };
};

template <typename T>
struct ref_count
{
	void* vt;
	int	  uses, weaks;
	int	  unk1;
	T*	  obj;
	int	  unk2;
};

template <typename T>
struct ref
{
	T*			  obj	  = nullptr;
	ref_count<T>* counter = nullptr;
};