#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <winevt.h>
#include <vector>

#pragma comment(lib, "wevtapi.lib")

#include "event-stream.h"


void PrintUsage(const wchar_t* name) {
  wprintf(L"Usage: %s <LogPath> <output_file_name> [thread]\n", name);
  wprintf(L"\nExamples:\n");
  wprintf(L"  %s Microsoft-Windows-Sysmon/Operational sysmon.xml\n", name);
  wprintf(
      L"  %s Microsoft-Windows-Sysmon/Operational "
      L"\\\\VBOXSVR\\tmp\\sysmon.xml\n",
      name);
  wprintf(L"  %s Security \\\\VBOXSVR\\tmp\\security.xml\n", name);
  wprintf(
      L"  %s Microsoft-Windows-Powershell/Operational "
      L"\\\\VBOXSVR\\tmp\\ps.xml\n",
      name);
}

int __cdecl wmain(int argc, wchar_t* argv[]) {
  LPWSTR channel_path = NULL; // example L"Microsoft-Windows-Sysmon/Operational";
  LPWSTR output_file_name = NULL;

  if (argc < 3) {
    PrintUsage(argv[0]);
    return 0;
  }
  channel_path = argv[1];
  output_file_name = argv[2];

  if (argc == 3) {
    wprintf(L"no thread\n");
    return StreamEvents(channel_path, output_file_name);
  } else {
    wprintf(L"Run thread 60sec\n");
    StartStreamEventsThread(channel_path, output_file_name);
    Sleep(15 * 1000);
    StopEventStreamThreads();
  }
  
}

