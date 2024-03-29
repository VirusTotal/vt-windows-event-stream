
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

// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <winevt.h>
#include <vector>
#include "event-stream.h"

#pragma comment(lib, "wevtapi.lib")

static const int kMaxThreads = 16;
static int event_threads_started = 0;
const int kMaxParamLen = 256;
struct EventSreamContext {
    WCHAR channel_path[kMaxParamLen];
    WCHAR output_file_name[kMaxParamLen];
    HANDLE thread_handle;
};
static bool keep_running = true;



static struct EventSreamContext thread_data[kMaxThreads];

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
      printf("DllMain DLL_PROCESS_ATTACH:\n");
      // init
      memset(thread_data, 0, kMaxThreads  * sizeof(struct EventSreamContext));

      break;
    case DLL_THREAD_ATTACH:
      printf("DllMain DLL_THREAD_ATTACH:\n");
      break;
    case DLL_THREAD_DETACH:
      printf("DllMain DLL_THREAD_DETACH:\n");
      break;
    case DLL_PROCESS_DETACH:
      printf("DllMain DLL_PROCESS_DETACH:\n");
      break;
    default:
      printf("DllMain default:\n");
      break;
  }
  return TRUE;
}


DWORD EnumerateResults(EVT_HANDLE hResults, HANDLE outFile);


BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
  keep_running = FALSE;

  switch (fdwCtrlType) {
      // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
      printf("Ctrl-C event\n\n");
      return TRUE;

      // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
      printf("Ctrl-Close event\n\n");
      return TRUE;

      // Pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
      printf("Ctrl-Break event\n\n");
      return FALSE;

    case CTRL_LOGOFF_EVENT:
      printf("Ctrl-Logoff event\n\n");
      return FALSE;

    case CTRL_SHUTDOWN_EVENT:
      printf("Ctrl-Shutdown event\n\n");
      return FALSE;

    default:
      return FALSE;
  }
}

// Output the event XML
DWORD OutputEvent(EVT_HANDLE hEvent, HANDLE output_handle) {
  DWORD status = ERROR_SUCCESS;
  DWORD unicode_buff_size = 0;
  DWORD unicode_buff_used = 0;
  DWORD property_count = 0;
  std::vector<wchar_t> xml_unicode;
  DWORD utf8_buff_size = 0;
  DWORD bytes_written = 0;
  DWORD utf8_len = 0;

  // EvtRenderEventXml flag to render the event as an XML string.
  if (!EvtRender(NULL, hEvent, EvtRenderEventXml,
                 0,     // buffer size
                 NULL,  // buffer
                 &unicode_buff_used, &property_count)) {
    if (ERROR_INSUFFICIENT_BUFFER == (status = GetLastError())) {
      unicode_buff_size = unicode_buff_used;
      xml_unicode.resize(unicode_buff_used);

      EvtRender(NULL, hEvent, EvtRenderEventXml, unicode_buff_used,
                xml_unicode.data(), &unicode_buff_used, &property_count);
    }

    status = GetLastError();
    if (status != ERROR_SUCCESS) {
      wprintf(L"EvtRender failed with %d\n", status);
      return status;
    }
  }

  utf8_buff_size = unicode_buff_size * 4;  // 4X is worst case
  std::vector<char> utf8_content(utf8_buff_size);

  // copy to utf8
  snprintf(utf8_content.data(), unicode_buff_size, "%S\n", xml_unicode.data());

  utf8_len = (DWORD)strlen(utf8_content.data());

  if (WriteFile(output_handle, utf8_content.data(), utf8_len, &bytes_written,
                NULL)) {
    if (utf8_len != bytes_written) {
      wprintf(L"WriteFile failed with %d.  buffSize=%d  Written=%d\n",
              GetLastError(), unicode_buff_size, bytes_written);
    }
  } else {
    // error
    wprintf(L"WriteFile failed: %d.\n", GetLastError());
  }
  FlushFileBuffers(output_handle);

  return status;
}

// Process results from subscription result set.
// Enumerate the events in the result set and send to output_handle
DWORD ProcessResults(EVT_HANDLE result_set, HANDLE ouput_handle) {
  const int kEventsArraySize = 10;
  DWORD status = ERROR_SUCCESS;
  EVT_HANDLE event_handles[kEventsArraySize];
  DWORD events_returned = 0;

  while (keep_running) {
    // Get a block of events from the result set.
    if (!EvtNext(result_set, kEventsArraySize, event_handles, INFINITE, 0,
                 &events_returned)) {
      status = GetLastError();
      if (status != ERROR_NO_MORE_ITEMS) {
        wprintf(L"EvtNext failed with %lu\n", status);
      }
      break;  // goto cleanup
    }

    // For each event, call the OutputEvent function which renders the
    // event for display.
    for (DWORD i = 0; i < events_returned; i++) {
      status = OutputEvent(event_handles[i], ouput_handle);
      if (ERROR_SUCCESS == status) {
        EvtClose(event_handles[i]);
        event_handles[i] = NULL;
      } else {
        goto cleanup;
      }
    }
  }

cleanup:

  // Closes any events in case an error occurred above.
  for (DWORD i = 0; i < events_returned; i++) {
    if (NULL != event_handles[i]) {
      EvtClose(event_handles[i]);
    }
  }

  return status;
}





DLLEXPORT int StreamEvents(LPWSTR channel_path, LPWSTR output_file_name) {
  EVT_HANDLE subscription_handle = NULL;
  HANDLE output_handle = NULL;
  LPCWSTR subscription_query = L"*";
  HANDLE signal_event = NULL;
  DWORD status = ERROR_SUCCESS;
  DWORD wait_ret = 0;

  if (!channel_path || !output_file_name) {
    wprintf(L"Invalid input %ls  %ls\n", channel_path, output_file_name);
    return 1;
  }

  signal_event = CreateEvent(NULL, TRUE, TRUE, NULL);
  if (NULL == signal_event) {
    wprintf(L"CreateEvent failed with %lu.\n", GetLastError());
    return 1;
  }

  // Subscribe to events.
  subscription_handle =
      EvtSubscribe(NULL, signal_event, channel_path, subscription_query, NULL,
                   NULL, NULL, EvtSubscribeToFutureEvents);
  if (NULL == subscription_handle) {
    status = GetLastError();

    if (ERROR_EVT_CHANNEL_NOT_FOUND == status)
      wprintf(L"Channel %s was not found.\n", channel_path);
    else if (ERROR_EVT_INVALID_QUERY == status)
      wprintf(L"The query %s was not found.\n", subscription_query);
    else
      wprintf(L"EvtSubscribe failed with %lu.\n", status);

    CloseHandle(signal_event);

    wprintf(L"Stream events exit\n");
    return 1;
  }

  output_handle = CreateFileW(output_file_name, GENERIC_WRITE, FILE_SHARE_READ,
                              NULL,                   // default security
                              CREATE_ALWAYS,          // always create new file
                              FILE_ATTRIBUTE_NORMAL,  // normal file
                              NULL);                  // no attr. template)

  if (output_handle == INVALID_HANDLE_VALUE) {
    wprintf(L"Unable to open output file: %ls\n", output_file_name);
    EvtClose(subscription_handle);
    CloseHandle(signal_event);
    return 1;
  }
  wprintf(L"Press control-C to quit.\n");

  // Loop until the user does a control-C
  while (keep_running) {
    wait_ret = WaitForSingleObject(signal_event, 1000);

    if (wait_ret == WAIT_OBJECT_0) {
      status = ProcessResults(subscription_handle, output_handle);
      if (status != ERROR_NO_MORE_ITEMS) {
        break;
      }

      ResetEvent(signal_event);
    } else if (wait_ret == WAIT_TIMEOUT) {
      // intended to timeout every second so we can exit on keep running change
      continue;
    } else if (wait_ret == WAIT_ABANDONED) {
      wprintf(L"WaitForSingleObject WAIT_ABANDONED\n");
    } else {
      wprintf(L"WaitForSingleObject %x.\n", wait_ret);
    }
  }

  if (subscription_handle) {
    EvtClose(subscription_handle);
  }

  if (signal_event) {
    CloseHandle(signal_event);
  }
  return 0;
}


DWORD WINAPI StreamEventParams(LPVOID lpParam) {
  struct EventSreamContext *params = (struct EventSreamContext* ) lpParam; // thread local copy

  wprintf(L"StreamEventParams %s %s\n", params->channel_path, params->output_file_name);

  return StreamEvents(params->channel_path, params->output_file_name);
}


DLLEXPORT int StartStreamEventsThread(LPWSTR channel_path, LPWSTR output_file_name) {


  wprintf(L"StartStreamEventsThread start\n");

  if (event_threads_started > kMaxThreads) {
    wprintf(L"Too many threads\n");
    return 1;
  }

  memset(&thread_data[event_threads_started], 0, sizeof(struct EventSreamContext)); // init
  wcscpy_s(thread_data[event_threads_started].channel_path, kMaxParamLen - 1, channel_path);
  wcscpy_s(thread_data[event_threads_started].output_file_name, kMaxParamLen - 1, output_file_name);

  thread_data[event_threads_started].thread_handle =
      CreateThread(NULL,                  // default security attributes
                   0,                     // use default stack size
                   StreamEventParams,     // thread function name
                   &thread_data[event_threads_started], // argument to thread function
                   0,                     // use default creation flags
                   NULL);  // returns the thread identifier. NULL ignore

  if (thread_data[event_threads_started].thread_handle == NULL) {
    wprintf(L"CreateThread failed with %d\n", GetLastError());
    return 1;
  }
  event_threads_started++;

  wprintf(L"Thread started %s %s\n", channel_path, output_file_name);
  return 0;
}

DLLEXPORT int StopEventStreamThreads() {
    keep_running = false;

    for (int i = 0; i < event_threads_started; i++) {
        WaitForSingleObject(thread_data[i].thread_handle, 3000);
    }

    return 0;
}
