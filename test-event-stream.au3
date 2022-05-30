#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.3.16.0
 Author:         karlh@google.com

 Script Function:
	Start event streams

#ce ----------------------------------------------------------------------------

; Script Start - Add your code below here
#include <MsgBoxConstants.au3>
#include <Debug.au3>

#include "importeventlogs.au3"

_DebugSetup("Start", True) 
ConsoleWrite("starting" & @CRLF)
ConsoleWrite("@CPUArch: " & @CPUArch & @CRLF)

_DebugSetup("DllCall", True) 

Local $ret = StreamWinEventLogs("event-stream.dll", "c:\temp\")
if $ret Then
	ConsoleWrite("Error starting" & @CRLF)
	Exit (1)
EndIf

; run test for 10 seconds and exit
sleep(10 * 1000)
StopStreamWinEventLogs()

ConsoleWrite("exit" & @CRLF)