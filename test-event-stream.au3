#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.3.16.0
 Author:         myName

 Script Function:
	Template AutoIt script.

#ce ----------------------------------------------------------------------------

; Script Start - Add your code below here
#include <MsgBoxConstants.au3>
#include <Debug.au3>

;MsgBox(0, "", "@AutoItVersion: " & AutoItVersion & ", @CPUArch: " & @CPUArch)
_DebugSetup("Start", True) 
ConsoleWrite("starting" & @CRLF)
ConsoleWrite("@CPUArch: " & @CPUArch)

_DebugSetup("DllCall", True) 

;Local $hDLL = DllOpen("x64\Debug\event-stream.dll")
Local $hDLL = DllOpen("Debug\event-stream.dll")
ConsoleWrite("hdll " & $hDLL  & @CRLF)

if $hDLL < 0 Then
	ConsoleWrite("Error opening dll " & $hDLL  & @CRLF)
	Return
EndIf

Local $ret = DllCall ( $hDLL, "int:cdecl", "StreamEvents", "WSTR", "Microsoft-Windows-Sysmon/Operational", "WSTR", "c:\temp\sysmon.xml")
;Local $ret = DllCall ( $hDLL, "INT", "HelloWorld0")
ConsoleWrite("dll ret " & $ret  & @CRLF)
$ret = DllCall ( $hDLL, "int:cdecl", "HelloWorld0")
ConsoleWrite("dll ret " & $ret  & @CRLF)
$ret = DllCall ( $hDLL, 'int:cdecl', "HelloWorld1", "int", 0)

ConsoleWrite("dll ret " & $ret  & @CRLF)
DllClose($hDLL)

; MsgBox($MB_OK, "Tutorial", "done") 
ConsoleWrite("exit")