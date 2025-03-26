#ifndef BASTD_OS_KEY_C
#define BASTD_OS_KEY_C

// stolen from https://gist.github.com/Nordaj/cc6cadcbee1019a3299939d0f1b86296
typedef enum os_Key os_Key;
enum os_Key
{
	os_Key_LeftMouseBtn			=0x01, //Left mouse button
	os_Key_RightMouseBtn			=0x02, //Right mouse button
	os_Key_CtrlBrkPrcs				=0x03, //Control-break processing
	os_Key_MidMouseBtn				=0x04, //Middle mouse button

	os_Key_ThumbForward			=0x05, //Thumb button back on mouse aka X1
	os_Key_ThumbBack				=0x06, //Thumb button forward on mouse aka X2

	//0x07 : reserved

	os_Key_BackSpace				=0x08, //Backspace key
	os_Key_Tab						=0x09, //Tab key

	//0x0A - 0x0B : reserved

	os_Key_Clear					=0x0C, //Clear key
	os_Key_Enter					=0x0D, //Enter or Return key

	//0x0E - 0x0F : unassigned

	os_Key_Shift					=0x10, //Shift key
	os_Key_Control					=0x11, //Ctrl key
	os_Key_Alt						=0x12, //Alt key
	os_Key_Pause					=0x13, //Pause key
	os_Key_CapsLock				=0x14, //Caps lock key

	os_Key_Kana					=0x15, //Kana input mode
	os_Key_Hangeul					=0x15, //Hangeul input mode
	os_Key_Hangul					=0x15, //Hangul input mode

	//0x16 : unassigned

	os_Key_Junju					=0x17, //Junja input method
	os_Key_Final					=0x18, //Final input method
	os_Key_Hanja					=0x19, //Hanja input method
	os_Key_Kanji					=0x19, //Kanji input method

	//0x1A : unassigned

	os_Key_Escape					=0x1B, //Esc key

	os_Key_Convert					=0x1C, //IME convert
	os_Key_NonConvert				=0x1D, //IME Non convert
	os_Key_Accept					=0x1E, //IME accept
	os_Key_ModeChange				=0x1F, //IME mode change

	os_Key_Space					=0x20, //Space bar
	os_Key_PageUp					=0x21, //Page up key
	os_Key_PageDown				=0x22, //Page down key
	os_Key_End						=0x23, //End key
	os_Key_Home					=0x24, //Home key
	os_Key_LeftArrow				=0x25, //Left arrow key
	os_Key_UpArrow					=0x26, //Up arrow key
	os_Key_RightArrow				=0x27, //Right arrow key
	os_Key_DownArrow				=0x28, //Down arrow key
	os_Key_Select					=0x29, //Select key
	os_Key_Print					=0x2A, //Print key
	os_Key_Execute					=0x2B, //Execute key
	os_Key_PrintScreen				=0x2C, //Print screen key
	os_Key_Inser					=0x2D, //Insert key
	os_Key_Delete					=0x2E, //Delete key
	os_Key_Help					=0x2F, //Help key

	os_Key_Num0					=0x30, //Top row 0 key (Matches '0')
	os_Key_Num1					=0x31, //Top row 1 key (Matches '1')
	os_Key_Num2					=0x32, //Top row 2 key (Matches '2')
	os_Key_Num3					=0x33, //Top row 3 key (Matches '3')
	os_Key_Num4					=0x34, //Top row 4 key (Matches '4')
	os_Key_Num5					=0x35, //Top row 5 key (Matches '5')
	os_Key_Num6					=0x36, //Top row 6 key (Matches '6')
	os_Key_Num7					=0x37, //Top row 7 key (Matches '7')
	os_Key_Num8					=0x38, //Top row 8 key (Matches '8')
	os_Key_Num9					=0x39, //Top row 9 key (Matches '9')

	//0x3A - 0x40 : unassigned

	os_Key_A						=0x41, //A key (Matches 'A')
	os_Key_B						=0x42, //B key (Matches 'B')
	os_Key_C						=0x43, //C key (Matches 'C')
	os_Key_D						=0x44, //D key (Matches 'D')
	os_Key_E						=0x45, //E key (Matches 'E')
	os_Key_F						=0x46, //F key (Matches 'F')
	os_Key_G						=0x47, //G key (Matches 'G')
	os_Key_H						=0x48, //H key (Matches 'H')
	os_Key_I						=0x49, //I key (Matches 'I')
	os_Key_J						=0x4A, //J key (Matches 'J')
	os_Key_K						=0x4B, //K key (Matches 'K')
	os_Key_L						=0x4C, //L key (Matches 'L')
	os_Key_M						=0x4D, //M key (Matches 'M')
	os_Key_N						=0x4E, //N key (Matches 'N')
	os_Key_O						=0x4F, //O key (Matches 'O')
	os_Key_P						=0x50, //P key (Matches 'P')
	os_Key_Q						=0x51, //Q key (Matches 'Q')
	os_Key_R						=0x52, //R key (Matches 'R')
	os_Key_S						=0x53, //S key (Matches 'S')
	os_Key_T						=0x54, //T key (Matches 'T')
	os_Key_U						=0x55, //U key (Matches 'U')
	os_Key_V						=0x56, //V key (Matches 'V')
	os_Key_W						=0x57, //W key (Matches 'W')
	os_Key_X						=0x58, //X key (Matches 'X')
	os_Key_Y						=0x59, //Y key (Matches 'Y')
	os_Key_Z						=0x5A, //Z key (Matches 'Z')

	os_Key_LeftWin					=0x5B, //Left windows key
	os_Key_RightWin				=0x5C, //Right windows key
	os_Key_Apps					=0x5D, //Applications key

	//0x5E : reserved

	os_Key_Sleep					=0x5F, //Computer sleep key

	os_Key_Numpad0					=0x60, //Numpad 0
	os_Key_Numpad1					=0x61, //Numpad 1
	os_Key_Numpad2					=0x62, //Numpad 2
	os_Key_Numpad3					=0x63, //Numpad 3
	os_Key_Numpad4					=0x64, //Numpad 4
	os_Key_Numpad5					=0x65, //Numpad 5
	os_Key_Numpad6					=0x66, //Numpad 6
	os_Key_Numpad7					=0x67, //Numpad 7
	os_Key_Numpad8					=0x68, //Numpad 8
	os_Key_Numpad9					=0x69, //Numpad 9
	os_Key_Multiply				=0x6A, //Multiply key
	os_Key_Add						=0x6B, //Add key
	os_Key_Separator				=0x6C, //Separator key
	os_Key_Subtract				=0x6D, //Subtract key
	os_Key_Decimal					=0x6E, //Decimal key
	os_Key_Divide					=0x6F, //Divide key
	os_Key_F1						=0x70, //F1
	os_Key_F2						=0x71, //F2
	os_Key_F3						=0x72, //F3
	os_Key_F4						=0x73, //F4
	os_Key_F5						=0x74, //F5
	os_Key_F6						=0x75, //F6
	os_Key_F7						=0x76, //F7
	os_Key_F8						=0x77, //F8
	os_Key_F9						=0x78, //F9
	os_Key_F10						=0x79, //F10
	os_Key_F11						=0x7A, //F11
	os_Key_F12						=0x7B, //F12
	os_Key_F13						=0x7C, //F13
	os_Key_F14						=0x7D, //F14
	os_Key_F15						=0x7E, //F15
	os_Key_F16						=0x7F, //F16
	os_Key_F17						=0x80, //F17
	os_Key_F18						=0x81, //F18
	os_Key_F19						=0x82, //F19
	os_Key_F20						=0x83, //F20
	os_Key_F21						=0x84, //F21
	os_Key_F22						=0x85, //F22
	os_Key_F23						=0x86, //F23
	os_Key_F24						=0x87, //F24

	//0x88 - 0x8F : UI navigation

	os_Key_NavigationView			=0x88, //reserved
	os_Key_NavigationMenu			=0x89, //reserved
	os_Key_NavigationUp			=0x8A, //reserved
	os_Key_NavigationDown			=0x8B, //reserved
	os_Key_NavigationLeft			=0x8C, //reserved
	os_Key_NavigationRight			=0x8D, //reserved
	os_Key_NavigationAccept		=0x8E, //reserved
	os_Key_NavigationCancel		=0x8F, //reserved

	os_Key_NumLock					=0x90, //Num lock key
	os_Key_ScrollLock				=0x91, //Scroll lock key

	os_Key_NumpadEqual				=0x92, //Numpad =

	os_Key_FJ_Jisho				=0x92, //Dictionary key
	os_Key_FJ_Masshou				=0x93, //Unregister word key
	os_Key_FJ_Touroku				=0x94, //Register word key
	os_Key_FJ_Loya					=0x95, //Left OYAYUBI key
	os_Key_FJ_Roya					=0x96, //Right OYAYUBI key

	//0x97 - 0x9F : unassigned

	os_Key_LeftShift				=0xA0, //Left shift key
	os_Key_RightShift				=0xA1, //Right shift key
	os_Key_LeftCtrl				=0xA2, //Left control key
	os_Key_RightCtrl				=0xA3, //Right control key
	os_Key_LeftMenu				=0xA4, //Left menu key
	os_Key_RightMenu				=0xA5, //Right menu

	os_Key_BrowserBack				=0xA6, //Browser back button
	os_Key_BrowserForward			=0xA7, //Browser forward button
	os_Key_BrowserRefresh			=0xA8, //Browser refresh button
	os_Key_BrowserStop				=0xA9, //Browser stop button
	os_Key_BrowserSearch			=0xAA, //Browser search button
	os_Key_BrowserFavorites		=0xAB, //Browser favorites button
	os_Key_BrowserHome				=0xAC, //Browser home button

	os_Key_VolumeMute				=0xAD, //Volume mute button
	os_Key_VolumeDown				=0xAE, //Volume down button
	os_Key_VolumeUp				=0xAF, //Volume up button
	os_Key_NextTrack				=0xB0, //Next track media button
	os_Key_PrevTrack				=0xB1, //Previous track media button
	os_Key_Stop					=0xB2, //Stop media button
	os_Key_PlayPause				=0xB3, //Play/pause media button
	os_Key_Mail					=0xB4, //Launch mail button
	os_Key_MediaSelect				=0xB5, //Launch media select button
	os_Key_App1					=0xB6, //Launch app 1 button
	os_Key_App2					=0xB7, //Launch app 2 button

	//0xB8 - 0xB9 : reserved

	os_Key_OEM1					=0xBA, //;: key for US or misc keys for others
	os_Key_Plus					=0xBB, //Plus key
	os_Key_Comma					=0xBC, //Comma key
	os_Key_Minus					=0xBD, //Minus key
	os_Key_Period					=0xBE, //Period key
	os_Key_OEM2					=0xBF, //? for US or misc keys for others
	os_Key_OEM3					=0xC0, //~ for US or misc keys for others

	//0xC1 - 0xC2 : reserved

	os_Key_Gamepad_A				=0xC3, //Gamepad A button
	os_Key_Gamepad_B				=0xC4, //Gamepad B button
	os_Key_Gamepad_X				=0xC5, //Gamepad X button
	os_Key_Gamepad_Y				=0xC6, //Gamepad Y button
	os_Key_GamepadRightBumper		=0xC7, //Gamepad right bumper
	os_Key_GamepadLeftBumper		=0xC8, //Gamepad left bumper
	os_Key_GamepadLeftTrigger		=0xC9, //Gamepad left trigger
	os_Key_GamepadRightTrigger		=0xCA, //Gamepad right trigger
	os_Key_GamepadDPadUp			=0xCB, //Gamepad DPad up
	os_Key_GamepadDPadDown			=0xCC, //Gamepad DPad down
	os_Key_GamepadDPadLeft			=0xCD, //Gamepad DPad left
	os_Key_GamepadDPadRight		=0xCE, //Gamepad DPad right
	os_Key_GamepadMenu				=0xCF, //Gamepad menu button
	os_Key_GamepadView				=0xD0, //Gamepad view button
	os_Key_GamepadLeftStickBtn		=0xD1, //Gamepad left stick button
	os_Key_GamepadRightStickBtn	=0xD2, //Gamepad right stick button
	os_Key_GamepadLeftStickUp		=0xD3, //Gamepad left stick up
	os_Key_GamepadLeftStickDown	=0xD4, //Gamepad left stick down
	os_Key_GamepadLeftStickRight	=0xD5, //Gamepad left stick right
	os_Key_GamepadLeftStickLeft	=0xD6, //Gamepad left stick left
	os_Key_GamepadRightStickUp		=0xD7, //Gamepad right stick up
	os_Key_GamepadRightStickDown	=0xD8, //Gamepad right stick down
	os_Key_GamepadRightStickRight	=0xD9, //Gamepad right stick right
	os_Key_GamepadRightStickLeft	=0xDA, //Gamepad right stick left

	os_Key_OEM4					=0xDB, //[ for US or misc keys for others
	os_Key_OEM5					=0xDC, //\ for US or misc keys for others
	os_Key_OEM6					=0xDD, //] for US or misc keys for others
	os_Key_OEM7					=0xDE, //' for US or misc keys for others
	os_Key_OEM8					=0xDF, //Misc keys for others

	//0xE0 : reserved

	os_Key_OEMAX					=0xE1, //AX key on Japanese AX keyboard
	os_Key_OEM102					=0xE2, //"<>" or "\|" on RT 102-key keyboard
	os_Key_ICOHelp					=0xE3, //Help key on ICO
	os_Key_ICO00					=0xE4, //00 key on ICO

	os_Key_ProcessKey				=0xE5, //Process key input method
	os_Key_OEMCLEAR				=0xE6, //OEM specific
	os_Key_Packet					=0xE7, //IDK man try to google it

	//0xE8 : unassigned

	os_Key_OEMReset				=0xE9, //OEM reset button
	os_Key_OEMJump					=0xEA, //OEM jump button
	os_Key_OEMPA1					=0xEB, //OEM PA1 button
	os_Key_OEMPA2					=0xEC, //OEM PA2 button
	os_Key_OEMPA3					=0xED, //OEM PA3 button
	os_Key_OEMWSCtrl				=0xEE, //OEM WS Control button
	os_Key_OEMCusel				=0xEF, //OEM CUSEL button
	os_Key_OEMAttn					=0xF0, //OEM ATTN button
	os_Key_OEMFinish				=0xF1, //OEM finish button
	os_Key_OEMCopy					=0xF2, //OEM copy button
	os_Key_OEMAuto					=0xF3, //OEM auto button
	os_Key_OEMEnlw					=0xF4, //OEM ENLW
	os_Key_OEMBackTab				=0xF5, //OEM back tab

	os_Key_Attn					=0xF6, //Attn
	os_Key_CrSel					=0xF7, //CrSel
	os_Key_ExSel					=0xF8, //ExSel
	os_Key_EraseEOF				=0xF9, //Erase EOF key
	os_Key_Play					=0xFA, //Play key
	os_Key_Zoom					=0xFB, //Zoom key
	os_Key_NoName					=0xFC, //No name
	os_Key_PA1						=0xFD, //PA1 key
	os_Key_OEMClear				=0xFE, //OEM Clear key

	os_Key_len,
};

#endif//BASTD_OS_KEY_C