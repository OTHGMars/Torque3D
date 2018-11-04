//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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

#include "platform/input/openVR/openVRRenderModel.h"
#include "platform/input/openVR/openVRProvider.h"

#include "scene/sceneRenderState.h"
#include "renderInstance/renderPassManager.h"
#include "materials/baseMatInstance.h"
#include "materials/materialManager.h"

OpenVRRenderModel::~OpenVRRenderModel()
{
   SAFE_DELETE(mMaterialInstance);
}

bool OpenVRRenderModel::init(const vr::RenderModel_t & vrModel, StringTableEntry materialName)
{
   SAFE_DELETE(mMaterialInstance);
   mMaterialInstance = MATMGR->createMatInstance(materialName, getGFXVertexFormat< VertexType >());
   if (!mMaterialInstance)
      return false;

   mLocalBox = Box3F::Invalid;

   // Prepare primitives
   U16 *indPtr = NULL;
   GFXPrimitive *primPtr = NULL;
   mPrimitiveBuffer.set(GFX, vrModel.unTriangleCount * 3, 1, GFXBufferTypeStatic, "OpenVR Controller buffer");

   mPrimitiveBuffer.lock(&indPtr, &primPtr);
   if (!indPtr || !primPtr)
      return false;

   primPtr->minIndex = 0;
   primPtr->numPrimitives = vrModel.unTriangleCount;
   primPtr->numVertices = vrModel.unVertexCount;
   primPtr->startIndex = 0;
   primPtr->startVertex = 0;
   primPtr->type = GFXTriangleList;

   for (U32 i = 0; i < vrModel.unTriangleCount; i++)
   {
      const U32 idx = i * 3;
      indPtr[idx + 0] = vrModel.rIndexData[idx + 2];
      indPtr[idx + 1] = vrModel.rIndexData[idx + 1];
      indPtr[idx + 2] = vrModel.rIndexData[idx + 0];
   }

   mPrimitiveBuffer.unlock();

   // Prepare verts
   mVertexBuffer.set(GFX, vrModel.unVertexCount, GFXBufferTypeStatic);
   VertexType *vertPtr = mVertexBuffer.lock();
   if (!vertPtr)
      return false;

   // Convert to torque coordinate system
   for (U32 i = 0; i < vrModel.unVertexCount; i++)
   {
      const vr::RenderModel_Vertex_t &vert = vrModel.rVertexData[i];
      vertPtr->point = OpenVRUtil::convertPointFromOVR(vert.vPosition);
      vertPtr->point.x = -vertPtr->point.x;
      vertPtr->point.y = -vertPtr->point.y;
      vertPtr->point.z = -vertPtr->point.z;
      vertPtr->normal = OpenVRUtil::convertPointFromOVR(vert.vNormal);
      vertPtr->normal.x = -vertPtr->normal.x;
      vertPtr->normal.y = -vertPtr->normal.y;
      vertPtr->normal.z = -vertPtr->normal.z;
      vertPtr->texCoord = Point2F(vert.rfTextureCoord[0], vert.rfTextureCoord[1]);
      vertPtr++;
   }

   mVertexBuffer.unlock();

   for (U32 i = 0, sz = vrModel.unVertexCount; i < sz; i++)
   {
      Point3F pos = Point3F(vrModel.rVertexData[i].vPosition.v[0], vrModel.rVertexData[i].vPosition.v[1], vrModel.rVertexData[i].vPosition.v[2]);
      mLocalBox.extend(pos);
   }

   return true;
}

bool OpenVRRenderModel::reinitMaterial(StringTableEntry materialName)
{
   SAFE_DELETE(mMaterialInstance);
   mMaterialInstance = MATMGR->createMatInstance(materialName, getGFXVertexFormat< VertexType >());

   return (mMaterialInstance != NULL);
}

void OpenVRRenderModel::draw(SceneRenderState *state, MeshRenderInst* renderInstance)
{
   renderInstance->type = RenderPassManager::RIT_Mesh;
   renderInstance->matInst = state->getOverrideMaterial(mMaterialInstance);
   if (!renderInstance->matInst)
      return;

   renderInstance->vertBuff = &mVertexBuffer;
   renderInstance->primBuff = &mPrimitiveBuffer;
   renderInstance->prim = NULL;
   renderInstance->primBuffIndex = 0;

   if (renderInstance->matInst->getMaterial()->isTranslucent())
   {
      renderInstance->type = RenderPassManager::RIT_Translucent;
      renderInstance->translucentSort = true;
   }

   renderInstance->defaultKey = renderInstance->matInst->getStateHint();
   renderInstance->defaultKey2 = (uintptr_t)renderInstance->vertBuff;
}
