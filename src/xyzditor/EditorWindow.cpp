#include "EditorWindow.h"
#include "imgui.h"
#include "imgui_impl_opengl3_loader.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdlrenderer.h"
#include "imgui_impl_sdl.h"
#include "Common.h"
#include "OgreItem.h"

EditorWindow::EditorWindow(RenderEngine* renderEngine, size_t width, size_t height) : m_pRenderEngine(renderEngine)
{
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	m_SDL_Window = SDL_CreateWindow("SDL Ogre Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, window_flags);

	m_GL_Context = SDL_GL_CreateContext(m_SDL_Window);
	SDL_GL_MakeCurrent(m_SDL_Window, m_GL_Context);
	SDL_GL_SetSwapInterval(0.5);
	m_pSDLRenderer = SDL_CreateRenderer(m_SDL_Window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	IMGUI_CHECKVERSION();

	m_pImGuiContext = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_pImGuiContext);

	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplSDL2_InitForSDLRenderer(m_SDL_Window);
	ImGui_ImplSDLRenderer_Init(m_pSDLRenderer);

	//ImGui_ImplSDL2_InitForOpenGL(m_SDL_Window, m_GL_Context);
	//bool f = ImGui_ImplOpenGL3_Init(SDL_GL_VERSION);
}

void destroyAllAttachedMovableObjects(Ogre::SceneNode* node)
{
	if (!node) return;

	// Destroy all the attached objects
	Ogre::SceneNode::ObjectIterator itObject = node->getAttachedObjectIterator();

	while (itObject.hasMoreElements())
		node->getCreator()->destroyMovableObject(itObject.getNext());

	// Recurse to child SceneNodes
	Ogre::SceneNode::NodeVecIterator itChild = node->getChildIterator();

	while (itChild.hasMoreElements())
	{
		Ogre::SceneNode* pChildNode = static_cast<Ogre::SceneNode*>(itChild.getNext());
		destroyAllAttachedMovableObjects(pChildNode);
	}
}

void destroySceneNode(Ogre::SceneNode* node)
{
	if (!node) return;
	destroyAllAttachedMovableObjects(node);
	node->removeAndDestroyAllChildren();
	node->getCreator()->destroySceneNode(node);
}

EditorWindow::~EditorWindow()
{
	SDL_GL_DeleteContext(m_GL_Context);
	SDL_DestroyWindow(m_SDL_Window);
}

void SceneTreeWindow::Update()
{
	//SDL_GL_MakeCurrent(m_SDL_Window, m_GL_Context);
	ImGui::SetCurrentContext(m_pImGuiContext);
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplSDL2_NewFrame();
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_SDL_Window);
	ImGui::NewFrame();

	ImGui::Begin("Scene");
	if (ImGui::Button("Add", ImVec2(100, 20)))
	{

		m_pRenderEngine->GetRT()->RC_LambdaAction([mgr = m_pRenderEngine->GetSceneManager()] {
			Ogre::Item* item = mgr->createItem("ogrehead.mesh Imported",
				Ogre::ResourceGroupManager::
				AUTODETECT_RESOURCE_GROUP_NAME,
				Ogre::SCENE_DYNAMIC);
			Ogre::SceneNode* sceneNode = mgr->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
			sceneNode->attachObject(item);
			sceneNode->scale(1.f, 1.0f, 1.0f);
			sceneNode->setName("Ogre1");
			});
	}
	DrawSceneTree(m_pRenderEngine->GetSceneManager()->getRootSceneNode());
	ImGui::End();

	ImGui::Render();
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	SDL_SetRenderDrawColor(m_pSDLRenderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
	SDL_RenderClear(m_pSDLRenderer);
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	//SwapFrames();
	SDL_RenderPresent(m_pSDLRenderer);
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
					destroySceneNode(child);
				});
			}
			DrawSceneTree(child);
			ImGui::TreePop();
		}
	}
}

void GameObjectEditor::Update()
{
	//SDL_GL_MakeCurrent(m_SDL_Window, m_GL_Context);
	ImGui::SetCurrentContext(m_pImGuiContext);
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplSDL2_NewFrame();
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_SDL_Window);
	ImGui::NewFrame();

	auto node = m_pSelectedNode;
	if (node) {
		ImGui::Begin("Edit object");


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

		ImGui::End();
	}

	ImGui::Render();
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	SDL_SetRenderDrawColor(m_pSDLRenderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
	SDL_RenderClear(m_pSDLRenderer);
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	//SwapFrames();
	SDL_RenderPresent(m_pSDLRenderer);
}
