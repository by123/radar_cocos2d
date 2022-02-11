//
//  CCCircleBy.cpp
//  HellpWorldDemo
//
//  Created by by.huang on 16/2/2.
//
//

#include "CCCircleBy.h"

void CCCircleBy :: startWithTarget(cocos2d::Node *target)
{
    m_target = target;
    ActionInterval :: startWithTarget(m_target);
}

CCCircleBy* CCCircleBy :: create(float t, Point circleCenter, float radius,float startRadian,float endRadian)
{
 
    CCCircleBy *pCircle = new CCCircleBy();
    pCircle->init(t, circleCenter, radius,startRadian,endRadian);
    pCircle->autorelease();
    return pCircle;
}

bool CCCircleBy::init(float t, Point circleCenter, float radius,float startRadian,float endRadian)
{
    if(ActionInterval :: initWithDuration(t))
    {
        m_circleCenter = circleCenter;
        m_radius = radius;
        m_radian = -2.0 *M_PI / (t/(Director :: getInstance() -> getAnimationInterval()));
        m_moveTimes = 1;
        m_startRadian = startRadian;
        m_endRadian = endRadian;
        return true;
    }
    return false;
}

void CCCircleBy::update(float time)
{
//    log("%f",m_moveTimes);
    float radian =  m_startRadian +m_radian * m_moveTimes;
    if(radian < m_endRadian)
    {
        return;
    }
    float x = m_radius *sin(radian);
    float y = m_radius *cos(radian);
    Point newPos = Point(x+m_circleCenter.x,y+m_circleCenter.y);
    m_target->setPosition(newPos);
    m_moveTimes +=1;
    
}





