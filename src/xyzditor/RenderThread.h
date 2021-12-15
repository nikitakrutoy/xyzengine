#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <functional>

typedef unsigned long threadID;
//#include "function2.hpp"

class RenderEngine;

class RenderThread
{
public:
	RenderThread(RenderEngine* pRenderEngine);
	~RenderThread();

	RenderThread(const RenderThread&) = delete;
	RenderThread& operator=(const RenderThread&) = delete;

	void Start();
	void Run();

	void RC_BeginFrame();
	void RC_EndFrame();

	void RC_LambdaAction(std::function<void()> lambdaToAdd);
	void RC_LambdaUIAction(std::function<void()> lambdaToProcess);
	void RC_ProcessLambdaCommands();
	void RC_ProcessUILambdaCommands();
private:
	threadID m_nRenderThreadId;
	threadID m_nMainThreadId;

	volatile uint32_t m_nFlush;

	std::unique_ptr<std::thread> m_pThread;

	RenderEngine* m_pRenderEngine;

	class Node
	{
	public:
		Node* next;
		std::function<void()> f;
	};

	std::atomic<Node*> m_commandQueueHead{ nullptr };
	std::atomic<Node*> m_UIcommandQueueHead{ nullptr };

	int m_nCurrentFrame;
	int m_nFrameFill;

	bool IsRenderThread();

	

	void NextFrame();

	inline bool CheckFlushCond();
	inline void SignalMainThread();
	inline void SignalRenderThread();
	void WaitForMainThreadSignal();
	void WaitForRenderThreadSignal();
	void SyncMainWithRender();
};