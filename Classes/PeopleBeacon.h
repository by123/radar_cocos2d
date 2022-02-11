//
//  PeopleBeacon.hpp
//  HellpWorldDemo
//
//  Created by by.huang on 16/2/17.
//
//

#ifndef PeopleBeacon_h
#define PeopleBeacon_h

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;

class PeopleBeacon : public cocos2d :: Sprite
{
public:
    //创建对象
    static PeopleBeacon *create(double degree,double radius,double x,double y,double time,std::string key);
    //出场动画
    virtual void enterAnim();
    //出场动画执行
    void enter();
    //缩放动画
    virtual void scaleAnim();
    //缩放动画执行
    virtual void scale(float t);
    //角度
    double degree;
    //半径
    double radius;
    //是否已经绘制
    double isShow;
protected:
    //坐标x
    double x;
    //坐标y
    double y;
    //动画时间
    double time;
    //标志
    std::string key;
    
};

#endif /* PeopleBeacon_h */

