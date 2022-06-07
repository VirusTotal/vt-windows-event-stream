// Copyright 2022 The vt-windows-event-stream authors
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


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

