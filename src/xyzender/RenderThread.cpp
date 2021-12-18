#include "RenderThread.h"
#include "RenderEngine.h"

// Creating Critical section interface
std::mutex RC_CriticalSection;

// Function to run render thread
static unsigned RunThisThread(void* thisPtr)
{
	RenderThread* const self = (RenderThread*)thisPtr;
	self->Run();

	return 0;
}

RenderThread::RenderThread(RenderEngine* pRenderEngine) :
	m_pRenderEngine(pRenderEngine),
	m_nRenderThreadId(0),
	m_nCurrentFrame(0),
	m_nFrameFill(1),
	m_nFlush(0),
	m_pThread(nullptr)
{
	m_nMainThreadId = ::GetCurrentThreadId();
}

RenderThread::~RenderThread()
{

}

// Render Loop
void RenderThread::Run()
{
	m_nRenderThreadId = ::GetCurrentThreadId();

	while (true)
	{
		WaitForMainThreadSignal();

		RC_BeginFrame();

		RC_ProcessLambdaCommands();

		SignalMainThread();

		m_pRenderEngine->Update();

		RC_EndFrame();

		if (m_pRenderEngine->GetQuit())
			break;
	}
}

void RenderThread::Start()
{
	if (!m_pThread)
	{
		m_pThread = std::unique_ptr<std::thread>(new std::thread(RunThisThread, this));
	}
}

bool RenderThread::IsRenderThread()
{
	return m_nRenderThreadId == ::GetCurrentThreadId();
}

void RenderThread::NextFrame()
{
	m_nCurrentFrame = (m_nCurrentFrame + 1) & 1;
	m_nFrameFill = (m_nFrameFill + 1) & 1;
}

bool RenderThread::CheckFlushCond()
{
	return *(int*)&m_nFlush != 0;
}


void RenderThread::SignalMainThread()
{
	m_nFlush = 0;
}


void RenderThread::SignalRenderThread()
{
	m_nFlush = 1;
}

void RenderThread::RC_BeginFrame()
{

}

void RenderThread::RC_EndFrame()
{
	if (IsRenderThread())
		return;

	SyncMainWithRender();
}

void RenderThread::SyncMainWithRender()
{
	assert(!IsRenderThread());

	WaitForRenderThreadSignal();

	{
		std::scoped_lock<std::mutex> criticalSection(RC_CriticalSection);
		NextFrame();
	}

	SignalRenderThread();
}

// Wait signal from main thread
void RenderThread::WaitForMainThreadSignal()
{
	assert(IsRenderThread());

	while (!m_nFlush)
	{
	}
}

// Wait signal from render thread
void RenderThread::WaitForRenderThreadSignal()
{
	assert(!IsRenderThread());

	while (m_nFlush)
	{
	}
}

void RenderThread::RC_LambdaAction(std::function<void()> lambdaToProcess) {
	auto node = new Node{ nullptr, std::move(lambdaToProcess) };
	auto prev_head = m_commandQueueHead.load();
	do
	{
		node->next = prev_head;
	} while (!m_commandQueueHead.compare_exchange_weak(prev_head, node));
}

void RenderThread::RC_LambdaUIAction(std::function<void()> lambdaToProcess) {
	auto node = new Node{ nullptr, std::move(lambdaToProcess) };
	auto prev_head = m_UIcommandQueueHead.load();
	do
	{
		node->next = prev_head;
	} while (!m_UIcommandQueueHead.compare_exchange_weak(prev_head, node));
}


void RenderThread::RC_ProcessUILambdaCommands()
{
	assert(IsRenderThread());

	if (!CheckFlushCond())
		return;

	// A cost to pay for easiest lock-free stack
	std::vector<std::function<void()>> reversed;

	auto stolen_head = m_UIcommandQueueHead.exchange(nullptr);
	while (stolen_head != nullptr) {
		//Simple and sad 
		reversed.insert(reversed.begin(), std::move(stolen_head->f));
		auto next = stolen_head->next;
		delete stolen_head;
		stolen_head = next;
	}

	for (auto& f : reversed)
	{
		f();
	}
	reversed.clear();
}

void RenderThread::RC_ProcessLambdaCommands()
{
	assert(IsRenderThread());

	if (!CheckFlushCond())
		return;

	// A cost to pay for easiest lock-free stack
	std::vector<std::function<void()>> reversed;

	auto stolen_head = m_commandQueueHead.exchange(nullptr);
	while (stolen_head != nullptr) {
		//Simple and sad 
		reversed.insert(reversed.begin(), std::move(stolen_head->f));
		auto next = stolen_head->next;
		delete stolen_head;
		stolen_head = next;
	}

	for (auto& f : reversed)
	{
		f();
	}
	reversed.clear();
}