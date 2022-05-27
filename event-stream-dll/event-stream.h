#pragma once
#include <WinDef.h>

#define DLLEXPORT extern "C" __declspec(dllexport)

// stackoverflow.com/questions/3207365/how-to-use-rundll32-to-execute-dll-function

//extern "C" DLLEXPORT EntryPoint(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine,
//                                int nCmdShow);


extern "C" DLLEXPORT int StreamEvents(LPWSTR channel_path,
                                                  LPWSTR output_file_name);

extern "C" DLLEXPORT int StartStreamEventsThread(LPWSTR channel_path,
                                      LPWSTR output_file_name);


extern "C" DLLEXPORT int HelloWorld0();
extern "C" DLLEXPORT int HelloWorld1(int test);
extern "C" DLLEXPORT int HelloWorld2(LPWSTR test);
