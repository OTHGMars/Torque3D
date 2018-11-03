#ifndef _EXTENDEDMOVE_H_
#define _EXTENDEDMOVE_H_

#include "T3D/gameBase/moveManager.h"
#include "math/mQuat.h"

struct ExtendedMove : public Move
{
   typedef Move Parent;

   enum Constants {
      MaxPositionsRotations = 3,

      MaxPositionBits = 16,
      MaxRotationBits = 15,
   };

   bool DeviceIsActive[MaxPositionsRotations];

   // Absolute position
   F32 posX[MaxPositionsRotations], posY[MaxPositionsRotations], posZ[MaxPositionsRotations];
   S32 cposX[MaxPositionsRotations], cposY[MaxPositionsRotations], cposZ[MaxPositionsRotations];

   // Absolute Rotation
   F32 rotX[MaxPositionsRotations], rotY[MaxPositionsRotations], rotZ[MaxPositionsRotations], rotW[MaxPositionsRotations];
   S32 crot[MaxPositionsRotations][3];
   S32 cmaxQuatIndex[MaxPositionsRotations];

   ExtendedMove();

   virtual void pack(BitStream *stream, const Move * move = NULL);
   virtual void unpack(BitStream *stream, const Move * move = NULL);

   virtual void clamp();
   virtual void unclamp();
   virtual void clampQuat(U32 deviceIndex);
   virtual void unclampQuat(U32 deviceIndex);
};

extern const ExtendedMove NullExtendedMove;

class ExtendedMoveManager
{
public:
   static bool mDeviceIsActive[ExtendedMove::MaxPositionsRotations];
   static F32 mPosX[ExtendedMove::MaxPositionsRotations];
   static F32 mPosY[ExtendedMove::MaxPositionsRotations];
   static F32 mPosZ[ExtendedMove::MaxPositionsRotations];
   static F32 mRotAX[ExtendedMove::MaxPositionsRotations];
   static F32 mRotAY[ExtendedMove::MaxPositionsRotations];
   static F32 mRotAZ[ExtendedMove::MaxPositionsRotations];
   static F32 mRotAW[ExtendedMove::MaxPositionsRotations];

   static F32 mPosScale;

   static void init();
};

#endif   // _EXTENDEDMOVE_H_
