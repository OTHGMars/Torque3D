#include "T3D/gameBase/extended/extendedMove.h"
#include "core/stream/bitStream.h"
#include "math/mathIO.h"
#include "math/mAngAxis.h"
#include "core/module.h"
#include "console/consoleTypes.h"
#include "core/strings/stringFunctions.h"

MODULE_BEGIN( ExtendedMoveManager )

   MODULE_INIT_AFTER( MoveManager )
   MODULE_INIT
   {
      ExtendedMoveManager::init();
   }

MODULE_END;

bool ExtendedMoveManager::mDeviceIsActive[ExtendedMove::MaxPositionsRotations] = { 0, };
F32 ExtendedMoveManager::mPosX[ExtendedMove::MaxPositionsRotations] = { 0, };
F32 ExtendedMoveManager::mPosY[ExtendedMove::MaxPositionsRotations] = { 0, };
F32 ExtendedMoveManager::mPosZ[ExtendedMove::MaxPositionsRotations] = { 0, };
F32 ExtendedMoveManager::mRotAX[ExtendedMove::MaxPositionsRotations] = { 0, };
F32 ExtendedMoveManager::mRotAY[ExtendedMove::MaxPositionsRotations] = { 0, };
F32 ExtendedMoveManager::mRotAZ[ExtendedMove::MaxPositionsRotations] = { 0, };
F32 ExtendedMoveManager::mRotAW[ExtendedMove::MaxPositionsRotations] = { 1, 1, 1 };
U32 ExtendedMoveManager::mBinBlobSize[ExtendedMove::MaxPositionsRotations] = { 0, };
U8 ExtendedMoveManager::mBinaryBlob[ExtendedMove::MaxPositionsRotations][ExtendedMove::MaxBinBlobSize] = { 0, };

F32 ExtendedMoveManager::mPosScale = 2.0f;

void ExtendedMoveManager::init()
{
   for(U32 i = 0; i < ExtendedMove::MaxPositionsRotations; ++i)
   {
      char varName[256];

      dSprintf(varName, sizeof(varName), "mvDeviceIsActive%d", i);
      Con::addVariable(varName, TypeBool, &mDeviceIsActive[i], 
         "@brief Indicates that the device at this index has an updated position and "
         "rotation in this move packet.\n"
         "@ingroup Game");

      dSprintf(varName, sizeof(varName), "mvPosX%d", i);
      Con::addVariable(varName, TypeF32, &mPosX[i], 
         "X position of controller.\n"
         "@ingroup Game");

      dSprintf(varName, sizeof(varName), "mvPosY%d", i);
      Con::addVariable(varName, TypeF32, &mPosY[i],
         "Y position of controller.\n"
         "@ingroup Game");

      dSprintf(varName, sizeof(varName), "mvPosZ%d", i);
      Con::addVariable(varName, TypeF32, &mPosZ[i],
         "Z position of controller.\n"
         "@ingroup Game");

      dSprintf(varName, sizeof(varName), "mvRotX%d", i);
      Con::addVariable(varName, TypeF32, &mRotAX[i], 
         "X component of controller quaternion.\n"
         "@ingroup Game");

      dSprintf(varName, sizeof(varName), "mvRotY%d", i);
      Con::addVariable(varName, TypeF32, &mRotAY[i], 
         "Y component of controller quaternion.\n"
         "@ingroup Game");

      dSprintf(varName, sizeof(varName), "mvRotZ%d", i);
      Con::addVariable(varName, TypeF32, &mRotAZ[i], 
         "Z component of controller quaternion.\n"
         "@ingroup Game");

      dSprintf(varName, sizeof(varName), "mvRotW%d", i);
      Con::addVariable(varName, TypeF32, &mRotAW[i], 
         "w component of controller quaternion.\n"
         "@ingroup Game");
   }

   Con::addVariable("mvPosScale", TypeF32, &mPosScale,
      "@brief Indicates the scale to be given to mvPos values.\n\n"
      ""
      "@ingroup Game");
}

const ExtendedMove NullExtendedMove;

#define CLAMPSIGNEDFLOAT(f, bitcount) ((S32)(((f + 1) * .5) * ((1 << bitcount) - 1)) & ((1<<bitcount)-1))
#define UNCLAMPSIGNEDFLOAT(x, bitcount) ((F32)(x * 2 / F32((1 << bitcount) - 1) - 1.0f))

ExtendedMove::ExtendedMove() : Move()
{
   for(U32 i=0; i<MaxPositionsRotations; ++i)
   {
      DeviceIsActive[i] = false;
      posX[i] = 0;
      posY[i] = 0;
      posZ[i] = 0;
      rotX[i] = 0;
      rotY[i] = 0;
      rotZ[i] = 0;
      rotW[i] = 1;

      cposX[i] = (1 << (MaxPositionBits - 1)) - 1;
      cposY[i] = cposX[i];
      cposZ[i] = cposX[i];

      crot[i][0] = (1 << (MaxRotationBits - 1)) - 1;
      crot[i][1] = crot[i][0];
      crot[i][2] = crot[i][0];
      cmaxQuatIndex[i] = 3;

      binBlobSize[i] = 0;
   }
}

void ExtendedMove::pack(BitStream *stream, const Move * basemove)
{
   bool alwaysWriteAll = basemove!=NULL;
   if (!basemove)
      basemove = &NullExtendedMove;

   // Write the standard Move stuff
   packMove(stream, basemove, alwaysWriteAll);

   bool extendedDifferent = false;
   for(U32 i=0; i<MaxPositionsRotations; ++i)
   {
      if(DeviceIsActive[i])
         extendedDifferent = true;
   }

   if (stream->writeFlag(extendedDifferent))
   {
      for(U32 i=0; i<MaxPositionsRotations; ++i)
      {
         if (stream->writeFlag(DeviceIsActive[i]))
         {
            // Position
            stream->writeInt(cposX[i], MaxPositionBits);
            stream->writeInt(cposY[i], MaxPositionBits);
            stream->writeInt(cposZ[i], MaxPositionBits);

            // Rotation
            stream->writeInt(cmaxQuatIndex[i], 2);
            stream->writeInt(crot[i][0], MaxRotationBits);
            stream->writeInt(crot[i][1], MaxRotationBits);
            stream->writeInt(crot[i][2], MaxRotationBits);

            // Binary Blob Data
            if (stream->writeFlag(binBlobSize[i] > 0))
            {
               stream->writeInt(binBlobSize[i], MaxBlobSizeBits);
               stream->writeBits(binBlobSize[i] * 8, binaryBlob[i]);
            }
         }
      }
   }
}

void ExtendedMove::unpack(BitStream *stream, const Move * basemove)
{
   bool alwaysReadAll = basemove!=NULL;
   if (!basemove)
      basemove=&NullExtendedMove;

   // Standard Move stuff
   bool isBaseMove = !unpackMove(stream, basemove, alwaysReadAll);

   // ExtendedMove
   const ExtendedMove* extBaseMove = static_cast<const ExtendedMove*>(basemove);

   if (stream->readFlag())
   {
      isBaseMove = false;

      for(U32 i=0; i<MaxPositionsRotations; ++i)
      {
         if (stream->readFlag()) // DeviceIsActive
         {
            DeviceIsActive[i] = true;
            // Position
            cposX[i] = stream->readInt(MaxPositionBits);
            posX[i] = UNCLAMPSIGNEDFLOAT(cposX[i], MaxPositionBits) * ExtendedMoveManager::mPosScale;

            cposY[i] = stream->readInt(MaxPositionBits);
            posY[i] = UNCLAMPSIGNEDFLOAT(cposY[i], MaxPositionBits) * ExtendedMoveManager::mPosScale;

            cposZ[i] = stream->readInt(MaxPositionBits);
            posZ[i] = UNCLAMPSIGNEDFLOAT(cposZ[i], MaxPositionBits) * ExtendedMoveManager::mPosScale;

            // Rotation
            cmaxQuatIndex[i] = stream->readInt(2);
            crot[i][0] = stream->readInt(MaxRotationBits);
            crot[i][1] = stream->readInt(MaxRotationBits);
            crot[i][2] = stream->readInt(MaxRotationBits);
            unclampQuat(i);

            // Binary Blob
            if (stream->readFlag())
            {
               binBlobSize[i] = stream->readInt(MaxBlobSizeBits);
               stream->readBits(binBlobSize[i] * 8, binaryBlob[i]);
            }
         }
         else
         {
            DeviceIsActive[i] = false;
            posX[i] = extBaseMove->posX[i];
            posY[i] = extBaseMove->posY[i];
            posZ[i] = extBaseMove->posZ[i];

            rotX[i] = extBaseMove->rotX[i];
            rotY[i] = extBaseMove->rotY[i];
            rotZ[i] = extBaseMove->rotZ[i];
            rotW[i] = extBaseMove->rotW[i];

            binBlobSize[i] = 0;
         }
      }
   }

   if(isBaseMove)
   {
      *this = *extBaseMove;
   }
}

void ExtendedMove::clamp()
{
   // Clamp the values the same as for net traffic so the client matches the server
   for(U32 i=0; i<MaxPositionsRotations; ++i)
   {
      // Positions
      cposX[i] = CLAMPSIGNEDFLOAT(posX[i] / ExtendedMoveManager::mPosScale, MaxPositionBits);
      cposY[i] = CLAMPSIGNEDFLOAT(posY[i] / ExtendedMoveManager::mPosScale, MaxPositionBits);
      cposZ[i] = CLAMPSIGNEDFLOAT(posZ[i] / ExtendedMoveManager::mPosScale, MaxPositionBits);

      clampQuat(i);
   }

   // Perform the standard Move clamp
   Parent::clamp();
}

void ExtendedMove::unclamp()
{
   // Unclamp the values the same as for net traffic so the client matches the server
   for(U32 i=0; i<MaxPositionsRotations; ++i)
   {
      posX[i] = UNCLAMPSIGNEDFLOAT(cposX[i], MaxPositionBits) * ExtendedMoveManager::mPosScale;
      posY[i] = UNCLAMPSIGNEDFLOAT(cposY[i], MaxPositionBits) * ExtendedMoveManager::mPosScale;
      posZ[i] = UNCLAMPSIGNEDFLOAT(cposZ[i], MaxPositionBits) * ExtendedMoveManager::mPosScale;
      unclampQuat(i);
   }

   // Perform the standard Move unclamp
   Parent::unclamp();
}

void ExtendedMove::clampQuat(U32 deviceIndex)
{  // Find the sign and index of the component with the greatest magnitude.
   // Then compress the remaining three flipping their sign if the greatest was negative.
   F32 quatVals[4] = { rotX[deviceIndex], rotY[deviceIndex], rotZ[deviceIndex], rotW[deviceIndex] };
   bool flipQuat = (quatVals[0] < 0);
   F32 maxVal = mFabs(quatVals[0]);
   S32 idxMax = 0;

   for (S32 i = 1; i < 4; ++i)
   {
      if (mFabs(quatVals[i]) > maxVal)
      {
         idxMax = i;
         maxVal = mFabs(quatVals[i]);
         flipQuat = (quatVals[i] < 0);
      }
   }
   cmaxQuatIndex[deviceIndex] = idxMax;

   S32 outIdx = 0;
   for (S32 i = 0; i < 4 && outIdx < 3; ++i)
   {
      if (i == idxMax)
         continue;
      F32 outVal = (flipQuat ? -quatVals[i] : quatVals[i]) * (F32) M_SQRT2;
      crot[deviceIndex][outIdx++] = CLAMPSIGNEDFLOAT(outVal, MaxRotationBits);
   }
}

void ExtendedMove::unclampQuat(U32 deviceIndex)
{
   F32 quatVals[4];
   F32 sum = 0.0f;
   S32 inIdx = 0;

   // Unpack the smallest three components
   for (S32 i = 0; i < 4; ++i)
   {
      if (i == cmaxQuatIndex[deviceIndex])
         continue;
      quatVals[i] = UNCLAMPSIGNEDFLOAT(crot[deviceIndex][inIdx++], MaxRotationBits) * M_SQRTHALF_F;
      sum += quatVals[i] * quatVals[i];
   }

   // Find the fourth from the sum of the squares of the other three
   if (sum > 1.0f)
      quatVals[cmaxQuatIndex[deviceIndex]] = 1.0f;
   else
      quatVals[cmaxQuatIndex[deviceIndex]] = mSqrt(1.0f - sum);

   rotX[deviceIndex] = quatVals[0];
   rotY[deviceIndex] = quatVals[1];
   rotZ[deviceIndex] = quatVals[2];
   rotW[deviceIndex] = quatVals[3];
   //AssertFatal(!isnan(quatVals[0]) && !isnan(quatVals[1]) && !isnan(quatVals[2]) && !isnan(quatVals[3]), "Malformed Compressed Quaternion");
}
