//
//  CCCircleBy.hpp
//  HellpWorldDemo
//
//  Created by by.huang on 16/2/2.
//
//

#ifndef CCCircleBy_h
#define CCCircleBy_h
#include <iostream>
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;


class CCCircleBy : public ActionInterval
{
public:
    
    static CCCircleBy *create(float t,Point circleCenter,float radius,float startRadian,float endRadian);
    
    bool init(float t,Point circleCenter,float radius,float startRadian,float endRadian);
    
    virtual void update(float time);
    
    void startWithTarget(cocos2d::Node *target);
    
protected:
    Point m_circleCenter; //圆心坐标
    float m_radius;       //半径
    float m_startRadian;  //开始弧度
    float m_endRadian;    //结束弧度
    float m_radian;       //弧度
    float m_moveTimes;    //刷新次数
    Node  *m_target;

};
#endif

