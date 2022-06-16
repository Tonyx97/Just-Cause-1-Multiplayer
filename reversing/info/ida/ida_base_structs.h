typedef unsigned long uintptr_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef short int16_t;

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

struct mat4
{
	vec4 col0, col1, col2, col3;
};

typedef struct _mat4 {
    union {
        struct {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

        };
        float m[4][4];
    };
} mat4;