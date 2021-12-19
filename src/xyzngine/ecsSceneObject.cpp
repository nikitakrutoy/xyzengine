#include "ecsSceneObject.h"

void register_ecs_control_systems(flecs::world* ecs)
{
	static auto inputQuery = ecs->query<InputHandlerPtr>();
	ecs->system<const Controllable, ScriptComponent, CameraPosition>()
		.each([&](flecs::entity e, const Controllable&, ScriptComponent& script, CameraPosition& cameraPos)
			{
				Ogre::Vector3 vCameraPosition = script.ptr->GetCameraPosition();
				cameraPos.x = vCameraPosition.x;
				cameraPos.y = vCameraPosition.y;
				cameraPos.z = vCameraPosition.z;
			});

	ecs->system<const Controllable, ScriptComponent, Position>()
		.each([&](flecs::entity e, const Controllable&, ScriptComponent& script, Position& pos)
			{
				Ogre::Vector3 vPosition = script.ptr->GetPosition();
				pos.x = vPosition.x;
				pos.y = vPosition.y;
				pos.z = vPosition.z;
			});
}

void register_ecs_render_systems(flecs::world* ecs)
{
	static auto renderQuery = ecs->query<RenderEnginePtr>();

	ecs->system<RenderComponent, const Position>()
		.each([&](RenderComponent& renderNode, const Position& pos)
			{
				renderQuery.each([&](RenderEnginePtr renderEngine)
					{
						renderEngine.ptr->GetRT()->RC_LambdaAction([=] {
							renderNode.ptr->setPosition(pos);
							});
					});
			});
}