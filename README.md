# vt-windows-event-stream
Stream windows events to a file. 

Example usage:

```
  vt-windows-event-stream.exe Microsoft-Windows-Sysmon/Operational sysmon.xml
  vt-windows-event-stream.exe Microsoft-Windows-Sysmon/Operational \\server\tmp\sysmon.xml
  vt-windows-event-stream.exe Security \\server\tmp\security.xml
  vt-windows-event-stream.exe Microsoft-Windows-Powershell/Operational \\server\tmp\ps.xml

```