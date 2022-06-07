# vt-windows-event-stream
Tool to stream windows events to a file.
This is a demo tool inspired by:
  * https://docs.microsoft.com/en-us/windows/win32/wes/subscribing-to-events
  * https://docs.microsoft.com/en-us/sysinternals/downloads/sysmon

## Command line usage
Example usage with command line:

```
  vt-windows-event-stream.exe <event log name> <output file>
  vt-windows-event-stream.exe Microsoft-Windows-Sysmon/Operational c:\sysmon.xml
  vt-windows-event-stream.exe Microsoft-Windows-Sysmon/Operational \\server\tmp\sysmon.xml
  vt-windows-event-stream.exe Security \\server\tmp\security.xml
  vt-windows-event-stream.exe Microsoft-Windows-Powershell/Operational \\server\tmp\ps.xml

```


## DLL usage
*  Call DLL directly see [headers](event-stream-dll/event-stream.h)
 * See [importeventlogs.au3](importeventlogs.au3) example

