//
//  SpriteBlur.h
//  RadarDemo
//
//  Created by by.huang on 16/3/2.
//
//

#ifndef SpriteBlur_h
#define SpriteBlur_h

#include "cocos2d.h"

USING_NS_CC;

//////////////////////////////////////////////////////////////////////////
//精灵模糊类
//////////////////////////////////////////////////////////////////////////
class SpriteBlur : public cocos2d::Sprite
{
public:
    SpriteBlur();
    ~SpriteBlur();
    static SpriteBlur* create(cocos2d::Sprite* pSprite, const float fRadius = 8.0f, const float fSampleNum = 8.0f);
    bool initWithTexture(cocos2d::Texture2D* pTexture, const cocos2d::Rect&  rRect);
    void initGLProgram();
    
    void setBlurRadius(float fRadius);
    void setBlurSampleNum(float fSampleNum);
    
protected:
    float fBlurRadius_;
    float fBlurSampleNum_;
};
#endif
