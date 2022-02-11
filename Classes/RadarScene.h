#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"


USING_NS_CC;

class RadarScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
        
    static cocos2d::RenderTexture* ScreenShot(bool bIsSave);
    
    static cocos2d::RenderTexture* SpriteBlurer(cocos2d::Sprite* pSprite, float fRadius, float fSampleNum);

    void test(float t);

    virtual bool init();
    
    //初始化精灵
    virtual void initView();

    //初始化数据
    virtual void initData();
    
    //雷达盘拖动事件
    virtual void moveRadarSprite(cocos2d::Ref *pSender);
    
    //滑动条拖动事件
    virtual void movePointSprite(cocos2d::Ref *pSender);
    
    //雷达盘的方向传感器
    virtual void initMotion();
    
    //更新方向传感器
    virtual void updateMotion(float t);
    
    //手指接触滑块
    virtual void beginMoveLabel();
    
    //设置滑块滑动时的显示
    virtual void updateMoveLabel(double x,double y);
    
    //手指离开滑块
    virtual void finishMoveLabel();
    

    
    CREATE_FUNC(RadarScene);
};

#endif
