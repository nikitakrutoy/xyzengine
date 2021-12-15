#include "RenderEngine.h"

#include "RenderEngine.h"

#include "OgreMeshManager2.h"
#include "OgreMesh2.h"
#include "OgreItem.h"

RenderEngine::RenderEngine(ResourceManager* pResourceManager) :
	m_pRoot(nullptr),
	m_pRenderWindow(nullptr),
	m_pSceneManager(nullptr),
	m_pGL3PlusPlugin(nullptr),
	m_pCamera(nullptr),
	m_pWorkspace(nullptr),
	m_pRT(nullptr),
	m_bIsInitialized(false),
	m_bQuit(false),
	m_pResourceManager(pResourceManager)
{
	m_pRT = std::unique_ptr<RenderThread>(new RenderThread(this));

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	m_SDL_Window = SDL_CreateWindow("SDL Ogre Engine ", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	m_pRT->RC_LambdaAction([this] {
		RT_Init();
		RT_SetupDefaultCamera();
		RT_SetupDefaultCompositor();
		RT_LoadDefaultResources();
		RT_SetupDefaultLight();
		RT_LoadOgreHead();
		RT_EndInit();
	});


	m_pRT->Start();
}

RenderEngine::~RenderEngine()
{
	//SAFE_OGRE_DELETE(m_pRoot);
}

bool RenderEngine::SetOgreConfig()
{
#ifdef _DEBUG
	constexpr bool bAlwaysShowConfigWindow = true;
	if (bAlwaysShowConfigWindow || !m_pRoot->restoreConfig())
#else
	if (!m_pRoot->restoreConfig())
#endif
	{
		if (!m_pRoot->showConfigDialog())
		{
			return false;
		}
	}

	return true;
}

void RenderEngine::Update()
{
	Ogre::WindowEventUtilities::messagePump();
	SDL_GL_MakeCurrent(m_SDL_Window, m_GL_Context);
	if (m_pRenderWindow->isVisible())
		m_bQuit |= !m_pRoot->renderOneFrame();

	SDL_GL_SwapWindow(m_SDL_Window);
}

void RenderEngine::RT_Init()
{
	m_pRoot = std::unique_ptr<Ogre::Root>(new Ogre::Root());
	m_pGL3PlusPlugin = std::unique_ptr<Ogre::GL3PlusPlugin>(new Ogre::GL3PlusPlugin());

	m_pRoot->installPlugin(m_pGL3PlusPlugin.get());

	if (!SetOgreConfig())
	{
		m_bQuit = true;
		return;
	}

	m_pRoot->initialise(false);

	RT_InitSDL();

	// Creating window
	Ogre::uint32 width = 1280;
	Ogre::uint32 height = 720;
	Ogre::String sTitleName = "Game Engine";

	Ogre::NameValuePairList params;
	params.insert(std::make_pair("title", sTitleName));
	params.insert(std::make_pair("gamma", "true"));
	params.insert(std::make_pair("FSAA", "false"));
	params.insert(std::make_pair("vsync", "false"));

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(m_SDL_Window, &info);

	params["externalWindowHandle"] = Ogre::StringConverter::toString(reinterpret_cast<size_t>(info.info.win.window));
	params["externalGLContent"] = Ogre::StringConverter::toString(reinterpret_cast<size_t>(info.info.win.hdc));
	params["externalGLControl"] = Ogre::String("True");

	m_pRenderWindow = std::unique_ptr<Ogre::Window>(Ogre::Root::getSingleton().createRenderWindow(sTitleName, width, height, false, &params));

	// Scene manager
	m_pSceneManager = std::unique_ptr<Ogre::SceneManager>(m_pRoot->createSceneManager(Ogre::SceneType::ST_GENERIC, 2));

}

void RenderEngine::RT_SetupDefaultCamera()
{
	m_pCamera = std::unique_ptr<Ogre::Camera>(m_pSceneManager->createCamera("Main Camera"));
	m_pCamera->setPosition(Ogre::Vector3(150, 0, 0));
	m_pCamera->lookAt(Ogre::Vector3(0, 0, 0));
	m_pCamera->setNearClipDistance(0.2f);
	m_pCamera->setFarClipDistance(1000.0f);
	m_pCamera->setAutoAspectRatio(true);
	m_pCamera->setFixedYawAxis(true);
}

void RenderEngine::RT_SetupDefaultCompositor()
{
	Ogre::CompositorManager2* compositorManager = m_pRoot->getCompositorManager2();

	const Ogre::String workspaceName("WorkSpace");

	if (!compositorManager->hasWorkspaceDefinition(workspaceName))
	{
		compositorManager->createBasicWorkspaceDef(workspaceName, Ogre::ColourValue::Blue);
	}

	m_pWorkspace = std::unique_ptr<Ogre::CompositorWorkspace>
		(compositorManager->addWorkspace(m_pSceneManager.get(), m_pRenderWindow->getTexture(), m_pCamera.get(), workspaceName, true));
}

void RenderEngine::RT_LoadDefaultResources()
{
	m_pResourceManager->LoadOgreResources("resources.cfg");
}

void RenderEngine::RT_LoadOgreHead()
{
	//Load the v1 mesh. Notice the v1 namespace
//Also notice the HBU_STATIC flag; since the HBU_WRITE_ONLY
//bit would prohibit us from reading the data for importing.
	Ogre::v1::MeshPtr v1Mesh;
	Ogre::MeshPtr v2Mesh;


	v1Mesh = Ogre::v1::MeshManager::getSingleton().load(
		"ogrehead.mesh", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
		Ogre::v1::HardwareBuffer::HBU_STATIC, Ogre::v1::HardwareBuffer::HBU_STATIC);




	//Create a v2 mesh to import to, with a different name (arbitrary).
	v2Mesh = Ogre::MeshManager::getSingleton().createManual(
		"ogrehead.mesh_imported", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	bool halfPosition = true;
	bool halfUVs = true;
	bool useQtangents = true;

	//Import the v1 mesh to v2
	v2Mesh->importV1(v1Mesh.get(), halfPosition, halfUVs, useQtangents);

	//We don't need the v1 mesh. Free CPU memory, get it out of the GPU.
	//Leave it loaded if you want to use athene with v1 Entity.
	v1Mesh->unload();

	//Create an Item with the model we just imported.
	//Notice we use the name of the imported model. We could also use the overload
	//with the mesh pointer:
	Ogre::Item* item = m_pSceneManager->createItem("ogrehead.mesh_imported",
		Ogre::ResourceGroupManager::
		AUTODETECT_RESOURCE_GROUP_NAME,
		Ogre::SCENE_DYNAMIC);
	item->setName("ogrehead.mesh");
	Ogre::SceneNode* sceneNode = m_pSceneManager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->
		createChildSceneNode(Ogre::SCENE_DYNAMIC);
	sceneNode->attachObject(item);
	sceneNode->scale(1.f, 1.0f, 1.0f);
	sceneNode->setName("Ogre");
}

void RenderEngine::RT_SetupDefaultLight()
{
	// Lightning
	Ogre::Light* light = m_pSceneManager->createLight();
	Ogre::SceneNode* lightNode = m_pSceneManager->getRootSceneNode()->createChildSceneNode();
	lightNode->setName("light2");
	lightNode->attachObject(light);
	light->setPowerScale(Ogre::Math::PI); //Since we don't do HDR, counter the PBS' division by PI
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setDirection(Ogre::Vector3(-1, -1, -1).normalisedCopy());
}

void RenderEngine::RT_EndInit()
{
	m_bIsInitialized = true;
}


void RenderEngine::RT_InitSDL()
{
	m_GL_Context = SDL_GL_CreateContext(m_SDL_Window);
	SDL_GL_MakeCurrent(m_SDL_Window, m_GL_Context);
	SDL_GL_SetSwapInterval(0);
}

void RenderEngine::RT_SDLClenup()
{
	SDL_GL_DeleteContext(m_GL_Context);
	SDL_DestroyWindow(m_SDL_Window);
}