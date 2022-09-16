#include "camera.h"

camera_t g_camera =
{
	.position = { 0.f, 0.f, 0.f },
	.direction = { 0.f, 0.f, 1.0f },
	.forward_velocity = { 0.f, 0.f, 0.f},
	.yaw_angle = 0.0f
};