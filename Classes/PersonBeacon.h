//
//  PersonBeacon.hpp
//  RadarDemo
//
//  Created by by.huang on 16/2/25.
//
//

#ifndef PersonBeacon_hpp
#define PersonBeacon_hpp

#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;

class PersonBeacon : public cocos2d::Sprite
{
    public :
    
    
        //纬度
        double lat;
        //经度
        double lon;
        //实际距离
        double distance;
        //女0,男1
        int gender;
        //是否显示在雷达盘内
        bool positionInRadar;
        //是否退场
        bool hasExit;
        //是否退场完毕
        bool hasExitCompelete;
    
        //创建人信标
        static PersonBeacon* create(double lat,double lon,int gender);
        //绘制人信标在雷达的位置
        virtual void computePosition(double largestDistance,double smallestDistance);
        //计算人信标与我的距离
        static double measureDistance(double lat,double lon);
        //计算人信标角度
        static double measureDegree(double lat,double lon);
    
    
        //出场动画
        virtual void enterAnim(double x,double y);
        //离场动画
        virtual void exitAnim();
        //跳动动画
        virtual void scaleAnim();
        virtual void scale(float t);
        //缩进动画
        virtual void moveAnim(double total,double moveDistance);
    
        //雷达盘上的x
        double x;
        //雷达盘上的y
        double y;
        //雷达盘上的角度
        double degree;
        //雷达盘上的半径
        double radius;
    
};

#endif
