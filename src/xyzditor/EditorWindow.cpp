#include "EditorWindow.h"
#include "imgui.h"
#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdlrenderer.h"
#include "imgui_impl_sdl.h"
#include "Common.h"
#include "OgreUtils.h"
#include "SceneLoader.h"

#include <filesystem>

EditorWindow::EditorWindow(RenderEngine* renderEngine, std::string name, size_t width, size_t height) : m_pRenderEngine(renderEngine)
{
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	m_SDL_Window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);

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

void SceneTreeWindow::Update()
{
	ImGui::SetCurrentContext(m_pImGuiContext);
	SDL_GL_MakeCurrent(m_SDL_Window, m_GL_Context);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	{
		bool f = true;
		ImGui::ShowDemoWindow(&f);
		ImGui::Begin("Scene");


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
				SceneLoader::LoadJSON(re->GetSceneManager().get(), scenePath);
				});
		}

		if (ImGui::Button("Add", ImVec2(100, 20)))
		{
			m_pRenderEngine->GetRT()->RC_LambdaAction([mgr = m_pRenderEngine->GetSceneManager()]{
				Ogre::Item * item = mgr->createItem("ogrehead.mesh",
					Ogre::ResourceGroupManager::
					AUTODETECT_RESOURCE_GROUP_NAME,
					Ogre::SCENE_DYNAMIC);
				Ogre::SceneNode * sceneNode = mgr->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
				sceneNode->attachObject(item);
				sceneNode->scale(1.f, 1.0f, 1.0f);
				sceneNode->setName("Default");
				});
		}
		DrawSceneTree(m_pRenderEngine->GetSceneManager()->getRootSceneNode());
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SwapFrames();
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
			if (ImGui::Button("Delete", ImVec2(100, 20))) {
				if (m_pSelectedNode == child)
					m_pSelectedNode = nullptr;
				m_pRenderEngine->GetRT()->RC_LambdaAction([child] {
					OgreUtils::destroySceneNode(child);
				});
			}
			DrawSceneTree(child);
			ImGui::TreePop();
		}
	}
}

void GameObjectEditor::Update()
{
	ImGui::SetCurrentContext(m_pImGuiContext);
	SDL_GL_MakeCurrent(m_SDL_Window, m_GL_Context);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	auto node = m_pSelectedNode;
	ImGui::Begin("Edit object");
	if (node) {
		ImGui::InputText("Name", objectName, IM_ARRAYSIZE(objectName));
		ImGui::SameLine();
		if (ImGui::Button("Rename", ImVec2(100, 20)))
			m_pRenderEngine->GetRT()->RC_LambdaAction([this] {
				m_pSelectedNode->setName(Ogre::String(objectName));
			});
			

		auto pos = node->getPosition();
		ImGui::SliderFloat3("Position", &pos.x, -10.f, 10.f, "%.3f");
		m_pRenderEngine->GetRT()->RC_LambdaAction([node, pos] {
			node->setPosition(pos);
			});

		auto scale = node->getScale();
		ImGui::SliderFloat3("Scale", &scale.x, -10.f, 10.f, "%.3f");
		
		m_pRenderEngine->GetRT()->RC_LambdaAction([node, scale] {
			node->setScale(scale);
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
	}
	else {
		ImGui::Text("Select Node in Scene Tree");
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SwapFrames();
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
		if (!script.isEmpty())
			scriptName = Ogre::any_cast<std::string>(script);
		else
			scriptName = std::string();
		while (objects.hasMoreElements()) {
			auto obj = objects.getNext();
			objectType = obj->getMovableType();
			if (objectType == "Item") {
				auto item = dynamic_cast<Ogre::Item*>(obj);
				meshName = item->getMesh().get()->getName();
			}
		}
	}
}
