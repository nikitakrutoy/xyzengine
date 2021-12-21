#include "ecsSceneObject.h"

void register_ecs_control_systems(flecs::world* ecs)
{
	static auto inputQuery = ecs->query<InputHandlerPtr>();

	ecs->system<ScriptComponent, Transform>()
		.each([&](flecs::entity e, ScriptComponent& script, Transform& t)
			{
				t = script.ptr->GetTransform("Transform");
			});


}

void register_ecs_render_systems(flecs::world* ecs)
{
	static auto renderQuery = ecs->query<RenderEnginePtr>();

	ecs->system<RenderComponent, const Transform>()
		.each([&](RenderComponent& renderNode, const Transform& t)
			{
				renderQuery.each([&](RenderEnginePtr renderEngine)
					{
						renderEngine.ptr->GetRT()->RC_LambdaAction([=] {
							renderNode.ptr->setPosition(t.Position);
							});
					});
			});

	ecs->system<RenderComponent, ScriptComponent, CameraController>()
		.each([&](RenderComponent& renderNode, ScriptComponent& script, CameraController& c)
			{
				auto cam = renderNode.ptr->getCreator()->getCameras().at(0);
				auto t = script.ptr->GetTransform("CameraTransform");
				renderQuery.each([&](RenderEnginePtr renderEngine)
					{
						renderEngine.ptr->GetRT()->RC_LambdaAction([=] {
							cam->setPosition(t.Position);
							cam->setOrientation(t.Orientation);
							});
					});
			});
}