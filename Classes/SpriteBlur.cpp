//
//  SpriteBlur.cpp
//  RadarDemo
//
//  Created by by.huang on 16/3/2.
//
//

#include "SpriteBlur.h"

USING_NS_CC;


SpriteBlur::SpriteBlur()
: fBlurRadius_(0.0f)
, fBlurSampleNum_(0.0f)
{
    
}

SpriteBlur::~SpriteBlur()
{
    
}

SpriteBlur* SpriteBlur::create(cocos2d::Sprite* pSprite, const float fRadius, const float fSampleNum)
{
    SpriteBlur* pRet = new (std::nothrow) SpriteBlur();
    if (nullptr == pRet)
        return nullptr;
    pRet->fBlurRadius_ = fRadius;
    pRet->fBlurSampleNum_ = fSampleNum;
    if (pRet->initWithSpriteFrame(pSprite->getSpriteFrame()))
        pRet->autorelease();
    else
        CC_SAFE_DELETE(pRet);
    return pRet;
}

bool SpriteBlur::initWithTexture(cocos2d::Texture2D* texture, const cocos2d::Rect& rect)
{
    if (Sprite::initWithTexture(texture, rect))
    {
#if CC_ENABLE_CACHE_TEXTURE_DATA
        auto listener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom* event){
            setGLProgram(nullptr);
            initGLProgram();
        });
        
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif
        initGLProgram();
        return true;
    }
    return false;
}

void SpriteBlur::initGLProgram()
{
    GLchar * fragSource = (GLchar*)cocos2d::__String::createWithContentsOfFile(
                                                                             cocos2d::FileUtils::getInstance()->fullPathForFilename("Sprite_blur.fsh").c_str())->getCString();
    auto program = cocos2d::GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource);
    
    auto glProgramState = cocos2d::GLProgramState::getOrCreateWithGLProgram(program);
    setGLProgramState(glProgramState);
    
    auto size = getTexture()->getContentSizeInPixels();
    getGLProgramState()->setUniformVec2("resolution", size);
    getGLProgramState()->setUniformFloat("blurRadius", fBlurRadius_);
    getGLProgramState()->setUniformFloat("sampleNum", fBlurSampleNum_);
}


void SpriteBlur::setBlurRadius(float radius)
{
    fBlurRadius_ = radius;
    getGLProgramState()->setUniformFloat("blurRadius", fBlurRadius_);
}

void SpriteBlur::setBlurSampleNum(float num)
{
    fBlurSampleNum_ = num;
    getGLProgramState()->setUniformFloat("sampleNum", fBlurSampleNum_);
}
