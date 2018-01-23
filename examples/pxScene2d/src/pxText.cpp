/*

 pxCore Copyright 2005-2017 John Robinson

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

// pxText.cpp

#include "pxText.h"
#include "rtFileDownloader.h"
#include "pxTimer.h"
#include "pxFont.h"
#include "pxContext.h"

extern pxContext context;


pxText::pxText(pxScene2d* scene):pxObject(scene), mFontLoaded(false), mDirty(true), mFontDownloadRequest(NULL), mListenerAdded(false)
{
  float c[4] = {1, 1, 1, 1};
  memcpy(mTextColor, c, sizeof(mTextColor));
  // Default to use default font
  mFont = pxFontManager::getFont(defaultFont);
  mPixelSize = defaultPixelSize;
}

pxText::~pxText()
{
  if (mListenerAdded)
  {
    if (getFontResource())
    {
      getFontResource()->removeListener(this);
    }
    mListenerAdded = false;
  }
}

void pxText::onInit()
{
  mInitialized = true;

  if( getFontResource() != NULL && getFontResource()->isFontLoaded()) {
    resourceReady("resolve");
  }
}
rtError pxText::text(rtString& s) const { s = mText; return RT_OK; }

void pxText::sendPromise() 
{ 
  if(mInitialized && mFontLoaded && !((rtPromise*)mReady.getPtr())->status()) 
  {
    //rtLogDebug("pxText SENDPROMISE\n");
    mReady.send("resolve",this); 
  }
}

rtError pxText::setText(const char* s) 
{ 
  //rtLogInfo("pxText::setText\n");
  if( !mText.compare(s)){
    rtLogDebug("pxText.setText setting to same value %s and %s\n", mText.cString(), s);
    return RT_OK;
  }
  mText = s; 
  if( getFontResource() != NULL && getFontResource()->isFontLoaded())
  {
    createNewPromise();
    getFontResource()->measureTextInternal(s, mPixelSize, 1.0, 1.0, mw, mh);
  }
  return RT_OK; 
}

rtError pxText::setPixelSize(uint32_t v) 
{   
  //rtLogInfo("pxText::setPixelSize\n");
  mPixelSize = v; 
  if( getFontResource() != NULL && getFontResource()->isFontLoaded())
  {
    createNewPromise();
    getFontResource()->measureTextInternal(mText, mPixelSize, 1.0, 1.0, mw, mh);
  }
  return RT_OK; 
}

void pxText::resourceReady(rtString readyResolution)
{
  if( !readyResolution.compare("resolve"))
  {    
    mFontLoaded=true;

    // pxText gets its height and width from the text itself, 
    // so measure it
    if (getFontResource() != NULL) {
       getFontResource()->measureTextInternal(mText, mPixelSize, 1.0, 1.0, mw, mh);
	  }
	
    mDirty=true;  
    mScene->mDirty = true;
    // !CLF: ToDo Use pxObject::onTextureReady() and rename it.
    if( mInitialized) 
      pxObject::onTextureReady();
    
  }
  else 
  {
      pxObject::onTextureReady();
      mReady.send("reject",this);
  }     
}
       


void pxText::draw() 
{
  static pxTextureRef nullMaskRef;
  if( getFontResource() != NULL && getFontResource()->isFontLoaded())
  {

    if (mDirty)
    {
      getFontResource()->renderTextToQuads(mText,mPixelSize,msx,msy,mQuads);
      mDirty = false;
    }
    mQuads.draw(0,0,mTextColor);
  }  

}

rtError pxText::setFontUrl(const char* s)
{
  //rtLogInfo("pxText::setFaceUrl for %s\n",s);
  if (!s || !s[0]) {
    s = defaultFont;
  }
  mFontLoaded = false;
  createNewPromise();

  mFont = pxFontManager::getFont(s);
  mListenerAdded = true;
  if (getFontResource() != NULL)
  {
    getFontResource()->addListener(this);
  }
  
  return RT_OK;
}

rtError pxText::setFont(rtObjectRef o) 
{ 
  mFontLoaded = false;
  createNewPromise();

  // !CLF: TODO: Need validation/verification of o
  mFont = o; 
  mListenerAdded = true;
  if (getFontResource() != NULL) {
    getFontResource()->addListener(this);
  }
    
  return RT_OK; 
}

float pxText::getOnscreenWidth()
{
  // TODO review max texture handling
  return (mw > MAX_TEXTURE_WIDTH?MAX_TEXTURE_WIDTH:mw);
}
float pxText::getOnscreenHeight()
{
  // TODO review max texture handling
  return (mh > MAX_TEXTURE_HEIGHT?MAX_TEXTURE_HEIGHT:mh);
}

float pxText::getFBOWidth() 
{ 
  if( mw > MAX_TEXTURE_WIDTH) 
  {
    rtLogWarn("Text width is larger than maximum texture allowed: %lf.  Maximum texture size of %d will be used.",mw, MAX_TEXTURE_WIDTH);  
    return MAX_TEXTURE_WIDTH;
  }
  else 
    return mw; 
}

float pxText::getFBOHeight() 
{ 
  if( mh > MAX_TEXTURE_HEIGHT) 
  {
    rtLogWarn("Text height is larger than maximum texture allowed: %lf.  Maximum texture size of %d will be used.",mh, MAX_TEXTURE_HEIGHT);
    return MAX_TEXTURE_HEIGHT;
  }
  else 
    return mh; 
} 

rtDefineObject(pxText, pxObject);
rtDefineProperty(pxText, text);
rtDefineProperty(pxText, textColor);
rtDefineProperty(pxText, pixelSize);
rtDefineProperty(pxText, fontUrl);
rtDefineProperty(pxText, font);
