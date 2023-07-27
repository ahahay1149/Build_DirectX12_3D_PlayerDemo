#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN                   
#endif
#include <Windows.h>

#include "GamePrograming2Scene.h"
#include "MyAppRunner.h"



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    MyGameEngine engine(960, 540, L"GamePrograming2");
    engine.SetSceneController(new GamePrograming2Scene());

    return MyAppRunner::Run(&engine, hInstance, nCmdShow);
}