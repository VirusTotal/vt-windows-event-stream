#pragma once
#include <WinDef.h>

#define DLLEXPORT extern "C" __declspec(dllexport)

// note if we want to use rundll do something like
// stackoverflow.com/questions/3207365/how-to-use-rundll32-to-execute-dll-function


extern "C" DLLEXPORT int StreamEvents(LPWSTR channel_path,
                                                  LPWSTR output_file_name);

extern "C" DLLEXPORT int StartStreamEventsThread(LPWSTR channel_path,
                                      LPWSTR output_file_name);


extern "C" DLLEXPORT int StopEventStreamThreads();

