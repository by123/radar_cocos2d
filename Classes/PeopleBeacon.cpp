//
//  PeopleBeacon.cpp
//  HellpWorldDemo
//
//  Created by by.huang on 16/2/17.
//
//

#include "PeopleBeacon.h"
#include "CCCircleBy.h"
#define ScreenWidth 640
#define ScreenHeight 1136
#define ScanTime 4
#define BeaconR 42

PeopleBeacon* PeopleBeacon::create(double degree, double radius, double x, double y, double time, std::string key)
{
    PeopleBeacon *beacon = new PeopleBeacon();
    if((int)degree %2 == 0)
    {
        beacon->initWithFile("redpoint.png", Rect(0, 0, BeaconR, BeaconR));
    }
    else
    {
        beacon->initWithFile("bluepoint.png", Rect(0, 0, BeaconR, BeaconR));
    }
    if (beacon)
    {
        beacon->degree = degree;
        beacon->radius = radius;
        beacon->x = x;
        beacon->y = y;
        beacon->time = time;
        beacon->key = key;
        beacon->autorelease();
        beacon->setVisible(false);
        return beacon;
    }
    CC_SAFE_DELETE(beacon);
    return NULL;
}

void PeopleBeacon::enterAnim()
{
    this->scheduleOnce(CC_CALLBACK_0(PeopleBeacon::enter,this), time/2, key);
//    this->schedule(CC_CALLBACK_1(PeopleBeacon::start,this), ScanTime, 0, 0.03, key);
}

void PeopleBeacon::enter()
{
    this->setVisible(true);
    /**第一种方案**/
    if(degree < 180)
    {
        this ->setPosition(0, 0);
        double x = ScreenWidth /2 ;
        double y = ScreenHeight / 2;
        double r = ScreenWidth * 15 / 16/2;
        double R = sqrt(x * x + y *y );
        double R1 = sqrt(R * R - r * r);
        
        
        double degree1 = MATH_RAD_TO_DEG(asin(r / R));
        double degree2 = MATH_RAD_TO_DEG(asin(y / R));
        double temp = degree2 - degree1;
        double new_x = cos(MATH_DEG_TO_RAD(temp)) * R1;
        double new_y = sin(MATH_DEG_TO_RAD(temp)) * R1;
        
        auto *moveAction = MoveTo :: create(0.3, Vec2(new_x, new_y));
        
        double startX = ScreenWidth / 32;
        double startDistance = sqrt( (startX - new_x) * (startX - new_x) + (y - new_y) * (y - new_y));
        double startDeg = 2 * MATH_RAD_TO_DEG (asin( startDistance / 2 / r)) + 90;
        
        log("startDeg->%f",startDeg);
        double endDistance =sqrt( (x - new_x) * (x - new_x) + (y - new_y) * (y - new_y));
        double endDeg = 2 * MATH_RAD_TO_DEG (asin( endDistance / 2 / r));
        log("endDeg1->%f",endDeg);

        endDeg = degree + startDeg;
        log("endDeg2->%f",endDeg);

        double endRadian = MATH_DEG_TO_RAD(-endDeg);
        
        ActionInterval* circleBy = CCCircleBy :: create(3, Point(x,y), r,MATH_DEG_TO_RAD(-startDeg),endRadian);
        auto action  = Sequence :: create(moveAction,circleBy, NULL);
        this->runAction(action);
    }
    else
    {
        this ->setPosition(ScreenWidth, ScreenHeight);
        double x = ScreenWidth /2 ;
        double y = ScreenHeight / 2;
        double r = ScreenWidth * 15 / 16/2;
        double R = sqrt(x * x + y *y );
        double R1 = sqrt(R * R - r * r);
        
        
        double degree1 = MATH_RAD_TO_DEG(asin(r / R));
        double degree2 = MATH_RAD_TO_DEG(asin(y / R));
        double temp = degree2 - degree1;
        double new_x = ScreenWidth -  cos(MATH_DEG_TO_RAD(temp)) * R1;
        double new_y = ScreenHeight -  sin(MATH_DEG_TO_RAD(temp)) * R1;
        
        auto *moveAction = MoveTo :: create(0.3, Vec2(new_x, new_y));
        
        double startX = ScreenWidth / 32 + r*2;
        double startDistance = sqrt( (startX - new_x) * (startX - new_x) + (y - new_y) * (y - new_y));
        double startDeg = 2 * MATH_RAD_TO_DEG (asin( startDistance / 2 / r))-90 ;
        
        double endDistance =sqrt( (x - new_x) * (x - new_x) + (y - new_y) * (y - new_y));
        double endDeg = 2 * MATH_RAD_TO_DEG (asin( endDistance / 2 / r));
        endDeg = degree + startDeg -180;
        
        double endRadian = MATH_DEG_TO_RAD(-endDeg);
        
        ActionInterval* circleBy = CCCircleBy :: create(3, Point(x,y), r,MATH_DEG_TO_RAD(-startDeg),endRadian);
        auto action  = Sequence :: create(moveAction,circleBy, NULL);
        this->runAction(action);
    }
    
    /**第二种方案**/
//    ccBezierConfig config;
//    config.controlPoint_2 = Point(-ScreenWidth * 15/32, ScreenHeight/2 - ScreenWidth *15/32);
//    config.controlPoint_1 = Point(-ScreenWidth * 15/32 ,ScreenHeight + ScreenWidth *15/32);
//
//    log("x->%f",this->x);
//    log("y->%f",this->y);
//
//    config.endPosition = Point(this->x, this->y);
//    auto action = BezierBy :: create(3, config);

}

void PeopleBeacon::scale(float t)
{
    auto scaleAction =  ScaleBy :: create(0.2, 1.3);
    auto scaleActionSequece = Sequence :: create(scaleAction,scaleAction->reverse(), NULL);
    this->runAction(scaleActionSequece);
}

void PeopleBeacon::scaleAnim()
{
    this->schedule(schedule_selector(PeopleBeacon::scale), ScanTime, GID_MAX, time+0.5);
}


