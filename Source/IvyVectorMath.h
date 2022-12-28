#ifndef IVY_VECTOR_MATH_H
#define IVY_VECTOR_MATH_H

typedef float IvyV2[2];
typedef float IvyV3[3];
typedef float IvyV4[4];
typedef float IvyQ4[4];

typedef IvyV2 IvyM2[2];
typedef IvyV3 IvyM3[3];
typedef IvyV4 IvyM4[4];

// TODO(samuel): inline versions

void ivyCopyV3(IvyV3 vector, IvyV3 out);
void ivyCopyV4(IvyV4 vector, IvyV4 out);

void ivyIdentityM3(IvyM3 matrix);
void ivyIdentityM4(IvyM4 matrix);

void ivyBroadcastV2(float value, IvyV2 vector);
void ivyBroadcastV3(float value, IvyV3 vector);
void ivyBroadcastV4(float value, IvyV4 vector);

float ivyGetMagnitudeOfV2(IvyV2 vector);
float ivyGetMagnitudeOfV3(IvyV3 vector);
float ivyGetMagnitudeOfV4(IvyV4 vector);

void ivyScaleV2(float value, IvyV2 inOut);
void ivyScaleV3(float value, IvyV3 inOut);
void ivyScaleV4(float value, IvyV4 inOut);

void ivyNormalizeV2(IvyV2 vector);
void ivyNormalizeV3(IvyV3 vector);
void ivyNormalizeV4(IvyV4 vector);

void ivyZeroV2(IvyV2 out);
void ivyZeroV3(IvyV3 out);
void ivyZeroV4(IvyV4 out);

void ivyInvM2(IvyM2 out);
void ivyInvM3(IvyM3 out);
void ivyInvM4(IvyM4 out);

void ivyCrossV2ToV2(IvyV2 lhs, IvyV2 rhs, IvyV2 out);
void ivyCrossV3ToV3(IvyV3 lhs, IvyV3 rhs, IvyV3 out);
void ivyCrossV4ToV4(IvyV4 lhs, IvyV4 rhs, IvyV4 out);

float ivyDotV2ToV2(IvyV2 lhs, IvyV2 rhs);
float ivyDotV3ToV3(IvyV3 lhs, IvyV3 rhs);
float ivyDotV4ToV4(IvyV4 lhs, IvyV4 rhs);

void ivyAddV2ToV2(IvyV2 lhs, IvyV2 rhs, IvyV2 out);
void ivyAddV3ToV3(IvyV3 lhs, IvyV3 rhs, IvyV3 out);
void ivyAddV4ToV4(IvyV4 lhs, IvyV4 rhs, IvyV4 out);

void ivyAddM2ToM2(IvyM2 lhs, IvyM2 rhs, IvyM2 out);
void ivyAddM3ToM3(IvyM3 lhs, IvyM3 rhs, IvyM3 out);
void ivyAddM4ToM4(IvyM4 lhs, IvyM4 rhs, IvyM4 out);

void ivyMulM2ToM2(IvyM2 lhs, IvyM2 rhs, IvyM2 out);
void ivyMulM3ToM3(IvyM3 lhs, IvyM3 rhs, IvyM3 out);
void ivyMulM4ToM4(IvyM4 lhs, IvyM4 rhs, IvyM4 out);

void ivyMakeRotateM4(float angle, IvyV3 axis, IvyM4 out);
void ivyTranslateM4(IvyV3 offset, IvyM4 inOut);

void ivyCreateOrthographicM4(
    float left,
    float right,
    float bottom,
    float top,
    float near,
    float far,
    IvyM4 out);

void ivyCreatePerspectiveM4(
    float fov,
    float ratio,
    float near,
    float far,
    IvyM4 out);

void ivyCreateLookAtM4(IvyV3 eye, IvyV3 at, IvyV3 up, IvyM4 out);
void ivyCreateDirectionV3(float pitch, float yaw, IvyV3 up, IvyV3 out);

void ivyRotateM4(IvyM4 matrix, float angle, IvyV3 axis, IvyM4 out);

void ivyGetDistanceBetweenV2AndV2(IvyV2 from, IvyV2 to);
void ivyGetDistanceBetweenV3AndV3(IvyV3 from, IvyV3 to);
void ivyGetDistanceBetweenV4AndV4(IvyV4 from, IvyV4 to);

void ivyMixV3(IvyV3 from, IvyV3 to, float weight, IvyV3 out);
void ivyMixV4(IvyV4 from, IvyV4 to, float weight, IvyV4 out);
void ivyQ4AsM4(IvyQ4 quaternion, IvyM3 out);
void ivyScaleM4ByV3(IvyM4 matrix, IvyV3 scale, IvyM4 out);

#endif