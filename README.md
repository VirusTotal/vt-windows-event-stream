# vt-windows-event-stream
Tool to stream windows events to a file. 

Example usage:

```
  vt-windows-event-stream.exe <event log name> <output file>
  vt-windows-event-stream.exe Microsoft-Windows-Sysmon/Operational c:\sysmon.xml
  vt-windows-event-stream.exe Microsoft-Windows-Sysmon/Operational \\server\tmp\sysmon.xml
  vt-windows-event-stream.exe Security \\server\tmp\security.xml
  vt-windows-event-stream.exe Microsoft-Windows-Powershell/Operational \\server\tmp\ps.xml

```
