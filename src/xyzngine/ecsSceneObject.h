#pragma once
#include "flecs.h"
#include "ecsComponents.h"



struct CameraPosition : public Ogre::Vector3
{
	using Ogre::Vector3::Vector3;
};

void register_ecs_control_systems(flecs::world* ecs);

void register_ecs_render_systems(flecs::world* ecs);