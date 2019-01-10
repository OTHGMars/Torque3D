//-----------------------------------------------------------------------------
// Copyright (c) 2012 GarageGames, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------
/// @file
/// Library-wide input events
///
/// All external events are converted into system events, which are defined
/// in this file.

///
#ifndef _EVENT_H_
#define _EVENT_H_

#include "platform/types.h"
#include "platform/input/IInputDevice.h"
#include "core/util/journal/journaledSignal.h"
#include "core/util/tSingleton.h"
#include "core/util/tDictionary.h"
#include "core/tSimpleHashTable.h"

#define AddInputVirtualMap( description, type, code )         \
   INPUTMGR->addVirtualMap( #description, type, code );

/// @defgroup input_constants Input system constants
/// @{

/// Wildcard match used by the input system.
#define SI_ANY 0x01ff

/// Input event constants:
typedef U32 InputObjectInstances;
enum InputObjectInstancesEnum
{
   KEY_NULL          = 0x000,     ///< Invalid KeyCode

   KEY_A             = 0x004,
   KEY_B             = 0x005,
   KEY_C             = 0x006,
   KEY_D             = 0x007,
   KEY_E             = 0x008,
   KEY_F             = 0x009,
   KEY_G             = 0x00A,
   KEY_H             = 0x00B,
   KEY_I             = 0x00C,
   KEY_J             = 0x00D,
   KEY_K             = 0x00E,
   KEY_L             = 0x00F,
   KEY_M             = 0x010,
   KEY_N             = 0x011,
   KEY_O             = 0x012,
   KEY_P             = 0x013,
   KEY_Q             = 0x014,
   KEY_R             = 0x015,
   KEY_S             = 0x016,
   KEY_T             = 0x017,
   KEY_U             = 0x018,
   KEY_V             = 0x019,
   KEY_W             = 0x01A,
   KEY_X             = 0x01B,
   KEY_Y             = 0x01C,
   KEY_Z             = 0x01D,

   KEY_1             = 0x01E,
   KEY_2             = 0x01F,
   KEY_3             = 0x020,
   KEY_4             = 0x021,
   KEY_5             = 0x022,
   KEY_6             = 0x023,
   KEY_7             = 0x024,
   KEY_8             = 0x025,
   KEY_9             = 0x026,
   KEY_0             = 0x027,

   KEY_RETURN        = 0x028,
   KEY_ESCAPE        = 0x029,
   KEY_BACKSPACE     = 0x02A,
   KEY_TAB           = 0x02B,
   KEY_SPACE         = 0x02C,
   KEY_MINUS         = 0x02D,
   KEY_EQUALS        = 0x02E,
   KEY_LBRACKET      = 0x02F,
   KEY_RBRACKET      = 0x030,
   KEY_BACKSLASH     = 0x031,
   KEY_SEMICOLON     = 0x033,
   KEY_APOSTROPHE    = 0x034,
   KEY_TILDE         = 0x035,
   KEY_COMMA         = 0x036,
   KEY_PERIOD        = 0x037,
   KEY_SLASH         = 0x038,
   KEY_CAPSLOCK      = 0x039,

   KEY_F1            = 0x03A,
   KEY_F2            = 0x03B,
   KEY_F3            = 0x03C,
   KEY_F4            = 0x03D,
   KEY_F5            = 0x03E,
   KEY_F6            = 0x03F,
   KEY_F7            = 0x040,
   KEY_F8            = 0x041,
   KEY_F9            = 0x042,
   KEY_F10           = 0x043,
   KEY_F11           = 0x044,
   KEY_F12           = 0x045,

   KEY_PRINT         = 0x046,
   KEY_SCROLLLOCK    = 0x047,
   KEY_PAUSE         = 0x048,
   KEY_INSERT        = 0x049,
   KEY_HOME          = 0x04A,
   KEY_PAGE_UP       = 0x04B,
   KEY_DELETE        = 0x04C,
   KEY_END           = 0x04D,
   KEY_PAGE_DOWN     = 0x04E,
   KEY_RIGHT         = 0x04F,
   KEY_LEFT          = 0x050,
   KEY_DOWN          = 0x051,
   KEY_UP            = 0x052,
   KEY_NUMLOCK       = 0x053,
   KEY_DIVIDE        = 0x054,
   KEY_MULTIPLY      = 0x055,
   KEY_SUBTRACT      = 0x056,
   KEY_ADD           = 0x057,
   KEY_NUMPADENTER   = 0x058,

   KEY_NUMPAD1       = 0x059,
   KEY_NUMPAD2       = 0x05A,
   KEY_NUMPAD3       = 0x05B,
   KEY_NUMPAD4       = 0x05C,
   KEY_NUMPAD5       = 0x05D,
   KEY_NUMPAD6       = 0x05E,
   KEY_NUMPAD7       = 0x05F,
   KEY_NUMPAD8       = 0x060,
   KEY_NUMPAD9       = 0x061,
   KEY_NUMPAD0       = 0x062,
   KEY_DECIMAL       = 0x063,
   KEY_OEM_102       = 0x064,
   KEY_WIN_APPS      = 0x065,
   KEY_NUMPADEQUALS  = 0x067,

   KEY_F13           = 0x068,
   KEY_F14           = 0x069,
   KEY_F15           = 0x06A,
   KEY_F16           = 0x06B,
   KEY_F17           = 0x06C,
   KEY_F18           = 0x06D,
   KEY_F19           = 0x06E,
   KEY_F20           = 0x06F,
   KEY_F21           = 0x070,
   KEY_F22           = 0x071,
   KEY_F23           = 0x072,
   KEY_F24           = 0x073,

   KEY_HELP          = 0x075,

   KEY_NUMPADCLEAR   = 0x09C,
   KEY_SEPARATOR     = 0x0C9,
   KEY_LCONTROL      = 0x0E0,
   KEY_RCONTROL      = 0x0E4,
   KEY_LALT          = 0x0E2,
   KEY_RALT          = 0x0E6,
   KEY_LSHIFT        = 0x0E1,
   KEY_RSHIFT        = 0x0E5,
   KEY_MAC_LOPT      = 0x0E3,
   KEY_MAC_ROPT      = 0x0E7,

   KEY_ANYKEY        = SI_ANY,

   /// Mouse/Joystick button event codes.
   KEY_BUTTON0       = 0x0200,
   KEY_BUTTON1       = 0x0201,
   KEY_BUTTON2       = 0x0202,
   KEY_BUTTON3       = 0x0203,
   KEY_BUTTON4       = 0x0204,
   KEY_BUTTON5       = 0x0205,
   KEY_BUTTON6       = 0x0206,
   KEY_BUTTON7       = 0x0207,
   KEY_BUTTON8       = 0x0208,
   KEY_BUTTON9       = 0x0209,
   KEY_BUTTON10      = 0x020A,
   KEY_BUTTON11      = 0x020B,
   KEY_BUTTON12      = 0x020C,
   KEY_BUTTON13      = 0x020D,
   KEY_BUTTON14      = 0x020E,
   KEY_BUTTON15      = 0x020F,
   KEY_BUTTON16      = 0x0210,
   KEY_BUTTON17      = 0x0211,
   KEY_BUTTON18      = 0x0212,
   KEY_BUTTON19      = 0x0213,
   KEY_BUTTON20      = 0x0214,
   KEY_BUTTON21      = 0x0215,
   KEY_BUTTON22      = 0x0216,
   KEY_BUTTON23      = 0x0217,
   KEY_BUTTON24      = 0x0218,
   KEY_BUTTON25      = 0x0219,
   KEY_BUTTON26      = 0x021A,
   KEY_BUTTON27      = 0x021B,
   KEY_BUTTON28      = 0x021C,
   KEY_BUTTON29      = 0x021D,
   KEY_BUTTON30      = 0x021E,
   KEY_BUTTON31      = 0x021F,
   KEY_BUTTON32      = 0x0220,
   KEY_BUTTON33      = 0x0221,
   KEY_BUTTON34      = 0x0222,
   KEY_BUTTON35      = 0x0223,
   KEY_BUTTON36      = 0x0224,
   KEY_BUTTON37      = 0x0225,
   KEY_BUTTON38      = 0x0226,
   KEY_BUTTON39      = 0x0227,
   KEY_BUTTON40      = 0x0228,
   KEY_BUTTON41      = 0x0229,
   KEY_BUTTON42      = 0x022A,
   KEY_BUTTON43      = 0x022B,
   KEY_BUTTON44      = 0x022C,
   KEY_BUTTON45      = 0x022D,
   KEY_BUTTON46      = 0x022E,
   KEY_BUTTON47      = 0x022F,

   /// Axis event codes
   SI_XAXIS          = 0x0300,
   SI_YAXIS          = 0x0301,
   SI_ZAXIS          = 0x0302,
   SI_RXAXIS         = 0x0303,
   SI_RYAXIS         = 0x0304,
   SI_RZAXIS         = 0x0305,
   SI_SLIDER         = 0x0306,

   /// DPad/Hat event codes.
   SI_UPOV           = 0x0310,
   SI_DPOV           = 0x0311,
   SI_LPOV           = 0x0312,
   SI_RPOV           = 0x0313,
   SI_UPOV2          = 0x0314,
   SI_DPOV2          = 0x0315,
   SI_LPOV2          = 0x0316,
   SI_RPOV2          = 0x0317,
   SI_POVMASK        = 0x0318,
   SI_POVMASK2       = 0x0319,

   /// Trackball event codes.
   SI_XBALL          = 0x0320,
   SI_YBALL          = 0x0321,
   SI_XBALL2         = 0x0322,
   SI_YBALL2         = 0x0323,

   /// Gamepad button event codes.
   XI_A              = 0x0330,
   XI_B              = 0x0331,
   XI_X              = 0x0332,
   XI_Y              = 0x0333,

   XI_BACK           = 0x0334,
   XI_GUIDE          = 0x0335,
   XI_START          = 0x0336,
   XI_LEFT_THUMB     = 0x0337,
   XI_RIGHT_THUMB    = 0x0338,
   XI_LEFT_SHOULDER  = 0x0339,
   XI_RIGHT_SHOULDER = 0x033A,

   INPUT_DEVICE_PLUGIN_CODES_START = 0x400,
};

#define XI_THUMBLX SI_XAXIS
#define XI_THUMBLY SI_YAXIS
#define XI_THUMBRX SI_RXAXIS
#define XI_THUMBRY SI_RYAXIS
#define XI_LEFT_TRIGGER SI_ZAXIS
#define XI_RIGHT_TRIGGER SI_RZAXIS

/// Input device types
typedef U32 InputDeviceTypes;
enum InputDeviceTypesEnum
{
   UnknownDeviceType,
   MouseDeviceType,
   KeyboardDeviceType,
   JoystickDeviceType,
   GamepadDeviceType,

   NUM_INPUT_DEVICE_TYPES,

   INPUT_DEVICE_PLUGIN_DEVICES_START = NUM_INPUT_DEVICE_TYPES,
};

/// Device Event Action Types
enum InputActionType
{
   /// Button was depressed.
   SI_MAKE    = 0x01,

   /// Button was released.
   SI_BREAK   = 0x02,

   /// An axis moved.
   SI_MOVE    = 0x03,

   /// A key repeat occurred. Happens in between a SI_MAKE and SI_BREAK.
   SI_REPEAT  = 0x04,

   /// A value of some type.  Matched with SI_FLOAT or SI_INT.
   SI_VALUE   = 0x05,
};

///Device Event Types
enum InputEventType
{
   SI_UNKNOWN = 0x01,
   SI_BUTTON  = 0x02,   // Button press/release
   SI_POV     = 0x03,   // Point of View hat
   SI_AXIS    = 0x04,   // Axis in range -1.0..1.0
   SI_POS     = 0x05,   // Absolute position value (Point3F)
   SI_ROT     = 0x06,   // Absolute rotation value (QuatF)
   SI_INT     = 0x07,   // Integer value (S32)
   SI_FLOAT   = 0x08,   // Float value (F32)
   SI_KEY     = 0x0A,   // Keyboard key
};

// Modifier Keys
enum InputModifiers
{
   /// shift and ctrl are the same between platforms.
   SI_LSHIFT = 0x0001,
   SI_RSHIFT = 0x0002,
   SI_SHIFT  = (SI_LSHIFT|SI_RSHIFT),
   SI_LCTRL  = 0x0040,
   SI_RCTRL  = 0x0080,
   SI_CTRL   = (SI_LCTRL|SI_RCTRL),

   /// win altkey, mapped to mac cmdkey.
   SI_LALT = 0x0100,
   SI_RALT = 0x0200,
   SI_ALT = (SI_LALT|SI_RALT),

   /// mac optionkey
   SI_MAC_LOPT  = 0x0400,
   SI_MAC_ROPT  = 0x0800,
   SI_MAC_OPT   = (SI_MAC_LOPT|SI_MAC_ROPT),

   /// modifier keys used for common operations
#if defined(TORQUE_OS_MAC)
   SI_COPYPASTE = SI_ALT,
   SI_MULTISELECT = SI_ALT,
   SI_RANGESELECT = SI_SHIFT,
   SI_PRIMARY_ALT = SI_MAC_OPT,  ///< Primary key used for toggling into alternates of commands.
   SI_PRIMARY_CTRL = SI_ALT,     ///< Primary key used for triggering commands.
#else
   SI_COPYPASTE = SI_CTRL,
   SI_MULTISELECT = SI_CTRL,
   SI_RANGESELECT = SI_SHIFT,
   SI_PRIMARY_ALT = SI_ALT,
   SI_PRIMARY_CTRL = SI_CTRL,
#endif
   /// modfier key used in conjunction w/ arrow keys to move cursor to next word
#if defined(TORQUE_OS_MAC)
   SI_WORDJUMP = SI_MAC_OPT,
#else
   SI_WORDJUMP = SI_CTRL,
#endif
   /// modifier key used in conjunction w/ arrow keys to move cursor to beginning / end of line
   SI_LINEJUMP = SI_ALT,

   /// modifier key used in conjunction w/ home & end to jump to the top or bottom of a document
#if defined(TORQUE_OS_MAC)
   SI_DOCJUMP = SI_ANY,
#else
   SI_DOCJUMP = SI_CTRL,
#endif
};

/// @}


/// Generic input event.
struct InputEventInfo
{
   InputEventInfo()
   {
      deviceInst = 0;
      fValue     = 0.f;
      fValue2    = 0.f;
      fValue3    = 0.f;
      fValue4    = 0.f;
      iValue     = 0;
      deviceType = (InputDeviceTypes)0;
      objType    = (InputEventType)0;
      ascii      = 0;
      objInst    = (InputObjectInstances)0;
      action     = (InputActionType)0;
      modifier   = (InputModifiers)0;
   }

   /// Device instance: joystick0, joystick1, etc
   U32 deviceInst;

   /// Value typically ranges from -1.0 to 1.0, but doesn't have to.
   /// It depends on the context.
   F32 fValue;

   /// Extended float values (often used for absolute rotation Quat)
   F32 fValue2;
   F32 fValue3;
   F32 fValue4;

   /// Signed integer value
   S32 iValue;

   /// What was the action? (MAKE/BREAK/MOVE)
   InputActionType      action;
   InputDeviceTypes     deviceType;
   InputEventType       objType;
   InputObjectInstances objInst;

   /// ASCII character code if this is a keyboard event.
   U16 ascii;
   
   /// Modifiers to action: SI_LSHIFT, SI_LCTRL, etc.
   U32 modifier;

   inline void postToSignal(InputEvent &ie)
   {
      ie.trigger(deviceInst, fValue, fValue2, fValue3, fValue4, iValue, deviceType, objType, ascii, objInst, action, modifier);
   }
};

class Point3F;
class QuatF;

/// Handles input device plug-ins
class InputEventManager
{
public:
   struct VirtualMapData
   {
      StringTableEntry     desc;
      InputEventType       type;
      InputObjectInstances code;
   };

public:
   InputEventManager();
   virtual ~InputEventManager();

   /// Get the next device type code
   U32 getNextDeviceType();

   /// Get the next device action code
   U32 getNextDeviceCode();

   void registerDevice(IInputDevice* device);
   void unregisterDevice(IInputDevice* device);

   /// Check if the given device name is a registered device.
   /// The given name can optionally include an instance number on the end.
   bool isRegisteredDevice(const char* name);

   /// Check if the given device type is a registered device.
   bool isRegisteredDevice(U32 type);

   /// Same as above but also provides the found device type and actual
   // device name length.  Used by ActionMap::getDeviceTypeAndInstance()
   bool isRegisteredDeviceWithAttributes(const char* name, U32& deviceType, U32&nameLen);

   /// Returns the name of a registered device given its type
   const char* getRegisteredDeviceName(U32 type);

   void start();
   void stop();

   void process();

   // Add to the virtual map table
   void addVirtualMap(const char* description, InputEventType type, InputObjectInstances code);

   // Find a virtual map entry based on the text description
   VirtualMapData* findVirtualMap(const char* description);

   // Find a keyboard map entry based on the text description
   VirtualMapData* findKeyboardMap(const char* description);

   // Find a virtual map entry's description based on the action code
   const char* findVirtualMapDescFromCode(U32 code);

   // Find a keyboard map entry's description based on the scan code
   const char* findKeyboardMapDescFromCode(U32 code);

   /// Build an input event based on a single iValue
   void buildInputEvent(U32 deviceType, U32 deviceInst, InputEventType objType, InputObjectInstances objInst, InputActionType action, S32 iValue);

   /// Build an input event based on a single fValue
   void buildInputEvent(U32 deviceType, U32 deviceInst, InputEventType objType, InputObjectInstances objInst, InputActionType action, F32 fValue);

   /// Build an input event based on a Point3F
   void buildInputEvent(U32 deviceType, U32 deviceInst, InputEventType objType, InputObjectInstances objInst, InputActionType action, Point3F& pValue);

   /// Build an input event based on a QuatF
   void buildInputEvent(U32 deviceType, U32 deviceInst, InputEventType objType, InputObjectInstances objInst, InputActionType action, QuatF& qValue);

   /// Build an input event based on a AngAxisF
   void buildInputEvent(U32 deviceType, U32 deviceInst, InputEventType objType, InputObjectInstances objInst, InputActionType action, AngAxisF& qValue);

protected:
   U32 mNextDeviceTypeCode;
   U32 mNextDeviceCode;

   Vector<IInputDevice*> mDeviceList;

   // Holds description to VirtualMapData struct
   SimpleHashTable<VirtualMapData> mVirtualMap;
   SimpleHashTable<VirtualMapData> mKeyboardMap;

   // Used to look up a description based on a VirtualMapData.code
   HashTable<U32, VirtualMapData> mActionCodeMap;
   HashTable<U32, VirtualMapData> mScanCodeMap;

protected:
   void buildVirtualMap();
   void buildKeyboardMap();

public:
   // For ManagedSingleton.
   static const char* getSingletonName() { return "InputEventManager"; }   
};

/// Returns the InputEventManager singleton.
#define INPUTMGR ManagedSingleton<InputEventManager>::instance()


#endif
