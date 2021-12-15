#include "Editor.h"
#include <Windows.h>


int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    Editor* pEditor = new Editor();
    pEditor->Run();

    return 0;
}