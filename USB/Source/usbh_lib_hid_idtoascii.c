/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usbh_lib_hid_idtoascii.c
 * Purpose: USB Host - Human Interface Device Class Key mapping table
 *----------------------------------------------------------------------------*/

#include "usbh_lib_hid_idtoascii.h"


// Values of 0xFFU will be translated to HID ID + 0x10000

const unsigned char HID_KEYBOARD_ID_TO_ASCII[] = {
  // Usage IDs:   0 ..   9
  0xFFU,                                // Reserved
  0xFFU,                                // ErrorRollOver
  0xFFU,                                // POSTFail
  0xFFU,                                // ErrorUndefined
  (unsigned char)'a',                   // Keyboard a and A
  (unsigned char)'b',                   // Keyboard b and B
  (unsigned char)'c',                   // Keyboard c and C
  (unsigned char)'d',                   // Keyboard d and D
  (unsigned char)'e',                   // Keyboard e and E
  (unsigned char)'f',                   // Keyboard f and F

  // Usage IDs:  10 ..  19
  (unsigned char)'g',                   // Keyboard g and G
  (unsigned char)'h',                   // Keyboard h and H
  (unsigned char)'i',                   // Keyboard i and I
  (unsigned char)'j',                   // Keyboard j and J
  (unsigned char)'k',                   // Keyboard k and K
  (unsigned char)'l',                   // Keyboard l and L
  (unsigned char)'m',                   // Keyboard m and M
  (unsigned char)'n',                   // Keyboard n and N
  (unsigned char)'o',                   // Keyboard o and O
  (unsigned char)'p',                   // Keyboard p and P

  // Usage IDs:  20 ..  29
  (unsigned char)'q',                   // Keyboard q and Q
  (unsigned char)'r',                   // Keyboard r and R
  (unsigned char)'s',                   // Keyboard s and S
  (unsigned char)'t',                   // Keyboard t and T
  (unsigned char)'u',                   // Keyboard u and U
  (unsigned char)'v',                   // Keyboard v and V
  (unsigned char)'w',                   // Keyboard w and W
  (unsigned char)'x',                   // Keyboard x and X
  (unsigned char)'y',                   // Keyboard y and Y
  (unsigned char)'z',                   // Keyboard z and Z

  // Usage IDs:  30 ..  39
  (unsigned char)'1',                   // Keyboard 1 and !
  (unsigned char)'2',                   // Keyboard 2 and @
  (unsigned char)'3',                   // Keyboard 3 and #
  (unsigned char)'4',                   // Keyboard 4 and $
  (unsigned char)'5',                   // Keyboard 5 and %
  (unsigned char)'6',                   // Keyboard 6 and ^
  (unsigned char)'7',                   // Keyboard 7 and &
  (unsigned char)'8',                   // Keyboard 8 and *
  (unsigned char)'9',                   // Keyboard 9 and (
  (unsigned char)'0',                   // Keyboard 0 and )

  // Usage IDs:  40 ..  49
  0x0DU,                                // Keyboard Return (ENTER)
  0x1BU,                                // Keyboard ESCAPE
  0x08U,                                // Keyboard DELETE (Backspace)
  0x09U,                                // Keyboard Tab
  (unsigned char)' ',                   // Keyboard Spacebar
  (unsigned char)'-',                   // Keyboard - and (underscore)
  (unsigned char)'=',                   // Keyboard = and +
  (unsigned char)'[',                   // Keyboard [ and {
  (unsigned char)']',                   // Keyboard ] and }
  (unsigned char)'\\',                  // Keyboard \ and |

  // Usage IDs:  50 ..  59
  (unsigned char)'#',                   // Keyboard Non-US # and ~
  (unsigned char)';',                   // Keyboard ; and :
  (unsigned char)'\'',                  // Keyboard ' and "
  (unsigned char)'`',                   // Keyboard Grave Accent and Tilde
  (unsigned char)',',                   // Keyboard , and <
  (unsigned char)'.',                   // Keyboard . and >
  (unsigned char)'/',                   // Keyboard / and ?
  0xFFU                                 // Keyboard Caps Lock
#if 0
 ,0xFFU,                                // Keyboard F1
  0xFFU,                                // Keyboard F2

  // Usage IDs:  60 ..  69
  0xFFU,                                // Keyboard F3
  0xFFU,                                // Keyboard F4
  0xFFU,                                // Keyboard F5
  0xFFU,                                // Keyboard F6
  0xFFU,                                // Keyboard F7
  0xFFU,                                // Keyboard F8
  0xFFU,                                // Keyboard F9
  0xFFU,                                // Keyboard F10
  0xFFU,                                // Keyboard F11
  0xFFU,                                // Keyboard F12

  // Usage IDs:  70 ..  79
  0xFFU,                                // Keyboard PrintScreen
  0xFFU,                                // Keyboard Scroll Lock
  0xFFU,                                // Keyboard Pause
  0xFFU,                                // Keyboard Insert
  0xFFU,                                // Keyboard Home
  0xFFU,                                // Keyboard PageUp
  0xFFU,                                // Keyboard Delete Forward
  0xFFU,                                // Keyboard End
  0xFFU,                                // Keyboard PageDown
  0xFFU,                                // Keyboard RightArrow

  // Usage IDs:  80 ..  89
  0xFFU,                                // Keyboard LeftArrow
  0xFFU,                                // Keyboard DownArrow
  0xFFU,                                // Keyboard UpArrow
  0xFFU,                                // Keypad Num Lock and Clear
  0xFFU,                                // Keypad /
  0xFFU,                                // Keypad *
  0xFFU,                                // Keypad -
  0xFFU,                                // Keypad +
  0xFFU,                                // Keypad ENTER
  0xFFU,                                // Keypad 1 and End

  // Usage IDs:  90 ..  99
  0xFFU,                                // Keypad 2 and Down Arrow
  0xFFU,                                // Keypad 3 and PageDn
  0xFFU,                                // Keypad 4 and Left Arrow
  0xFFU,                                // Keypad 5
  0xFFU,                                // Keypad 6 and Right Arrow
  0xFFU,                                // Keypad 7 and Home
  0xFFU,                                // Keypad 8 and Up Arrow
  0xFFU,                                // Keypad 9 and PageUp
  0xFFU,                                // Keypad 0 and Insert
  0xFFU,                                // Keypad . and Delete

  // Usage IDs: 100 .. 109
  0xFFU,                                // Keyboard Non-US \ and |
  0xFFU,                                // Keyboard Application
  0xFFU,                                // Keyboard Power
  0xFFU,                                // Keypad =
  0xFFU,                                // Keyboard F13
  0xFFU,                                // Keyboard F14
  0xFFU,                                // Keyboard F15
  0xFFU,                                // Keyboard F16
  0xFFU,                                // Keyboard F17
  0xFFU,                                // Keyboard F18

  // Usage IDs: 110 .. 119
  0xFFU,                                // Keyboard F19
  0xFFU,                                // Keyboard F20
  0xFFU,                                // Keyboard F21
  0xFFU,                                // Keyboard F22
  0xFFU,                                // Keyboard F23
  0xFFU,                                // Keyboard F24
  0xFFU,                                // Keyboard Execute
  0xFFU,                                // Keyboard Help
  0xFFU,                                // Keyboard Menu
  0xFFU,                                // Keyboard Select

  // Usage IDs: 120 .. 129
  0xFFU,                                // Keyboard Stop
  0xFFU,                                // Keyboard Again
  0xFFU,                                // Keyboard Undo
  0xFFU,                                // Keyboard Cut
  0xFFU,                                // Keyboard Copy
  0xFFU,                                // Keyboard Paste
  0xFFU,                                // Keyboard Find
  0xFFU,                                // Keyboard Mute
  0xFFU,                                // Keyboard Volume Up
  0xFFU,                                // Keyboard Volume Down

  // Usage IDs: 130 .. 139
  0xFFU,                                // Keyboard Locking Caps Lock
  0xFFU,                                // Keyboard Locking Num Lock
  0xFFU,                                // Keyboard Locking Scroll Lock
  0xFFU,                                // Keypad Comma
  0xFFU,                                // Keypad Equal Sign
  0xFFU,                                // Keyboard International 1
  0xFFU,                                // Keyboard International 2
  0xFFU,                                // Keyboard International 3
  0xFFU,                                // Keyboard International 4
  0xFFU,                                // Keyboard International 5

  // Usage IDs: 140 .. 149
  0xFFU,                                // Keyboard International 6
  0xFFU,                                // Keyboard International 7
  0xFFU,                                // Keyboard International 8
  0xFFU,                                // Keyboard International 9
  0xFFU,                                // Keyboard LANG1
  0xFFU,                                // Keyboard LANG2
  0xFFU,                                // Keyboard LANG3
  0xFFU,                                // Keyboard LANG4
  0xFFU,                                // Keyboard LANG5
  0xFFU,                                // Keyboard LANG6

  // Usage IDs: 150 .. 159
  0xFFU,                                // Keyboard LANG7
  0xFFU,                                // Keyboard LANG8
  0xFFU,                                // Keyboard LANG9
  0xFFU,                                // Keyboard Alternate Erase
  0xFFU,                                // Keyboard SysReq/Attention
  0xFFU,                                // Keyboard Cancel
  0xFFU,                                // Keyboard Clear
  0xFFU,                                // Keyboard Prior
  0xFFU,                                // Keyboard Return
  0xFFU,                                // Keyboard Separator

  // Usage IDs: 160 .. 169
  0xFFU,                                // Keyboard Out
  0xFFU,                                // Keyboard Oper
  0xFFU,                                // Keyboard Clear/Again
  0xFFU,                                // Keyboard CrSel/Props
  0xFFU,                                // Keyboard ExSel
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved

  // Usage IDs: 170 .. 179
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Keypad 00
  0xFFU,                                // Keypad 000
  0xFFU,                                // Thousands Separator
  0xFFU,                                // Decimal Separator

  // Usage IDs: 180 .. 189
  0xFFU,                                // Currency Unit
  0xFFU,                                // Currency Sub-unit
  0xFFU,                                // Keypad (
  0xFFU,                                // Keypad )
  0xFFU,                                // Keypad {
  0xFFU,                                // Keypad }
  0xFFU,                                // Keypad Tab
  0xFFU,                                // Keypad Backspace
  0xFFU,                                // Keypad A
  0xFFU,                                // Keypad B

  // Usage IDs: 190 .. 199
  0xFFU,                                // Keypad C
  0xFFU,                                // Keypad D
  0xFFU,                                // Keypad E
  0xFFU,                                // Keypad F
  0xFFU,                                // Keypad XOR
  0xFFU,                                // Keypad ^
  0xFFU,                                // Keypad %
  0xFFU,                                // Keypad <
  0xFFU,                                // Keypad >
  0xFFU,                                // Keypad &

  // Usage IDs: 200 .. 209
  0xFFU,                                // Keypad &&
  0xFFU,                                // Keypad |
  0xFFU,                                // Keypad ||
  0xFFU,                                // Keypad :
  0xFFU,                                // Keypad #
  0xFFU,                                // Keypad Space
  0xFFU,                                // Keypad @
  0xFFU,                                // Keypad !
  0xFFU,                                // Keypad Memory Store
  0xFFU,                                // Keypad Memory Recall

  // Usage IDs: 210 .. 219
  0xFFU,                                // Keypad Memory Clear
  0xFFU,                                // Keypad Memory Add
  0xFFU,                                // Keypad Memory Substract
  0xFFU,                                // Keypad Memory Multiply
  0xFFU,                                // Keypad Memory Divide
  0xFFU,                                // Keypad +/-
  0xFFU,                                // Keypad Clear
  0xFFU,                                // Keypad Clear Entry
  0xFFU,                                // Keypad Binary
  0xFFU,                                // Keypad Octal

  // Usage IDs: 220 .. 229
  0xFFU,                                // Keypad Decimal
  0xFFU,                                // Keypad Hexadecimal
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Keyboard LeftControl
  0xFFU,                                // Keyboard LeftShift
  0xFFU,                                // Keyboard LeftAlt
  0xFFU,                                // Keyboard Left GUI
  0xFFU,                                // Keyboard RightControl
  0xFFU,                                // Keyboard RightShift

  // Usage IDs: 230 .. 239
  0xFFU,                                // Keyboard RightAlt
  0xFFU,                                // Keyboard Right GUI
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved

  // Usage IDs: 240 .. 249
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved

  // Usage IDs: 250 .. 255
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU,                                // Reserved
  0xFFU                                 // Reserved
#endif
};
