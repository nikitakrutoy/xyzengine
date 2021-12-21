#include "ecsSceneObject.h"

void register_ecs_control_systems(flecs::world* ecs)
{
	static auto inputQuery = ecs->query<InputHandlerPtr>();

	ecs->system<const Controllable, ScriptComponent, Transform>()
		.each([&](flecs::entity e, const Controllable&, ScriptComponent& script, Transform& t)
			{
				t = script.ptr->GetTransform();
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
}