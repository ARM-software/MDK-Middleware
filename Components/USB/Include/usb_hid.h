/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    usb_hid.h
 * Purpose: USB Human Interface Device Class Definitions
 *----------------------------------------------------------------------------*/

#ifndef __USB_HID_H__
#define __USB_HID_H__

#include <stdint.h>

#include "usb_compiler.h"


// Human Interface Device Class (HID)
// HID Subclass Codes
#define HID_SUBCLASS_NONE                       0x00U
#define HID_SUBCLASS_BOOT                       0x01U

// HID Protocol Codes
#define HID_PROTOCOL_NONE                       0x00U
#define HID_PROTOCOL_BOOT                       0x00U
#define HID_PROTOCOL_KEYBOARD                   0x01U
#define HID_PROTOCOL_REPORT                     0x01U
#define HID_PROTOCOL_MOUSE                      0x02U


// HID Descriptor Types
#define HID_HID_DESCRIPTOR_TYPE                 0x21U
#define HID_REPORT_DESCRIPTOR_TYPE              0x22U
#define HID_PHYSICAL_DESCRIPTOR_TYPE            0x23U

// HID Descriptor
typedef __PACKED_STRUCT {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t bcdHID;
  uint8_t  bCountryCode;
  uint8_t  bNumDescriptors;
  // Array of one or more descriptors
  __PACKED_STRUCT _HID_DESCRIPTOR_LIST {
    uint8_t  bDescriptorType;
    uint16_t wDescriptorLength;
  } DescriptorList[1];
} HID_DESCRIPTOR;

// HID Request Codes
#define HID_REQUEST_GET_REPORT                  0x01U
#define HID_REQUEST_GET_IDLE                    0x02U
#define HID_REQUEST_GET_PROTOCOL                0x03U
#define HID_REQUEST_SET_REPORT                  0x09U
#define HID_REQUEST_SET_IDLE                    0x0AU
#define HID_REQUEST_SET_PROTOCOL                0x0BU

// HID Report Types
#define HID_REPORT_INPUT                        0x01U
#define HID_REPORT_OUTPUT                       0x02U
#define HID_REPORT_FEATURE                      0x03U


// Usage Pages
#define HID_USAGE_PAGE_UNDEFINED                0x00U
#define HID_USAGE_PAGE_GENERIC                  0x01U
#define HID_USAGE_PAGE_SIMULATION               0x02U
#define HID_USAGE_PAGE_VR                       0x03U
#define HID_USAGE_PAGE_SPORT                    0x04U
#define HID_USAGE_PAGE_GAME                     0x05U
#define HID_USAGE_PAGE_DEV_CONTROLS             0x06U
#define HID_USAGE_PAGE_KEYBOARD                 0x07U
#define HID_USAGE_PAGE_LED                      0x08U
#define HID_USAGE_PAGE_BUTTON                   0x09U
#define HID_USAGE_PAGE_ORDINAL                  0x0AU
#define HID_USAGE_PAGE_TELEPHONY                0x0BU
#define HID_USAGE_PAGE_CONSUMER                 0x0CU
#define HID_USAGE_PAGE_DIGITIZER                0x0DU
#define HID_USAGE_PAGE_UNICODE                  0x10U
#define HID_USAGE_PAGE_ALPHANUMERIC             0x14U
// ...


// Generic Desktop Page (0x01)
#define HID_USAGE_GENERIC_POINTER               0x01U
#define HID_USAGE_GENERIC_MOUSE                 0x02U
#define HID_USAGE_GENERIC_JOYSTICK              0x04U
#define HID_USAGE_GENERIC_GAMEPAD               0x05U
#define HID_USAGE_GENERIC_KEYBOARD              0x06U
#define HID_USAGE_GENERIC_KEYPAD                0x07U
#define HID_USAGE_GENERIC_X                     0x30U
#define HID_USAGE_GENERIC_Y                     0x31U
#define HID_USAGE_GENERIC_Z                     0x32U
#define HID_USAGE_GENERIC_RX                    0x33U
#define HID_USAGE_GENERIC_RY                    0x34U
#define HID_USAGE_GENERIC_RZ                    0x35U
#define HID_USAGE_GENERIC_SLIDER                0x36U
#define HID_USAGE_GENERIC_DIAL                  0x37U
#define HID_USAGE_GENERIC_WHEEL                 0x38U
#define HID_USAGE_GENERIC_HATSWITCH             0x39U
#define HID_USAGE_GENERIC_COUNTED_BUFFER        0x3AU
#define HID_USAGE_GENERIC_BYTE_COUNT            0x3BU
#define HID_USAGE_GENERIC_MOTION_WAKEUP         0x3CU
#define HID_USAGE_GENERIC_VX                    0x40U
#define HID_USAGE_GENERIC_VY                    0x41U
#define HID_USAGE_GENERIC_VZ                    0x42U
#define HID_USAGE_GENERIC_VBRX                  0x43U
#define HID_USAGE_GENERIC_VBRY                  0x44U
#define HID_USAGE_GENERIC_VBRZ                  0x45U
#define HID_USAGE_GENERIC_VNO                   0x46U
#define HID_USAGE_GENERIC_SYSTEM_CTL            0x80U
#define HID_USAGE_GENERIC_SYSCTL_POWER          0x81U
#define HID_USAGE_GENERIC_SYSCTL_SLEEP          0x82U
#define HID_USAGE_GENERIC_SYSCTL_WAKE           0x83U
#define HID_USAGE_GENERIC_SYSCTL_CONTEXT_MENU   0x84U
#define HID_USAGE_GENERIC_SYSCTL_MAIN_MENU      0x85U
#define HID_USAGE_GENERIC_SYSCTL_APP_MENU       0x86U
#define HID_USAGE_GENERIC_SYSCTL_HELP_MENU      0x87U
#define HID_USAGE_GENERIC_SYSCTL_MENU_EXIT      0x88U
#define HID_USAGE_GENERIC_SYSCTL_MENU_SELECT    0x89U
#define HID_USAGE_GENERIC_SYSCTL_MENU_RIGHT     0x8AU
#define HID_USAGE_GENERIC_SYSCTL_MENU_LEFT      0x8BU
#define HID_USAGE_GENERIC_SYSCTL_MENU_UP        0x8CU
#define HID_USAGE_GENERIC_SYSCTL_MENU_DOWN      0x8DU
// ...

// Simulation Controls Page (0x02)
// ...
#define HID_USAGE_SIMULATION_RUDDER             0xBAU
#define HID_USAGE_SIMULATION_THROTTLE           0xBBU
// ...

// Virtual Reality Controls Page (0x03)
// ...

// Sport Controls Page (0x04)
// ...

// Game Controls Page (0x05)
// ...

// Generic Device Controls Page (0x06)
// ...

// Keyboard/Keypad Page (0x07)

// Keyboard Usage Keys
extern const unsigned char HID_KEYBOARD_ID_TO_ASCII[];

// Error "Keys"
#define HID_USAGE_KEYBOARD_NOEVENT              0x00U
#define HID_USAGE_KEYBOARD_ROLLOVER             0x01U
#define HID_USAGE_KEYBOARD_POSTFAIL             0x02U
#define HID_USAGE_KEYBOARD_UNDEFINED            0x03U

// Letters
#define HID_USAGE_KEYBOARD_aA                   0x04U
#define HID_USAGE_KEYBOARD_zZ                   0x1DU

// Numbers
#define HID_USAGE_KEYBOARD_ONE                  0x1EU
#define HID_USAGE_KEYBOARD_ZERO                 0x27U

#define HID_USAGE_KEYBOARD_RETURN               0x28U
#define HID_USAGE_KEYBOARD_ESCAPE               0x29U
#define HID_USAGE_KEYBOARD_DELETE               0x2AU

// Funtion Keys
#define HID_USAGE_KEYBOARD_F1                   0x3AU
#define HID_USAGE_KEYBOARD_F12                  0x45U

#define HID_USAGE_KEYBOARD_PRINT_SCREEN         0x46U

// Modifier Keys
#define HID_USAGE_KEYBOARD_LCTRL                0xE0U
#define HID_USAGE_KEYBOARD_LSHFT                0xE1U
#define HID_USAGE_KEYBOARD_LALT                 0xE2U
#define HID_USAGE_KEYBOARD_LGUI                 0xE3U
#define HID_USAGE_KEYBOARD_RCTRL                0xE4U
#define HID_USAGE_KEYBOARD_RSHFT                0xE5U
#define HID_USAGE_KEYBOARD_RALT                 0xE6U
#define HID_USAGE_KEYBOARD_RGUI                 0xE7U
#define HID_USAGE_KEYBOARD_SCROLL_LOCK          0x47U
#define HID_USAGE_KEYBOARD_NUM_LOCK             0x53U
#define HID_USAGE_KEYBOARD_CAPS_LOCK            0x39U

// Modifier Keys (values)
#define HID_USAGE_KEYBOARD_MOD_LCTRL            0x01U
#define HID_USAGE_KEYBOARD_MOD_LSHIFT           0x02U
#define HID_USAGE_KEYBOARD_MOD_LALTL            0x04U
#define HID_USAGE_KEYBOARD_MOD_LGUI             0x08U
#define HID_USAGE_KEYBOARD_MOD_RCTRL            0x10U
#define HID_USAGE_KEYBOARD_MOD_RSHIFT           0x20U
#define HID_USAGE_KEYBOARD_MOD_RALTL            0x40U
#define HID_USAGE_KEYBOARD_MOD_RGUI             0x80U

// ...

// LED Page (0x08)
#define HID_USAGE_LED_NUM_LOCK                  0x01U
#define HID_USAGE_LED_CAPS_LOCK                 0x02U
#define HID_USAGE_LED_SCROLL_LOCK               0x03U
#define HID_USAGE_LED_COMPOSE                   0x04U
#define HID_USAGE_LED_KANA                      0x05U
#define HID_USAGE_LED_POWER                     0x06U
#define HID_USAGE_LED_SHIFT                     0x07U
#define HID_USAGE_LED_DO_NOT_DISTURB            0x08U
#define HID_USAGE_LED_MUTE                      0x09U
#define HID_USAGE_LED_TONE_ENABLE               0x0AU
#define HID_USAGE_LED_HIGH_CUT_FILTER           0x0BU
#define HID_USAGE_LED_LOW_CUT_FILTER            0x0CU
#define HID_USAGE_LED_EQUALIZER_ENABLE          0x0DU
#define HID_USAGE_LED_SOUND_FIELD_ON            0x0EU
#define HID_USAGE_LED_SURROUND_FIELD_ON         0x0FU
#define HID_USAGE_LED_REPEAT                    0x10U
#define HID_USAGE_LED_STEREO                    0x11U
#define HID_USAGE_LED_SAMPLING_RATE_DETECT      0x12U
#define HID_USAGE_LED_SPINNING                  0x13U
#define HID_USAGE_LED_CAV                       0x14U
#define HID_USAGE_LED_CLV                       0x15U
#define HID_USAGE_LED_RECORDING_FORMAT_DET      0x16U
#define HID_USAGE_LED_OFF_HOOK                  0x17U
#define HID_USAGE_LED_RING                      0x18U
#define HID_USAGE_LED_MESSAGE_WAITING           0x19U
#define HID_USAGE_LED_DATA_MODE                 0x1AU
#define HID_USAGE_LED_BATTERY_OPERATION         0x1BU
#define HID_USAGE_LED_BATTERY_OK                0x1CU
#define HID_USAGE_LED_BATTERY_LOW               0x1DU
#define HID_USAGE_LED_SPEAKER                   0x1EU
#define HID_USAGE_LED_HEAD_SET                  0x1FU
#define HID_USAGE_LED_HOLD                      0x20U
#define HID_USAGE_LED_MICROPHONE                0x21U
#define HID_USAGE_LED_COVERAGE                  0x22U
#define HID_USAGE_LED_NIGHT_MODE                0x23U
#define HID_USAGE_LED_SEND_CALLS                0x24U
#define HID_USAGE_LED_CALL_PICKUP               0x25U
#define HID_USAGE_LED_CONFERENCE                0x26U
#define HID_USAGE_LED_STAND_BY                  0x27U
#define HID_USAGE_LED_CAMERA_ON                 0x28U
#define HID_USAGE_LED_CAMERA_OFF                0x29U
#define HID_USAGE_LED_ON_LINE                   0x2AU
#define HID_USAGE_LED_OFF_LINE                  0x2BU
#define HID_USAGE_LED_BUSY                      0x2CU
#define HID_USAGE_LED_READY                     0x2DU
#define HID_USAGE_LED_PAPER_OUT                 0x2EU
#define HID_USAGE_LED_PAPER_JAM                 0x2FU
#define HID_USAGE_LED_REMOTE                    0x30U
#define HID_USAGE_LED_FORWARD                   0x31U
#define HID_USAGE_LED_REVERSE                   0x32U
#define HID_USAGE_LED_STOP                      0x33U
#define HID_USAGE_LED_REWIND                    0x34U
#define HID_USAGE_LED_FAST_FORWARD              0x35U
#define HID_USAGE_LED_PLAY                      0x36U
#define HID_USAGE_LED_PAUSE                     0x37U
#define HID_USAGE_LED_RECORD                    0x38U
#define HID_USAGE_LED_ERROR                     0x39U
#define HID_USAGE_LED_SELECTED_INDICATOR        0x3AU
#define HID_USAGE_LED_IN_USE_INDICATOR          0x3BU
#define HID_USAGE_LED_MULTI_MODE_INDICATOR      0x3CU
#define HID_USAGE_LED_INDICATOR_ON              0x3DU
#define HID_USAGE_LED_INDICATOR_FLASH           0x3EU
#define HID_USAGE_LED_INDICATOR_SLOW_BLINK      0x3FU
#define HID_USAGE_LED_INDICATOR_FAST_BLINK      0x40U
#define HID_USAGE_LED_INDICATOR_OFF             0x41U
#define HID_USAGE_LED_FLASH_ON_TIME             0x42U
#define HID_USAGE_LED_SLOW_BLINK_ON_TIME        0x43U
#define HID_USAGE_LED_SLOW_BLINK_OFF_TIME       0x44U
#define HID_USAGE_LED_FAST_BLINK_ON_TIME        0x45U
#define HID_USAGE_LED_FAST_BLINK_OFF_TIME       0x46U
#define HID_USAGE_LED_INDICATOR_COLOR           0x47U
#define HID_USAGE_LED_RED                       0x48U
#define HID_USAGE_LED_GREEN                     0x49U
#define HID_USAGE_LED_AMBER                     0x4AU
#define HID_USAGE_LED_GENERIC_INDICATOR         0x4BU

// Button Page (0x09)
//   There is no need to label these usages.

// Ordinal Page (0x0A)
//   There is no need to label these usages.

// Telephony Device Page (0x0B)
#define HID_USAGE_TELEPHONY_PHONE               0x01U
#define HID_USAGE_TELEPHONY_ANSWERING_MACHINE   0x02U
#define HID_USAGE_TELEPHONY_MESSAGE_CONTROLS    0x03U
#define HID_USAGE_TELEPHONY_HANDSET             0x04U
#define HID_USAGE_TELEPHONY_HEADSET             0x05U
#define HID_USAGE_TELEPHONY_KEYPAD              0x06U
#define HID_USAGE_TELEPHONY_PROGRAMMABLE_BUTTON 0x07U
// ...

// Consumer Page (0x0C)
#define HID_USAGE_CONSUMER_CONTROL              0x01U
// ...

// and others ...


// HID Report Item Macros

// Main Items
#define HID_Input(x)                            0x81U,x
#define HID_Output(x)                           0x91U,x
#define HID_Feature(x)                          0xB1U,x
#define HID_Collection(x)                       0xA1U,x
#define HID_EndCollection                       0xC0U

// Data (Input, Output, Feature)
#define HID_Data                                0U
#define HID_Constant                           (1U)
#define HID_Array                               0U
#define HID_Variable                           (1U << 1)
#define HID_Absolute                            0U
#define HID_Relative                           (1U << 2)
#define HID_NoWrap                              0U
#define HID_Wrap                               (1U << 3)
#define HID_Linear                              0U
#define HID_NonLinear                          (1U << 4)
#define HID_PreferredState                      0U
#define HID_NoPreferred                        (1U << 5)
#define HID_NoNullPosition                      0U
#define HID_NullState                          (1U << 6)
#define HID_NonVolatile                         0U
#define HID_Volatile                           (1U << 7)

// Collection Data
#define HID_Physical                            0x00U
#define HID_Application                         0x01U
#define HID_Logical                             0x02U
#define HID_Report                              0x03U
#define HID_NamedArray                          0x04U
#define HID_UsageSwitch                         0x05U
#define HID_UsageModifier                       0x06U

// Global Items
#define HID_UsagePage(x)                        0x05U,x
#define HID_UsagePageVendor(x)                  0x06U,x,0xFFU
#define HID_LogicalMin(x)                       0x15U,x
#define HID_LogicalMinS(x)                      0x16U,(x&0xFFU),((x>>8)&0xFFU)
#define HID_LogicalMinL(x)                      0x17U,(x&0xFFU),((x>>8)&0xFFU),((x>>16)&0xFFU),((x>>24)&0xFFU)
#define HID_LogicalMax(x)                       0x25U,x
#define HID_LogicalMaxS(x)                      0x26U,(x&0xFFU),((x>>8)&0xFFU)
#define HID_LogicalMaxL(x)                      0x27U,(x&0xFFU),((x>>8)&0xFFU),((x>>16)&0xFFU),((x>>24)&0xFFU)
#define HID_PhysicalMin(x)                      0x35U,x
#define HID_PhysicalMinS(x)                     0x36U,(x&0xFFU),((x>>8)&0xFFU)
#define HID_PhysicalMinL(x)                     0x37U,(x&0xFFU),((x>>8)&0xFFU),((x>>16)&0xFFU),((x>>24)&0xFFU)
#define HID_PhysicalMax(x)                      0x45U,x
#define HID_PhysicalMaxS(x)                     0x46U,(x&0xFFU),((x>>8)&0xFFU)
#define HID_PhysicalMaxL(x)                     0x47U,(x&0xFFU),((x>>8)&0xFFU),((x>>16)&0xFFU),((x>>24)&0xFFU)
#define HID_UnitExponent(x)                     0x55U,x
#define HID_Unit(x)                             0x65U,x
#define HID_UnitS(x)                            0x66U,(x&0xFFU),((x>>8)&0xFFU)
#define HID_UnitL(x)                            0x67U,(x&0xFFU),((x>>8)&0xFFU),((x>>16)&0xFFU),((x>>24)&0xFFU)
#define HID_ReportSize(x)                       0x75U,x
#define HID_ReportSizeS(x)                      0x76U,(x&0xFFU),((x>>8)&0xFFU)
#define HID_ReportSizeL(x)                      0x77U,(x&0xFFU),((x>>8)&0xFFU),((x>>16)&0xFFU),((x>>24)&0xFFU)
#define HID_ReportID(x)                         0x85U,x
#define HID_ReportCount(x)                      0x95U,x
#define HID_ReportCountS(x)                     0x96U,(x&0xFFU),((x>>8)&0xFFU)
#define HID_ReportCountL(x)                     0x97U,(x&0xFFU),((x>>8)&0xFFU),((x>>16)&0xFFU),((x>>24)&0xFFU)
#define HID_Push                                0xA4U
#define HID_Pop                                 0xB4U

// Local Items
#define HID_Usage(x)                            0x09U,x
#define HID_UsageMin(x)                         0x19U,x
#define HID_UsageMax(x)                         0x29U,x

#endif
