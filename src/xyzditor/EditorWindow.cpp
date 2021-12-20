#include "EditorWindow.h"
#include "imgui.h"
#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdlrenderer.h"
#include "imgui_impl_sdl.h"
#include "Common.h"
#include "RenderUtils.h"
#include "SceneLoader.h"

#include <filesystem>

std::string OBJECT_TYPES[] = {
	"Empty",
	"Mesh",
	"Light"
};

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

EditorWindow::EditorWindow(RenderEngine* renderEngine, std::string name, size_t width, size_t height) : m_pRenderEngine(renderEngine), width(width), height(height)
{
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MOUSE_FOCUS);
	m_SDL_Window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, window_flags);

	m_GL_Context = SDL_GL_CreateContext(m_SDL_Window);
	SDL_GL_MakeCurrent(m_SDL_Window, m_GL_Context);
	SDL_GL_SetSwapInterval(0.5);

	IMGUI_CHECKVERSION();

	m_pImGuiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_pImGuiContext);

	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplSDL2_InitForOpenGL(m_SDL_Window, m_GL_Context);
	bool f = ImGui_ImplOpenGL3_Init(SDL_GL_VERSION);
}


EditorWindow::~EditorWindow()
{
	SDL_GL_DeleteContext(m_GL_Context);
	SDL_DestroyWindow(m_SDL_Window);
}

void EditorWindow::StartFrame()
{
	ImGui::SetCurrentContext(m_pImGuiContext);
	SDL_GL_MakeCurrent(m_SDL_Window, m_GL_Context);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void EditorWindow::EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SwapFrames();
}

void SceneTreeWindow::AddObject(Ogre::SceneNode* node) {
	if (ImGui::Button("Add Empty", ImVec2(100, 20)))
	{
		m_pRenderEngine->GetRT()->RC_LambdaAction([node]{
			Ogre::SceneNode * sceneNode = node->createChildSceneNode(Ogre::SCENE_DYNAMIC);
			sceneNode->setName("Default Empty");
			});
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Mesh", ImVec2(100, 20)))
	{
		m_pRenderEngine->GetRT()->RC_LambdaAction([node, mgr = m_pRenderEngine->GetSceneManager()]{
			Ogre::Item * item = OgreUtils::loadMesh("ogrehead.mesh", mgr.get());
			Ogre::SceneNode * sceneNode = node->createChildSceneNode(Ogre::SCENE_DYNAMIC);
			sceneNode->attachObject(item);
			sceneNode->scale(1.f, 1.0f, 1.0f);
			sceneNode->setName("Default Mesh");
			});
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Light", ImVec2(100, 20)))
	{
		m_pRenderEngine->GetRT()->RC_LambdaAction([node, mgr = m_pRenderEngine->GetSceneManager()]{
			Ogre::Light * light = mgr->createLight();
			Ogre::SceneNode * lightNode = node->createChildSceneNode();
			lightNode->attachObject(light);
			light->setPowerScale(Ogre::Math::PI);
			light->setType(Ogre::Light::LT_DIRECTIONAL);
			light->setDirection(Ogre::Vector3(-1, -1, -1).normalisedCopy());
			lightNode->setName("Default Light");
			light->setAttenuation(100, 1.0, 0.045, 0.0075);
			});
	}
}

void SceneTreeWindow::Update()
{
	StartFrame();
	{
		ImGui::SetNextWindowSize(ImVec2(width, height));
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
		ImGui::Begin("Scene", (bool*)0, window_flags);


		ImGui::InputText("Scene name", sceneName, IM_ARRAYSIZE(sceneName));
		if (ImGui::Button("Save", ImVec2(100, 20))) {
			auto scenePath = Ogre::ResourceGroupManager::getSingleton().listResourceLocations("Scenes")->at(0);
			scenePath += "/" + std::string(sceneName);
			SceneLoader::StoreJSON(m_pRenderEngine->GetSceneManager().get(), scenePath);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load", ImVec2(100, 20))) {
			m_pSelectedNode = nullptr;
			auto scenePath = Ogre::ResourceGroupManager::getSingleton().listResourceLocations("Scenes")->at(0);
			scenePath += "/" + std::string(sceneName);
			m_pRenderEngine->GetRT()->RC_LambdaAction([re = m_pRenderEngine, scenePath] {
				SceneLoader::LoadJSON(re->GetSceneManager().get(), nullptr, scenePath);
				});
		}
		AddObject(m_pRenderEngine->GetSceneManager()->getRootSceneNode(Ogre::SCENE_DYNAMIC));

		DrawSceneTree(m_pRenderEngine->GetSceneManager()->getRootSceneNode());
		ImGui::End();
	}
	EndFrame();
}

void SceneTreeWindow::DrawSceneTree(Ogre::SceneNode* node)
{
	auto iterator = node->getChildIterator();
	while (iterator.hasMoreElements()) {
		Ogre::SceneNode* child = static_cast<Ogre::SceneNode*>(iterator.getNext());
		if (ImGui::TreeNode(child->getName().c_str())) {
			if (ImGui::Button("Edit", ImVec2(100, 20))) {
				m_pSelectedNode = child;
			};
			ImGui::SameLine();
			if (ImGui::Button("Delete", ImVec2(100, 20))) {
				if (m_pSelectedNode == child)
					m_pSelectedNode = nullptr;
				m_pRenderEngine->GetRT()->RC_LambdaAction([child] {
					OgreUtils::destroySceneNode(child);
				});
			}
			AddObject(child);
			DrawSceneTree(child);
			ImGui::TreePop();
		}
	}
}

void GameObjectEditor::Update()
{
	StartFrame();
	auto node = m_pSelectedNode;
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("Edit Object", (bool*)0, window_flags);
	if (node) {
		ImGui::InputText("Name", objectName, IM_ARRAYSIZE(objectName));
		ImGui::SameLine();
		if (ImGui::Button("Rename", ImVec2(100, 20)))
			m_pRenderEngine->GetRT()->RC_LambdaAction([this] {
				m_pSelectedNode->setName(Ogre::String(objectName));
			});
			

		auto pos = node->getPosition();
		ImGui::DragFloat3("Position", &pos.x, 0.005f);
		m_pRenderEngine->GetRT()->RC_LambdaAction([node, pos] {
			node->setPosition(pos);
			});

		auto scale = node->getScale();
		ImGui::DragFloat3("Scale", &scale.x, 0.005f);
		m_pRenderEngine->GetRT()->RC_LambdaAction([node, scale] {
			node->setScale(scale);
		});

		bool prevVisibility = visibility;
		ImGui::Checkbox("Visibility", &visibility);
		if (prevVisibility != visibility)
			m_pRenderEngine->GetRT()->RC_LambdaAction([node, prevVisibility]{
							node->setVisible(!prevVisibility);
				});

		ImGui::Text("Object Type"); ImGui::SameLine(); ImGui::Text(objectType.c_str());

		auto scripts = Ogre::ResourceGroupManager::getSingleton().listResourceNames("Scripts");
		if (ImGui::BeginCombo("Script", scriptName.c_str(), 0))
		{
			for (int n = 0; n < scripts->size(); n++)
			{
				std::string selectedScriptName = scripts->at(n);
				if (ImGui::Selectable(selectedScriptName.c_str()) && !selectedScriptName.empty()) {
					node->getUserObjectBindings().setUserAny("scriptName", Ogre::Any(selectedScriptName));
					scriptName = selectedScriptName;
				}
			}
			ImGui::EndCombo();
		}

		auto models = Ogre::ResourceGroupManager::getSingleton().listResourceNames("Models");
		if (objectType == "Item") {
			int flags = 0;
			if (ImGui::BeginCombo("Model", meshName.c_str(), flags))
			{
				for (int n = 0; n < models->size(); n++)
				{
					std::string selectedMeshName = models->at(n);
					if (ImGui::Selectable(selectedMeshName.c_str())) {
						m_pRenderEngine->GetRT()->RC_LambdaAction([selectedMeshName, node, mgr = m_pRenderEngine->GetSceneManager()]{
							OgreUtils::destroyAttachedItem(node);
							node->attachObject(OgreUtils::loadMesh(selectedMeshName, mgr.get()));
						});
						meshName = selectedMeshName;
					}
				}
				ImGui::EndCombo();
			}
		}
		if (objectType == "Light") {
			int prevLightType = lightType;
			ImGui::SliderInt("Light Type", &lightType, 0, 2, "%d", ImGuiSliderFlags_NoInput);
			HelpMarker(
				"0 - Directional.\n"
				"1 - Point.\n"
				"2 - Spotlight.");

			if (prevLightType != lightType) {
				m_pRenderEngine->GetRT()->RC_LambdaAction([this]{
					light->setType(Ogre::Light::LightTypes(lightType));
				});
			}

			auto dir = light->getDirection();
			ImGui::DragFloat3("Direction", &dir.x, 0.005f);
			m_pRenderEngine->GetRT()->RC_LambdaAction([this, dir] {
				light->setDirection(dir);
				});

			auto scale = light->getPowerScale();
			ImGui::DragFloat("Power Scale", &scale, 0.005f);
			m_pRenderEngine->GetRT()->RC_LambdaAction([this, scale] {
				light->setPowerScale(scale);
			});
		}
	}
	else {
		ImGui::Text("Select Node in Scene Tree");
	}
	ImGui::End();

	EndFrame();
}

void GameObjectEditor::SetSelected(Ogre::SceneNode* node)
{
	if (node == m_pSelectedNode) return;
	m_pSelectedNode = node;
	if (node) {
		memset(objectName, 0, 128);
		strncpy(objectName, node->getName().c_str(), node->getName().length());
		auto objects = node->getAttachedObjectIterator();
		auto bindigns = node->getUserObjectBindings();
		auto script = bindigns.getUserAny("scriptName");
		objectType = "Empty";
		if (!script.isEmpty())
			scriptName = Ogre::any_cast<std::string>(script);
		else
			scriptName = std::string();
		while (objects.hasMoreElements()) {
			auto obj = objects.getNext();
			objectType = obj->getMovableType();
			visibility = obj->isVisible();
			if (objectType == "Item") {
				item = dynamic_cast<Ogre::Item*>(obj);
				meshName = item->getName();
			}
			if (objectType == "Light") {
				light = dynamic_cast<Ogre::Light*>(obj);
				lightType = light->getType();
			}
		}
	}
}

 void DemoWindow::Update() {
	StartFrame();
	ImGui::ShowDemoWindow();
	EndFrame();
}

 void CameraWindow::Update()
 {
	 StartFrame();
	 ImGui::SetNextWindowSize(ImVec2(width, height));
	 ImGui::SetNextWindowPos(ImVec2(0, 0));
	 ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	 ImGui::Begin("Camera", (bool*)0, window_flags);
	 auto cam = m_pRenderEngine->GetMainCamera();
	 auto pos = cam->getPosition();
	 ImGui::DragFloat3("Position", &pos.x, 0.05f);
	 m_pRenderEngine->GetRT()->RC_LambdaAction([cam, pos] {
		 cam->setPosition(pos);
		 });

	 auto fclip = cam->getFarClipDistance();
	 ImGui::DragFloat("Far Clip Distance", &fclip, 0.05f);
	 m_pRenderEngine->GetRT()->RC_LambdaAction([cam, fclip] {
		 cam->setFarClipDistance(fclip > 0.1 ? fclip : 0.1);
	});

	 auto nclip = cam->getNearClipDistance();
	 ImGui::DragFloat("Near Clip Distance", &nclip, 0.05f);
	 m_pRenderEngine->GetRT()->RC_LambdaAction([cam, nclip] {
		 cam->setNearClipDistance(nclip > 0.1 ? nclip : 0.1);
	});

	 ImGui::End();
	 EndFrame();
 }
