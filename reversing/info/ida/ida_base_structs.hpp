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