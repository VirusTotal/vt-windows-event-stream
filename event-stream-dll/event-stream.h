#pragma once
#include <WinDef.h>

#define DLLEXPORT extern "C" __declspec(dllexport)

extern "C" DLLEXPORT int StreamEvents(LPWSTR channel_path,
                                                  LPWSTR output_file_name);