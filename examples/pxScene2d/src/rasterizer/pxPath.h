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

#ifndef _PX_PATH_H
#define _PX_PATH_H

#ifdef _WINDOWS_
#define RTPLATFORM_WINDOWS
#endif


#include "pxCanvas2d.h"
#include "pxContext.h"
#include "pxResource.h"

#include <vector>

typedef union
{
  uint8_t bytes[4];
  float f;
}
floatBytes_t;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SVG_OP_MOVE           ((uint8_t) 'M') // MOVE
#define SVG_OP_LINE           ((uint8_t) 'L') // LINE
#define SVG_OP_ARC            ((uint8_t) 'A') // ARC
#define SVG_OP_Q_CURVE        ((uint8_t) 'Q') // QUADRATIC
#define SVG_OP_C_CURVE        ((uint8_t) 'C') // CUBIC
#define SVG_OP_CLOSE          ((uint8_t) 'Z') // CLOSE

#define SVG_OP_move           ((uint8_t) 'm') // MOVE       (relative)
#define SVG_OP_arc            ((uint8_t) 'a') // ARC        (relative)
#define SVG_OP_line           ((uint8_t) 'l') // LINE       (relative)
#define SVG_OP_q_curve        ((uint8_t) 'q') // QUADRATIC  (relative)
#define SVG_OP_c_curve        ((uint8_t) 'c') // CUBIC      (relative)

//#define SVG_LEN_MOVE          ((uint8_t) sizeof(float) * 2 ) // MOVE
//#define SVG_LEN_LINE          ((uint8_t) sizeof(float) * 2 ) // LINE
//#define SVG_LEN_Q_CURVE       ((uint8_t) sizeof(float) * 4 ) // QUADRATIC
//#define SVG_LEN_C_CURVE       ((uint8_t) sizeof(float) * 6 ) // CUBIC

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class pxPath: public pxObject
{
public:
  rtDeclareObject(pxPath, pxObject);
  
  rtProperty(d,                  path,        setPath, rtString);
  rtProperty(fillColor,     fillColor,   setFillColor, uint32_t);
  rtProperty(strokeColor, strokeColor, setStrokeColor, uint32_t);
  rtProperty(strokeWidth, strokeWidth, setStrokeWidth, float);

  
public:
  pxPath(pxScene2d* scene);
  ~pxPath();
  
  virtual void draw();
  virtual void onInit();
  virtual void sendPromise();
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  virtual rtError setPath(const rtString d);
  virtual rtError path(rtString& v) const { v = mPath; return RT_OK; };

  
  rtError fillColor(uint32_t& v) const { v = (uint32_t)mFillColor.u; return RT_OK; };
  virtual rtError setFillColor(const uint32_t c);
  
  rtError strokeColor(uint32_t& v) const { v = (uint32_t)mStrokeColor.u; return RT_OK; };
  virtual rtError setStrokeColor(const uint32_t c);
  
  rtError strokeWidth(float& v) const { v = (float)mStrokeWidth;    return RT_OK; };
  virtual rtError setStrokeWidth(const float w);

  // One STATIC parser
  static rtError parsePath(const char *d, pxPath *p /*= NULL*/ );
  
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  
  void pushOpcode(uint8_t op);

  void pushFloat(float f);
  void pushFloat(float a, float b);
  void pushFloat(float a, float b, float c, float d);
  void pushFloat(float a, float b, float c, float d, float e, float f);
  
  float   getFloatAt(int i);
  float   getFloatAt(const uint8_t *p);
  uint8_t getByteAt(const uint8_t *p);
  
  uint8_t *getStream() { return (opStream.size() > 0) ? (uint8_t *) &opStream[0] : NULL; };
  int     getLength()  { return (int) opStream.size(); };
 
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  pxColor      mStrokeColor;
  float        mStrokeWidth;
  
  pxColor      mFillColor;
  rtString     mPath;
  
private:
  
  std::vector<uint8_t>  opStream;
};

#endif //_PX_PATH_H
