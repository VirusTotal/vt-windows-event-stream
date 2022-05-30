
Global $stream_dll_hand

Func StreamWinEventLogs($dll_path, $output_dir = "c:\temp\")
	 ; Start streaming logs
	 ConsoleWrite("StreamWinEventLogs" & @CRLF)
	 
	$stream_dll_hand = DllOpen($dll_path)
	ConsoleWrite("dll handle  " & $stream_dll_hand  & @CRLF)
	if $stream_dll_hand < 0 Then
		ConsoleWrite("Error opening dll " & $stream_dll_hand " = " & $dll_path & @CRLF)
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


