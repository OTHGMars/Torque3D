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

#include "platform/platform.h"

#include "gui/core/guiControl.h"
#include "gui/controls/guiBitmapCtrl.h"
#include "console/consoleTypes.h"
#include "scene/sceneManager.h"
#include "T3D/gameBase/gameConnection.h"
#include "T3D/shapeBase.h"
#include "gfx/gfxDrawUtil.h"
#include "console/engineAPI.h"
#include "gui/core/guiOffscreenCanvas.h"
#include "T3D/tsStatic.h"


//-----------------------------------------------------------------------------
/// Very basic cross hair hud.
/// Uses the base bitmap control to render a bitmap, and decides whether
/// to draw or not depending on the current control object and it's state.
/// If there is ShapeBase object under the cross hair and it's named,
/// then a small health bar is displayed.
class GuiCrossHairHud : public GuiBitmapCtrl
{
   typedef GuiBitmapCtrl Parent;

   LinearColorF   mDamageFillColor;
   LinearColorF   mDamageFrameColor;
   Point2I  mDamageRectSize;
   Point2I  mDamageOffset;

   F32 mMaxInteractDistance;
   bool mReticleCursor;
   S32 mUpdateDelay;
   PlatformTimer* mFrameTime;
   GuiOffscreenCanvas* mInteractCanvas;

protected:
   void drawDamage(Point2I offset, F32 damage, F32 opacity);
   bool testCanvasInteraction(const Point3F &start, const Point3F &end, const RayInfo &info, GameBase* control);

public:
   GuiCrossHairHud();
   ~GuiCrossHairHud();

   void onRender( Point2I, const RectI &);
   static void initPersistFields();

   DECLARE_CALLBACK( void, onStartCanvasInteract, ( GuiOffscreenCanvas *pCanvas ) );
   DECLARE_CONOBJECT( GuiCrossHairHud );
   DECLARE_CATEGORY( "Gui Game" );
   DECLARE_DESCRIPTION( "Basic cross hair hud. Reacts to state of control object.\n"
      "Also displays health bar for named objects under the cross hair." );
};

/// Valid object types for which the cross hair will render, this
/// should really all be script controlled.
static const U32 ObjectMask = PlayerObjectType | VehicleObjectType;


//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT( GuiCrossHairHud );

ConsoleDocClass( GuiCrossHairHud,
   "@brief Basic cross hair hud. Reacts to state of control object. Also displays health bar for named objects under the cross hair.\n\n"
   "Uses the base bitmap control to render a bitmap, and decides whether to draw or not depending "
   "on the current control object and it's state. If there is ShapeBase object under the cross hair "
   "and it's named, then a small health bar is displayed.\n\n"
     
   "@tsexample\n"
		"\n new GuiCrossHairHud()"
		"{\n"
		"	damageFillColor = \"1.0 0.0 0.0 1.0\"; // Fills with a solid red color\n"
		"	damageFrameColor = \"1.0 1.0 1.0 1.0\"; // Solid white frame color\n"
		"	damageRect = \"15 5\";\n"
		"	damageOffset = \"0 -10\";\n"
		"};\n"
   "@endtsexample\n"
   
   "@ingroup GuiGame\n"
);

IMPLEMENT_CALLBACK( GuiCrossHairHud, onStartCanvasInteract, void, ( GuiOffscreenCanvas *pCanvas ), ( pCanvas ),
   "Called whenever the crosshair enters or leaves an interactable offscreen canvas and the control object is "
   "within the interact distance for that object and for the GuiCrossHairHud.\n\n"
   "@param pCanvas The canvas the crosshair is over or NULL when the crosshair leaves the target." );

GuiCrossHairHud::GuiCrossHairHud()
{
   mDamageFillColor.set( 0.0f, 1.0f, 0.0f, 1.0f );
   mDamageFrameColor.set( 1.0f, 0.6f, 0.0f, 1.0f );
   mDamageRectSize.set(50, 4);
   mDamageOffset.set(0,32);

   mMaxInteractDistance = 0.0f;
   mReticleCursor = false;
   mUpdateDelay = 16;

   mFrameTime = PlatformTimer::create();
   mInteractCanvas = NULL;
}

GuiCrossHairHud::~GuiCrossHairHud()
{
   SAFE_DELETE(mFrameTime);
}

void GuiCrossHairHud::initPersistFields()
{
   addGroup("Damage");		
   addField( "damageFillColor", TypeColorF, Offset( mDamageFillColor, GuiCrossHairHud ), "As the health bar depletes, this color will represent the health loss amount." );
   addField( "damageFrameColor", TypeColorF, Offset( mDamageFrameColor, GuiCrossHairHud ), "Color for the health bar's frame." );
   addField( "damageRect", TypePoint2I, Offset( mDamageRectSize, GuiCrossHairHud ), "Size for the health bar portion of the control." );
   addField( "damageOffset", TypePoint2I, Offset( mDamageOffset, GuiCrossHairHud ), "Offset for drawing the damage portion of the health control." );
   endGroup("Damage");

   addGroup("Canvas Interaction");
   addField("maxInteractDistance", TypeF32, Offset(mMaxInteractDistance, GuiCrossHairHud),
      "(default 0) If the reticle is over a TSStatic object, it must be within this distance to be checked for canvas textures. "
      "Set this value to 0 to disable all offscreen canvas interaction.");
   addField("showCursorAsReticle", TypeBool, Offset(mReticleCursor, GuiCrossHairHud),
      "(default false) If true, the cursor from the offscreen canvas will be rendered as the reticle "
      "with the 'hotspot' at the center of the extent when over an offscreen canvas. If false, the "
      "reticle will be hidden and the offscreen canvas will need to render it's own cursor.");
   addField("canvasUpdateMS", TypeS32, Offset(mUpdateDelay, GuiCrossHairHud),
      "(default 16) Sets the offscreen canvas check/update frequency. This is the minimum time "
      "between raycasts to offscreen canvases. Set to 0 to update every frame.");
   endGroup("Canvas Interaction");
   Parent::initPersistFields();
}


//-----------------------------------------------------------------------------

void GuiCrossHairHud::onRender(Point2I offset, const RectI &updateRect)
{
   // Must have a connection and player control object
   GameConnection* conn = GameConnection::getConnectionToServer();
   if (!conn)
      return;
   GameBase* control = dynamic_cast<GameBase*>(conn->getCameraObject());
   if (!control || !(control->getTypeMask() & ObjectMask) || !conn->isFirstPerson())
      return;

   // Get control camera info
   MatrixF cam;
   Point3F camPos;
   conn->getControlCameraTransform(0,&cam);
   cam.getColumn(3, &camPos);

   // Extend the camera vector to create an endpoint for our ray
   Point3F endPos;
   cam.getColumn(1, &endPos);
   endPos *= gClientSceneGraph->getVisibleDistance();
   endPos += camPos;

   // Collision info. We're going to be running LOS tests and we
   // don't want to collide with the control object.
   static U32 losMask = TerrainObjectType | ShapeBaseObjectType | StaticShapeObjectType;
   control->disableCollision();

   RayInfo info;
   bool rayHit = gClientContainer.castRay(camPos, endPos, losMask, &info);

   // Restore control object collision
   control->enableCollision();

   if (rayHit && (mMaxInteractDistance > info.distance) && testCanvasInteraction(camPos, endPos, info, control))
   {  // If the ray hit an offscreen canvas, skip out on rendering the crosshair
      if (mReticleCursor)
      {  // Draw the mouse cursor at the reticle center
         GuiCursor* mouseCursor = mInteractCanvas->getMouseCursor();
         if (mouseCursor)
         {
            Point2I pos((S32)offset.x + updateRect.extent.x /2, (S32)offset.y + updateRect.extent.y / 2);
            Point2I spot = mouseCursor->getHotSpot();
            pos -= spot;
            mouseCursor->render(pos);
         }
      }
      return;
   }
   else if (mInteractCanvas)
   {
      onStartCanvasInteract_callback(NULL);
      mInteractCanvas = NULL;
   }

   // Parent render.
   Parent::onRender(offset,updateRect);

   if (rayHit)
   {
      // Hit something... but we'll only display health for named
      // ShapeBase objects.  Could mask against the object type here
      // and do a static cast if it's a ShapeBaseObjectType, but this
      // isn't a performance situation, so I'll just use dynamic_cast.
      if (ShapeBase* obj = dynamic_cast<ShapeBase*>(info.object))
         if (obj->getShapeName()) {
            offset.x = updateRect.point.x + updateRect.extent.x / 2;
            offset.y = updateRect.point.y + updateRect.extent.y / 2;
            drawDamage(offset + mDamageOffset, obj->getDamageValue(), 1);
         }
   }
}


//-----------------------------------------------------------------------------
/**
   Display a damage bar ubove the shape.
   This is a support funtion, called by onRender.
*/
void GuiCrossHairHud::drawDamage(Point2I offset, F32 damage, F32 opacity)
{
   mDamageFillColor.alpha = mDamageFrameColor.alpha = opacity;

   // Damage should be 0->1 (0 being no damage,or healthy), but
   // we'll just make sure here as we flip it.
   damage = mClampF(1 - damage, 0, 1);

   // Center the bar
   RectI rect(offset, mDamageRectSize);
   rect.point.x -= mDamageRectSize.x / 2;

   // Draw the border
   GFX->getDrawUtil()->drawRect(rect, mDamageFrameColor.toColorI());

   // Draw the damage % fill
   rect.point += Point2I(1, 1);
   rect.extent -= Point2I(1, 1);
   rect.extent.x = (S32)(rect.extent.x * damage);
   if (rect.extent.x == 1)
      rect.extent.x = 2;
   if (rect.extent.x > 0)
      GFX->getDrawUtil()->drawRectFill(rect, mDamageFillColor.toColorI());
}

bool GuiCrossHairHud::testCanvasInteraction(const Point3F &startPos, const Point3F &endPos, const RayInfo &info, GameBase* control)
{
   if (mFrameTime->getElapsedMs() > mUpdateDelay)
   {
      mFrameTime->reset();
      GuiOffscreenCanvas* currentCanvas = mInteractCanvas;
      mInteractCanvas = NULL;

      TSStatic* obj = dynamic_cast<TSStatic*>(info.object);
      if (obj)
      {
         Point3F xformedStart, xformedEnd;
         obj->getWorldTransform().mulP(startPos, &xformedStart);
         obj->getWorldTransform().mulP(endPos, &xformedEnd);
         xformedStart.convolveInverse(obj->getScale());
         xformedEnd.convolveInverse(obj->getScale());

         RayInfo localInfo;
         localInfo.generateTexCoord = true;
         if (obj->getShapeInstance()->castRayOpcode(0, xformedStart, xformedEnd, &localInfo))
         {
            GuiOffscreenCanvas *pCanvas = GuiOffscreenCanvas::getCanvasFromRayInfo(localInfo);
            if (pCanvas && pCanvas->canInteract() && info.distance < pCanvas->getMaxInteractDistance())
            {
               if (currentCanvas != pCanvas)
                  onStartCanvasInteract_callback(pCanvas);

               pCanvas->setCursorPosFromUV(localInfo.texCoord);
               mInteractCanvas = pCanvas;
            }
         }
      }

      if (currentCanvas && !mInteractCanvas)
         onStartCanvasInteract_callback(NULL);

   }

   return (mInteractCanvas != NULL);
}