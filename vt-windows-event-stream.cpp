#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include <winevt.h>

#pragma comment(lib, "wevtapi.lib")

// Max events to process in one call
#define EVENTS_ARRAY_SIZE 10

DWORD EnumerateResults(EVT_HANDLE hResults, HANDLE outFile);
DWORD OutputEvent(EVT_HANDLE hEvent, HANDLE outFile);

void PrintUsage(const wchar_t* name) {
  wprintf(L"Usage: %s <LogPath> <OutputFile>\n", name);
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
  DWORD status = ERROR_SUCCESS;
  EVT_HANDLE hSubscription = NULL;
  LPWSTR pwsPath = L"Microsoft-Windows-Sysmon/Operational";
  LPWSTR outputFile = NULL;
  LPWSTR pwsQuery = L"*";
  HANDLE signalEvent = NULL;
  HANDLE outputHandle = NULL;

  DWORD dwWait = 0;

  if (argc != 3) {
    PrintUsage(argv[0]);
    return 0;
  }
  pwsPath = argv[1];
  outputFile = argv[2];

  signalEvent = CreateEvent(NULL, TRUE, TRUE, NULL);
  if (NULL == signalEvent) {
    wprintf(L"CreateEvent failed with %lu.\n", GetLastError());
    goto cleanup;
  }

  // Subscribe to events.
  hSubscription = EvtSubscribe(NULL, signalEvent, pwsPath, pwsQuery, NULL, NULL,
                               NULL, EvtSubscribeToFutureEvents);
  // hSubscription = EvtSubscribe(NULL, aWaitHandles[1], pwsPath, pwsQuery,
  // NULL, NULL, NULL, EvtSubscribeStartAtOldestRecord);
  if (NULL == hSubscription) {
    status = GetLastError();

    if (ERROR_EVT_CHANNEL_NOT_FOUND == status)
      wprintf(L"Channel %s was not found.\n", pwsPath);
    else if (ERROR_EVT_INVALID_QUERY == status)
      wprintf(L"The query %s was not found.\n", pwsQuery);
    else
      wprintf(L"EvtSubscribe failed with %lu.\n", status);

    goto cleanup;
  }

  outputHandle = CreateFileW(outputFile, GENERIC_WRITE, FILE_SHARE_READ,
                             NULL,                   // default security
                             CREATE_ALWAYS,          // always create new file
                             FILE_ATTRIBUTE_NORMAL,  // normal file
                             NULL);                  // no attr. template)

  if (outputHandle == INVALID_HANDLE_VALUE) {
    wprintf(L"Unable to open output file: %s\n", outputFile);
    goto cleanup;
  }
  wprintf(L"Press control-C to quit.\n");

  // Loop until the user presses a key or there is an error.
  while (true) {
    dwWait = WaitForSingleObject(signalEvent, INFINITE);
    if (dwWait == WAIT_OBJECT_0) {
      if (ERROR_NO_MORE_ITEMS !=
          (status = EnumerateResults(hSubscription, outputHandle))) {
        break;
      }

      ResetEvent(signalEvent);
    } else if (dwWait == WAIT_ABANDONED) {
      wprintf(L"WaitForSingleObject WAIT_ABANDONED\n");
    } else {
      wprintf(L"WaitForSingleObject %x.\n", dwWait);
    }
  }

cleanup:

  if (hSubscription) EvtClose(hSubscription);

  if (signalEvent) CloseHandle(signalEvent);
  return 0;
}

// Enumerate the events in the result set.
DWORD EnumerateResults(EVT_HANDLE hResults, HANDLE outputHandle) {
  DWORD status = ERROR_SUCCESS;
  EVT_HANDLE hEvents[EVENTS_ARRAY_SIZE];
  DWORD dwReturned = 0;

  while (true) {
    // Get a block of events from the result set.
    if (!EvtNext(hResults, EVENTS_ARRAY_SIZE, hEvents, INFINITE, 0, &dwReturned)) {
      if (ERROR_NO_MORE_ITEMS != (status = GetLastError())) {
        wprintf(L"EvtNext failed with %lu\n", status);
      }

      goto cleanup;
    }

    // For each event, call the OutputEvent function which renders the
    // event for display.
    for (DWORD i = 0; i < dwReturned; i++) {
      if (ERROR_SUCCESS == (status = OutputEvent(hEvents[i], outputHandle))) {
        EvtClose(hEvents[i]);
        hEvents[i] = NULL;
      } else {
        goto cleanup;
      }
    }
  }

cleanup:

  // Closes any events in case an error occurred above.
  for (DWORD i = 0; i < dwReturned; i++) {
    if (NULL != hEvents[i]) EvtClose(hEvents[i]);
  }

  return status;
}

// Output the event XML
DWORD OutputEvent(EVT_HANDLE hEvent, HANDLE outputHandle) {
  DWORD status = ERROR_SUCCESS;
  DWORD dwBufferSize = 0;
  DWORD dwBufferUsed = 0;
  DWORD dwPropertyCount = 0;
  LPWSTR pRenderedContent = NULL;
  LPSTR utf8Content = NULL;
  DWORD utf8BuffSize = 0;
  DWORD dwBytesWritten = 0;
  int ret = 0;
  DWORD len = 0;

  // The EvtRenderEventXml flag tells EvtRender to render the event as an XML
  // string.
  if (!EvtRender(NULL, hEvent, EvtRenderEventXml, dwBufferSize,
                 pRenderedContent, &dwBufferUsed, &dwPropertyCount)) {
    if (ERROR_INSUFFICIENT_BUFFER == (status = GetLastError())) {
      dwBufferSize = dwBufferUsed;
      pRenderedContent = (LPWSTR)malloc(dwBufferSize);
      if (pRenderedContent) {
        EvtRender(NULL, hEvent, EvtRenderEventXml, dwBufferSize,
                  pRenderedContent, &dwBufferUsed, &dwPropertyCount);
      } else {
        wprintf(L"malloc failed\n");
        status = ERROR_OUTOFMEMORY;
        goto cleanup;
      }
    }

    if (ERROR_SUCCESS != (status = GetLastError())) {
      wprintf(L"EvtRender failed with %d\n", GetLastError());
      goto cleanup;
    }
  }

  utf8BuffSize = dwBufferSize * 4;  // 4X is worst case
  utf8Content = (LPSTR)calloc(utf8BuffSize, 1);

  // copy to utf8
  snprintf(utf8Content, utf8BuffSize, "%S\n", pRenderedContent);

  len = (DWORD)strlen(utf8Content);

  if (WriteFile(outputHandle, utf8Content, len, &dwBytesWritten, NULL)) {
    if (len != dwBytesWritten) {
      wprintf(L"WriteFile failed with %d.  buffSize=%d  Written=%d\n",
              GetLastError(), dwBufferSize, dwBytesWritten);
    }
  } else {
    // error
    wprintf(L"WriteFile failed: %d.\n", GetLastError());
  }

cleanup:

  if (pRenderedContent) free(pRenderedContent);

  if (utf8Content) free(utf8Content);

  return status;
}
