//
//  PersonBeacon.cpp
//  RadarDemo
//
//  Created by by.huang on 16/2/25.
//
//

#include "PersonBeacon.h"
#include <cocos/cocos2d.h>
#include "CCCircleBy.h"

#define BeaconWidth 42
#define EARTH_RADIUS  6378.137
#define CENTER_WIDTH 160
#define ScanTime 4


static double Radar_R =  ScreenWidth *15/32;

PersonBeacon* PersonBeacon::create(double lat,double lon,int gender)
{
    PersonBeacon *beacon = new PersonBeacon();
    if(gender == 0){
        beacon->initWithFile("redpoint.png",Rect(0, 0, BeaconWidth, BeaconWidth));
    }
    else{
        beacon->initWithFile("bluepoint.png",Rect(0, 0, BeaconWidth, BeaconWidth));
    }
    
    if (beacon){
        beacon->lat = lat;
        beacon->lon = lon;
        beacon->distance = PersonBeacon :: measureDistance(lat, lon);
        beacon->degree = PersonBeacon :: measureDegree(lat, lon);
        beacon->gender = gender;
        int tag = lat * 100000;
        beacon->setTag(tag);
        beacon->autorelease();
        return beacon;
    }
    CC_SAFE_DELETE(beacon);
    return NULL;
}


void PersonBeacon :: computePosition(double largestDistance,double smallestDistance)
{
    double xValue = 0.0;
    double yValue = 0.0;
    double bValue =  this->distance * Radar_R / largestDistance;

    if (bValue < CENTER_WIDTH / 2 ) {//圆心到信标距离小于内圆半径
        bValue = CENTER_WIDTH / 2 ;
    }
    xValue = ScreenWidth/2 + cos(MATH_DEG_TO_RAD(this->degree)) * bValue;
    yValue = ScreenHeight/2 + sin(MATH_DEG_TO_RAD(this->degree)) * bValue;

    this->radius = bValue;
//    log("x->%f",xValue);
//    log("y->%f",yValue);
    
    this->setPosition(Vec2(xValue, yValue));
}


double PersonBeacon:: measureDistance(double lat, double lon)
{
    double a = (iLat - lat) * M_PI / 180.0;
    double b = (iLon - lon) * M_PI / 180.0;
    double s = 2 * asin(sqrt(pow(sin(a/2),2) + cos(iLat * M_PI / 180.0 )*cos(lat * M_PI / 180.0 )*pow(sin(b/2),2)));
    s = s * EARTH_RADIUS;
    s = round(s * 10000) / 10000 * 1000;

    return s;
}

double PersonBeacon:: measureDegree(double lat,double lon)
{
    double lat1 = MATH_DEG_TO_RAD(iLat);
    double lon1 = MATH_DEG_TO_RAD(iLon);
    double lat2 = MATH_DEG_TO_RAD(lat);
    double lon2 = MATH_DEG_TO_RAD(lon);
    double dLon = (lon2 - lon1);
    
    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    double degree = MATH_RAD_TO_DEG(atan2(y, x));
    degree = (degree + 360);
    degree = (degree > 360) ? (degree - 360) : degree;
    
    while (degree > 360) {
        degree = degree - 360;
        if (degree <= 360)
            break;
    }
//    log("角度->%f",degree);

    return degree;
}

void PersonBeacon::scaleAnim()
{
    auto time = ScanTime * (360 -this->degree) / 360;
    this->schedule(schedule_selector(PersonBeacon::scale), ScanTime, GID_MAX, time);
}

void PersonBeacon::scale(float t)
{
    auto *scaleAction = ScaleBy :: create(0.2, 1.4);
    this->runAction(Sequence :: create(scaleAction,scaleAction->reverse(), NULL));
}


void PersonBeacon::moveAnim(double total,double moveDistance)
{
    double perValue = total / (Radar_R - CENTER_WIDTH/2);
    double bValue = this->radius + moveDistance / perValue;

    if(bValue > Radar_R)
    {
        this->positionInRadar = false;
    }
    else
    {
        this->positionInRadar = true;
    }
    
    if(this->positionInRadar)
    {
        this->hasExit = false;
        
        double xValue = ScreenWidth/2 + cos(MATH_DEG_TO_RAD(this->degree)) * bValue;
        double yValue = ScreenHeight/2 + sin(MATH_DEG_TO_RAD(this->degree)) * bValue;

        auto temp = Spawn :: create(FadeIn :: create(1), NULL);
        auto action = EaseQuarticActionOut :: create(temp);
        this->runAction(action);
        
        this->setPosition(Vec2(xValue, yValue));
       
    }
    else
    {
        if(!this->hasExit)
        {
            this->hasExit = true;
//            EventCustom event("radar");
//            event.setUserData(this);
//            Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);

            double xValue = ScreenWidth/2 + cos(MATH_DEG_TO_RAD(this->degree)) * 1000;
            double yValue = ScreenHeight/2 + sin(MATH_DEG_TO_RAD(this->degree)) * 1000;
            auto temp = Spawn :: create(FadeOut :: create(1),MoveTo :: create(2, Vec2(xValue, yValue)), NULL);
            auto action = EaseQuarticActionOut :: create(temp);
            this->runAction(action);
        }
    }
    
//    if(bValue > Radar_R)
//    {
//        this->positionInRadar = false;
//    }
//    else
//    {
//        this->positionInRadar = true;
//        if(this->hasExitCompelete)
//        {
//            this->hasExit = false;
//            this->hasExitCompelete = false;
//        }
//    }
//    
//    if(this->positionInRadar)
//    {
//        double xValue = ScreenWidth/2 + cos(MATH_DEG_TO_RAD(this->degree)) * bValue;
//        double yValue = ScreenHeight/2 + sin(MATH_DEG_TO_RAD(this->degree)) * bValue;
//        this->setPosition(Vec2(xValue, yValue));
//    }
//    else
//    {
//        if(!this->hasExit && !this->hasExitCompelete)
//        {
//            this->hasExit = true;
//            PersonBeacon :: exitAnim();
//        }
//    }
}

void PersonBeacon::enterAnim(double x,double y)
{
    this ->setPosition(0, 0);
    ccBezierConfig config;
    config.controlPoint_1 = Point(0,0);
    config.controlPoint_2 = Point(x+50,y+150);
    config.endPosition = Point(x,y);
    
    log("%f",x);
    auto action = BezierTo :: create(3, config);
    this->runAction(action);

}

void PersonBeacon::exitAnim()
{
//    double xValue = ScreenWidth/2 + cos(MATH_DEG_TO_RAD(this->degree)) * 1000;
//    double yValue = ScreenHeight/2 + sin(MATH_DEG_TO_RAD(this->degree)) * 1000;
//    auto moveAction = MoveTo :: create(1, Vec2(xValue,yValue));
//    auto speed = EaseBackOut :: create(moveAction);
//    auto callBack = CallFuncN :: create([&](Ref *pSender){
//        this->hasExitCompelete = true;
//    });
//    this->runAction(Sequence :: create(speed,callBack,NULL));
}



