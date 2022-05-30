#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.3.16.0
 Author:         karlh@

 Script Function:
	Start event streams

#ce ----------------------------------------------------------------------------

; Script Start - Add your code below here
#include <MsgBoxConstants.au3>
#include <Debug.au3>

#include "importeventlogs.au3"

;MsgBox(0, "", "@AutoItVersion: " & AutoItVersion & ", @CPUArch: " & @CPUArch)
_DebugSetup("Start", True) 
ConsoleWrite("starting" & @CRLF)
ConsoleWrite("@CPUArch: " & @CPUArch & @CRLF)

_DebugSetup("DllCall", True) 

Local $ret = StreamLogs("c:\temp\")
if $ret Then
	ConsoleWrite("Error starting" & @CRLF)
	Exit (1)
EndIf

sleep(10 * 1000)

; MsgBox($MB_OK, "Tutorial", "done") 
ConsoleWrite("exit" & @CRLF)