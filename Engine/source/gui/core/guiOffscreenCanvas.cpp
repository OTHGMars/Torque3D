#include "gui/core/guiOffscreenCanvas.h"
#include "gfx/gfxDrawUtil.h"
#include "gfx/gfxTextureManager.h"
#include "gfx/gfxAPI.h"
#include "gfx/gfxDebugEvent.h"

#include "materials/baseMatInstance.h"
#include "materials/matInstance.h"
#include "materials/materialDefinition.h"

#include "console/consoleTypes.h"
#include "console/console.h"

IMPLEMENT_CONOBJECT(GuiOffscreenCanvas);

GuiOffscreenCanvas* GuiOffscreenCanvas::sActiveOffscreenCanvas = NULL;
Vector<GuiOffscreenCanvas*> GuiOffscreenCanvas::sList;

GuiOffscreenCanvas::GuiOffscreenCanvas()
{
   mTargetFormat = GFXFormatR8G8B8A8;
   mTargetSize = Point2I(256,256);
   mTargetName = "offscreenCanvas";
   mTargetDirty = true;
   mDynamicTarget = false;
   mUseDepth = false;
   mCanInteract = false;
   mMaxInteractDistance = 0.0f;
   mRenderCount = 0;
}

GuiOffscreenCanvas::~GuiOffscreenCanvas()
{
   mPlatformWindow = NULL;
}

void GuiOffscreenCanvas::initPersistFields()
{
   addField( "targetSize", TypePoint2I, Offset( mTargetSize, GuiOffscreenCanvas ),"" );
   addField( "targetFormat", TypeGFXFormat, Offset( mTargetFormat, GuiOffscreenCanvas ), "");
   addField( "targetName", TypeRealString, Offset( mTargetName, GuiOffscreenCanvas ), "");
   addField( "dynamicTarget", TypeBool, Offset( mDynamicTarget, GuiOffscreenCanvas ), "");
   addField( "useDepth", TypeBool, Offset( mUseDepth, GuiOffscreenCanvas ), "");
   addField( "canInteract", TypeBool, Offset( mCanInteract, GuiOffscreenCanvas ), "If true the user can interact with this object via crosshair and mouse click when the canvas is rendered on a TSShapeInstance.");
   addField( "maxInteractDistance", TypeF32, Offset( mMaxInteractDistance, GuiOffscreenCanvas ), "The camera must be within this distance to enable user interaction with the canvas if canInteract is true.");
   addField( "renderCount", TypeS32, Offset( mRenderCount, GuiOffscreenCanvas ), "");

   Parent::initPersistFields();
}

GuiOffscreenCanvas *GuiOffscreenCanvas::getCanvasFromRayInfo(RayInfo &info)
{
   if (sList.size() > 0)
   {
      MatInstance* matInst = dynamic_cast<MatInstance*>(info.material);
      if (matInst)
      {
         Material* testMat = matInst->getMaterial();
         if (testMat && testMat->mDiffuseMapFilename[0].startsWith("#"))
         {
            String testName = testMat->mDiffuseMapFilename[0].substr(1);
            for (S32 i = 0; i < sList.size(); ++i)
            {
               if (sList[i]->getTarget()->getName() == testName)
                  return sList[i];
            }
         }
      }
   }
   return NULL;
}

bool GuiOffscreenCanvas::onAdd()
{
   if (GuiControl::onAdd()) // jamesu - skip GuiCanvas onAdd since it sets up GFX which is bad
   {
      // ensure that we have a cursor
      setCursor(dynamic_cast<GuiCursor*>(Sim::findObject("DefaultCursor")));
      
      GuiCanvas* cv = dynamic_cast<GuiCanvas*>(Sim::findObject("Canvas"));
      if (cv)
         mPlatformWindow = cv->getPlatformWindow();

      mRenderFront = true;
      sList.push_back(this);

      //Con::printf("Registering target %s...", mTargetName.c_str());
      mNamedTarget.registerWithName( mTargetName );

      _setupTargets();

      GFXTextureManager::addEventDelegate( this, &GuiOffscreenCanvas::_onTextureEvent );

      return true;
   }
   return false;
}

void GuiOffscreenCanvas::onRemove()
{
   GFXTextureManager::removeEventDelegate( this, &GuiOffscreenCanvas::_onTextureEvent );

   _teardownTargets();

   U32 idx = sList.find_next(this);
   if (idx != (U32)-1)
   {
      sList.erase(idx);
   }

   if (isActiveCanvas())
      sActiveOffscreenCanvas = NULL;

   mTarget = NULL;
   mTargetTexture = NULL;
   mTargetDepth = NULL;

   Parent::onRemove();
}

void GuiOffscreenCanvas::_setupTargets()
{
   _teardownTargets();

   if (!mTarget.isValid())
   {
      mTarget = GFX->allocRenderToTextureTarget();
   }

   // Update color
   if (!mTargetTexture.isValid() || mTargetSize != mTargetTexture.getWidthHeight())
   {
      mTargetTexture.set( mTargetSize.x, mTargetSize.y, mTargetFormat, &GFXRenderTargetSRGBProfile, avar( "%s() - (line %d)", __FUNCTION__, __LINE__ ), 1, 0 );
   }

   // Update depth if needed
   if (mUseDepth && (!mTargetDepth.isValid() || mTargetSize != mTargetDepth.getWidthHeight()))
   {
      mTargetDepth.set( mTargetSize.x, mTargetSize.y, GFXFormatD24S8, &GFXRenderTargetProfile, avar( "%s() - (line %d)", __FUNCTION__, __LINE__ ), 1, 0 );
      mTarget->attachTexture( GFXTextureTarget::RenderSlot(GFXTextureTarget::DepthStencil), mTargetDepth );
   }

   mTarget->attachTexture( GFXTextureTarget::RenderSlot(GFXTextureTarget::Color0), mTargetTexture );
   mNamedTarget.setTexture(0, mTargetTexture);
}

void GuiOffscreenCanvas::_teardownTargets()
{
   mNamedTarget.release();
   mTargetTexture = NULL;
   mTargetDepth = NULL;
   mTargetDirty = true;
}

void GuiOffscreenCanvas::renderFrame(bool preRenderOnly, bool bufferSwap /* = true */)
{
   if (!mTargetDirty)
      return;
   
#ifdef TORQUE_ENABLE_GFXDEBUGEVENTS
   char buf[256];
   dSprintf(buf, sizeof(buf), "OffsceenCanvas %s", getName() ? getName() : getIdString());
   GFXDEBUGEVENT_SCOPE_EX(GuiOffscreenCanvas_renderFrame, ColorI::GREEN, buf);
#endif
   
   PROFILE_START(OffscreenCanvasPreRender);

#ifdef TORQUE_GFX_STATE_DEBUG
   GFX->getDebugStateManager()->startFrame();
#endif

   if (mTarget->getSize() != mTargetSize)
   {
      _setupTargets();
      mNamedTarget.setViewport( RectI( Point2I::Zero, mTargetSize ) );
   }

   // Make sure the root control is the size of the canvas.
   Point2I size = mTarget->getSize();

   if(size.x == 0 || size.y == 0)
   {
      PROFILE_END();
      return;
   }

   RectI screenRect(0, 0, size.x, size.y);

   maintainSizing();

   //preRender (recursive) all controls
   preRender();

   PROFILE_END();

   // Are we just doing pre-render?
   if(preRenderOnly)
   {
      return;
   }

   resetUpdateRegions();

   PROFILE_START(OffscreenCanvasRenderControls);

   GuiCursor *mouseCursor = NULL;
   bool cursorVisible = true;

   Point2I cursorPos((S32)mCursorPt.x, (S32)mCursorPt.y);
   mouseCursor = mDefaultCursor;

	mLastCursorEnabled = cursorVisible;
	mLastCursor = mouseCursor;
	mLastCursorPt = cursorPos;

   // Set active target
   GFX->pushActiveRenderTarget();
   GFX->setActiveRenderTarget(mTarget);

   // Clear the current viewport area
   GFX->setViewport( screenRect );
   GFX->clear( GFXClearTarget, LinearColorF(0,0,0,0), 1.0f, 0 );

   resetUpdateRegions();

	// Make sure we have a clean matrix state 
   // before we start rendering anything!   
   GFX->setWorldMatrix( MatrixF::Identity );
   GFX->setViewMatrix( MatrixF::Identity );
   GFX->setProjectionMatrix( MatrixF::Identity );
   
   RectI contentRect(Point2I(0,0), mTargetSize);
   {
      // Render active GUI Dialogs
      for(iterator i = begin(); i != end(); i++)
      {
         // Get the control
         GuiControl *contentCtrl = static_cast<GuiControl*>(*i);
         
         GFX->setClipRect( contentRect );
         GFX->setStateBlock(mDefaultGuiSB);
         
         contentCtrl->onRender(contentCtrl->getPosition(), contentRect);
      }

      // Fill Blue if no Dialogs
      if(this->size() == 0)
         GFX->clear( GFXClearTarget, LinearColorF(0,0,0.5f,0.5f), 1.0f, 0 );

      GFX->setClipRect( contentRect );

      // Draw cursor
      // 
      if (mCursorEnabled && mouseCursor && mShowCursor)
      {
         Point2I pos((S32)mCursorPt.x, (S32)mCursorPt.y);
         Point2I spot = mouseCursor->getHotSpot();

         pos -= spot;
         mouseCursor->render(pos);
      }

      GFX->getDrawUtil()->clearBitmapModulation();
   }

   mTarget->resolve();
   GFX->popActiveRenderTarget();

   PROFILE_END();

   // Keep track of the last time we rendered.
   mLastRenderMs = Platform::getRealMilliseconds();
   mTargetDirty = mDynamicTarget;

   onFrameRendered();
}

void GuiOffscreenCanvas::onFrameRendered()
{
   mRenderCount++;
}

Point2I GuiOffscreenCanvas::getWindowSize()
{
   return mTargetSize;
}

Point2I GuiOffscreenCanvas::getCursorPos()
{
   return Point2I(mCursorPt.x, mCursorPt.y);
}

void GuiOffscreenCanvas::setCursorPos(const Point2I &pt)
{
   mCursorPt.x = F32( pt.x );
   mCursorPt.y = F32( pt.y );
}

void GuiOffscreenCanvas::setCursorPosFromUV(const Point2F &pt)
{
   Point2F oldPt = mCursorPt;
   mCursorPt.x = mRound(mClampF( mTargetSize.x - (pt.x * mTargetSize.x), 0.0f, (F32) mTargetSize.x ));
   mCursorPt.y = mRound(mClampF( mTargetSize.y - (pt.y * mTargetSize.y), 0.0f, (F32) mTargetSize.y ));
   if (oldPt != mCursorPt)
      markDirty();
}

void GuiOffscreenCanvas::showCursor(bool state)
{ 
   mShowCursor = state;
}

bool GuiOffscreenCanvas::isCursorShown()
{
   return mShowCursor;
}

void GuiOffscreenCanvas::_onTextureEvent( GFXTexCallbackCode code )
{
   switch(code)
   {
      case GFXZombify:
         _teardownTargets();
         break;

      case GFXResurrect:
         _setupTargets();
         break;
   }
}

void GuiOffscreenCanvas::setCanvasActive(bool active)
{
   if (active)
      sActiveOffscreenCanvas = this;
   else
   {
      if (isActiveCanvas())
         sActiveOffscreenCanvas = NULL;

      if (mPlatformWindow && mPlatformWindow->getKeyboardTranslation())
         mPlatformWindow->setKeyboardTranslation(false);
   }
}

void GuiOffscreenCanvas::dumpTarget(const char *filename)
{
    mTargetTexture->dumpToDisk("PNG", filename);
}

DefineEngineMethod(GuiOffscreenCanvas, dumpTarget, void, (const char *filename), , "Saves the render target texture as a png.")
{
    object->dumpTarget(filename);
}

DefineEngineMethod(GuiOffscreenCanvas, resetTarget, void, (), , "")
{
   object->_setupTargets();
}

DefineEngineMethod(GuiOffscreenCanvas, markDirty, void, (), , "")
{
   object->markDirty();
}

DefineEngineMethod(GuiOffscreenCanvas, isActiveCanvas, bool, (), , "")
{
   return object->isActiveCanvas();
}

DefineEngineMethod(GuiOffscreenCanvas, setActiveCanvas, void, (bool active), ( false ), "Sets the canvas active for keyboard/mouse input.")
{
   object->setCanvasActive(active);
}
