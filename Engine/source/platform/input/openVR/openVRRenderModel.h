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

#ifndef _OPENVR_RENDER_MODEL_H_
#define _OPENVR_RENDER_MODEL_H_

#include "math/mQuat.h"
#include "math/mPoint4.h"

#include "gfx/gfxVertexBuffer.h"
#include "gfx/gfxPrimitiveBuffer.h"

#include <openvr.h>

class BaseMatInstance;
class SceneRenderState;
struct MeshRenderInst;

/// Simple class to handle rendering native OpenVR model data
class OpenVRRenderModel
{
public:
   typedef GFXVertexPNT VertexType;
   GFXVertexBufferHandle<VertexType> mVertexBuffer;
   GFXPrimitiveBufferHandle mPrimitiveBuffer;
   BaseMatInstance* mMaterialInstance; ///< Material to use for rendering. NOTE:  
   Box3F mLocalBox;

   OpenVRRenderModel() : mMaterialInstance(NULL)
   {
   }

   virtual ~OpenVRRenderModel();

   Box3F getWorldBox(MatrixF &mat)
   {
      Box3F ret = mLocalBox;
      mat.mul(ret);
      return ret;
   }

   Box3F getLocalBox() { return mLocalBox; }

   bool init(const vr::RenderModel_t & vrModel, StringTableEntry materialName);
   bool reinitMaterial(StringTableEntry materialName);
   void draw(SceneRenderState *state, MeshRenderInst* renderInstance);
};

#endif //_OPENVR_RENDER_MODEL_H_