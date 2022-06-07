;  Copyright 2022 The vt-windows-event-stream authors
;  Licensed under the Apache License, Version 2.0 (the "License");
;  you may not use this file except in compliance with the License.
;  You may obtain a copy of the License at
;
;      http://www.apache.org/licenses/LICENSE-2.0
;
;  Unless required by applicable law or agreed to in writing, software
;  distributed under the License is distributed on an "AS IS" BASIS,
;  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;  See the License for the specific language governing permissions and
; limitations under the License.


; Proof of concept demo to start event streams

Global $stream_dll_hand

Func StreamWinEventLogs($dll_path, $output_dir = "c:\temp\")
	 ; Start streaming logs
	 ConsoleWrite("StreamWinEventLogs" & @CRLF)

	$stream_dll_hand = DllOpen($dll_path)
	ConsoleWrite("dll handle  " & $stream_dll_hand  & @CRLF)
	if $stream_dll_hand < 0 Then
		ConsoleWrite("Error opening dll " & $stream_dll_hand & " = " & $dll_path & @CRLF)
		return $stream_dll_hand
	EndIf


	Local $ret = DllCall ( $stream_dll_hand, "int:cdecl", "StartStreamEventsThread", "WSTR", "Microsoft-Windows-Sysmon/Operational", "WSTR", $output_dir & "Microsoft-Windows-SysmonOperational.xml")
	if $ret Then
		ConsoleWrite("error sysmon " & $ret  & @CRLF)
		return $ret
	Endif

	$ret = DllCall ( $stream_dll_hand, "int:cdecl", "StartStreamEventsThread", "WSTR", "System", "WSTR",  $output_dir &  "System.xml")
	if $ret Then
		ConsoleWrite("error System " & $ret  & @CRLF)
		return $ret
	Endif

	$ret = DllCall ( $stream_dll_hand, "int:cdecl", "StartStreamEventsThread", "WSTR", "Application", "WSTR",  $output_dir &  "Application.xml")
	if $ret Then
		ConsoleWrite("error Application " & $ret  & @CRLF)
		return $ret
	Endif

	$ret = DllCall ( $stream_dll_hand, "int:cdecl", "StartStreamEventsThread", "WSTR", "Security", "WSTR",  $output_dir &  "Security.xml")
	if $ret Then
		ConsoleWrite("error Application " & $ret  & @CRLF)
		return $ret
	Endif

	$ret = DllCall ( $stream_dll_hand, "int:cdecl", "StartStreamEventsThread", "WSTR", "Windows Powershell", "WSTR",  $output_dir &  "Microsoft-Windows-PowershellOperational.xml")
	if $ret Then
		ConsoleWrite("error powershell " & $ret  & @CRLF)
		return $ret
	Endif
	ConsoleWrite("StreamWinEventLogs return " & $ret & @CRLF)

	return 0
EndFunc

Func StopStreamWinEventLogs()

	$ret = DllCall ( $stream_dll_hand, "int:cdecl", "StopEventStreamThreads")
	if $ret Then
		ConsoleWrite("error stopping" & $ret  & @CRLF)
		return $ret
	Endif

	DllClose($stream_dll_hand)
EndFunc


