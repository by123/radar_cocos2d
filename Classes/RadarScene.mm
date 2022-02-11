#include "RadarScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "PersonBeacon.h"
#include <CoreMotion/CoreMotion.h>
#include <CoreFoundation/CoreFoundation.h>
#include "SpriteBlur.h"

#define ScanTime 4
#define Left 120
#define Bottom 100
#define Tag_RadarViewSprte 1001
#define Tag_RadarSprte 1002
#define Tag_Label 1003
#define Tag_BgWideSprite 1004
#define Tag_CenterSprite 1005
#define Tag_WhiteLine 1006
#define Tag_MaskNode 1007
#define Tag_MaskLabel 1008


USING_NS_CC;

using namespace cocostudio::timeline;

//雷达盘初始位置
static cocos2d::Point originalRadarP;

//雷达盘滑动的位置
static cocos2d::Point newRadarP;

//滑动点初始位置
static cocos2d::Point originalPointP;

//滑动点滑动的位置
static cocos2d::Point newPointP;

//总数据源
static cocos2d :: Vector<PersonBeacon *>datas = Vector<PersonBeacon *>(MAXFLOAT);

//最大距离
static double LargestDistance = 0;
//最小距离
static double SmallestDistance = 9999999;

//全局事件分发器
auto dispatcher = Director::getInstance()->getEventDispatcher();

CMMotionManager *motionManager;

Scene* RadarScene::createScene()
{
    auto scene = Scene::create();
    
    auto layer = RadarScene::create();
    
    scene->addChild(layer);

    return scene;
}

bool RadarScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    RadarScene :: initView();
    
    return true;
}


void RadarScene::initView()
{

    //背景图
    Sprite *bgSprite = Sprite :: create("bg.png");
    bgSprite->setPosition(Vec2(ScreenWidth/2,ScreenHeight/2));
    this -> addChild(bgSprite);
    
    this->scheduleOnce(schedule_selector(RadarScene :: test), 0.1);
    

}

void RadarScene::test(float t)
{
    auto textureScreen = RadarScene :: ScreenShot(false);
    auto textureBlur = SpriteBlurer(textureScreen->getSprite(),10,10);
    //将模糊化后的图片保存成一张图片
    auto spriteBlur = Sprite::createWithSpriteFrame(textureBlur->getSprite()->getSpriteFrame());
    spriteBlur->setPosition(Vec2(ScreenWidth/2, ScreenHeight/2));
    spriteBlur->setRotationSkewX(180.0f);
    spriteBlur->setPosition(Vec2(ScreenWidth / 2, ScreenHeight/ 2));
    
    //将此精灵覆盖在原有层之上，成为一个临时背景
    this->addChild(spriteBlur);
    
    
    //雨雪特效
    ParticleSystem *rainParticle = ParticleRain :: create();
    rainParticle->setPosition(ScreenWidth/2, ScreenHeight);
    this->addChild(rainParticle);
    
    ParticleSystem *snowParticle = ParticleSnow :: create();
    snowParticle->setPosition(ScreenWidth/2, ScreenHeight);
    snowParticle->setSpeed(rainParticle->getSpeed());
    this->addChild(snowParticle);
    
    /**************雷达组件**************/
    
    auto radarViewSprite = Sprite :: create();
    radarViewSprite->setContentSize(cocos2d::Size(ScreenWidth, ScreenHeight));
    radarViewSprite->setPosition(ScreenWidth/2, ScreenHeight/2);
    radarViewSprite->setTag(Tag_RadarViewSprte);
    this -> addChild(radarViewSprite);
    RadarScene :: moveRadarSprite(radarViewSprite);
    
    //雷达盘
    auto radarSprite = Sprite::create("radar.png");
    radarSprite->setPosition(ScreenWidth/2, ScreenHeight/2);
    radarSprite->setTag(Tag_RadarSprte);
    radarViewSprite->addChild(radarSprite);
    
    //雷达外环
    auto bgWideSprite = Sprite::create("radar_wide.png");
    bgWideSprite->setPosition(ScreenWidth/2, ScreenHeight/2);
    bgWideSprite->setTag(Tag_BgWideSprite);
    radarViewSprite->addChild(bgWideSprite);
    
    //中间按钮
    auto centerSprite = Sprite::create("more.png");
    centerSprite->setPosition(ScreenWidth/2, ScreenHeight/2);
    centerSprite->setTag(Tag_CenterSprite);
    radarViewSprite->addChild(centerSprite);
    
    //扫描图层
    auto scanSprite = Sprite::create("scan.png");
    scanSprite->setPosition(ScreenWidth/2, ScreenHeight/2);
    radarViewSprite->addChild(scanSprite);
    
    auto rotateAction =  RotateBy :: create(ScanTime, 360);
    auto action = RepeatForever::create(rotateAction);
    scanSprite->runAction(action);
    
    /**************滑动条组建**************/
    
    //滑动条
    auto alphaWhiteLine = cocos2d::DrawNode :: create();
    this -> addChild(alphaWhiteLine);
    alphaWhiteLine -> drawSegment(Vec2(Left, Bottom), Vec2(ScreenWidth-Left,Bottom), 2, Color4F(1, 1, 1, 0.3f));
    
    auto whiteLine = cocos2d::DrawNode :: create();
    whiteLine->setTag(Tag_WhiteLine);
    this -> addChild(whiteLine);
    
    //缩小精灵
    auto smallerSprite = Sprite :: create("smaller_normal.png");
    smallerSprite->setPosition(Left-40, Bottom);
    this->addChild(smallerSprite);
    
    //放大精灵
    auto biggerSprite = Sprite :: create("bigger_normal.png");
    biggerSprite->setPosition(ScreenWidth-Left + 40, Bottom);
    this->addChild(biggerSprite);
    
    
    //滑动精灵
    Sprite *pointSprite = Sprite :: create("point.png");
    pointSprite->setPosition(ScreenWidth/2, Bottom);
    this->addChild(pointSprite);
    originalPointP = pointSprite->getPosition();
    
    RadarScene :: movePointSprite(pointSprite);
    
    //文字
    Label *label = Label :: create();
    label->setTextColor(Color4B :: WHITE);
    label->setPosition(ScreenWidth/2, Bottom + 40);
    label->setString("在dasdasdad");
    label->setSystemFontSize(26);
    label->setTag(Tag_Label);
    this->addChild(label);
    
    //蒙板
    auto maskNode = cocos2d::DrawNode :: create();
    maskNode->setTag(Tag_MaskNode);
    this->addChild(maskNode);
    maskNode->setVisible(false);
    
    //蒙板上的文字
    Label *maskLabel = Label :: create();
    maskLabel->setTextColor(Color4B :: WHITE);
    maskLabel->setSystemFontSize(50);
    maskLabel->setTag(Tag_MaskLabel);
    this->addChild(maskLabel);
    maskLabel->setVisible(false);
    
    
    //初始化数据
    RadarScene :: initData();
    
    //初始化方向传感器
    RadarScene :: initMotion();
    
    //初始化文字显示
    RadarScene :: updateMoveLabel(ScreenWidth/2,0);

}


void RadarScene::initData()
{

    struct timeval now;
    gettimeofday(&now, NULL);
    unsigned rand_seed = (unsigned)(now.tv_sec*1000 + now.tv_usec/1000);
    srand(rand_seed);
    for(int i=0;i<100;i++)
    {
        int randomNum = rand()%4;
        int randowmSex = rand()%2;

        double lat = 0;
        double lon = 0;
        switch (randomNum) {
            case 0:
                lat = iLat + random(0.001, 0.01);
                lon = iLon + random(0.001, 0.01);
                break;
            case 1:
                lat = iLat + random(0.001, 0.01);
                lon = iLon - random(0.001, 0.01);
                break;
            case 2:
                lat = iLat - random(0.001, 0.01);
                lon = iLon + random(0.001, 0.01);
                break;
            case 3:
                lat = iLat - random(0.001, 0.01);
                lon = iLon - random(0.001, 0.01);
                break;
            default:
                break;
        }
        PersonBeacon *beacon = PersonBeacon :: create(lat, lon, randowmSex%2);
        auto radarViewSprite = (Sprite *)this->getChildByTag(Tag_RadarViewSprte);
        radarViewSprite->addChild(beacon);
        datas.pushBack(beacon);
    }
    
    
    for(PersonBeacon *beacon : datas)
    {
    
        if(LargestDistance < beacon->distance)
        {
            LargestDistance = beacon->distance;
        }
        if(SmallestDistance > beacon->distance)
        {
            SmallestDistance = beacon->distance;
        }
    }
 
    log("最大%f",LargestDistance);
    log("最小%f",SmallestDistance);


//    EventListenerCustom *listener = EventListenerCustom :: create("radar", [=](EventCustom *event)
//                                                  {
//                                                      log("%ld",datas.size());
//                                                      datas.eraseObject((PersonBeacon *)event->getUserData());
//                                                  });
//    dispatcher->addEventListenerWithFixedPriority(listener, 1111);

    for(PersonBeacon *beacon : datas)
    {
        beacon->computePosition(LargestDistance,SmallestDistance);
        beacon->scaleAnim();
        double total = ScreenWidth - 2 * Left;
        beacon->moveAnim(total, total/2);


    }
 
}

void RadarScene::moveRadarSprite(cocos2d::Ref *pSender)
{
    Sprite *radarViewSprite = (Sprite *)pSender;
    auto radarListener = EventListenerTouchOneByOne :: create();
    radarListener->setSwallowTouches(true);
    originalRadarP = radarViewSprite->getPosition();
    radarListener->onTouchBegan = [=](Touch *touch,Event *event)
    {
        auto s = radarViewSprite->getContentSize();
        bool b = radarViewSprite->getBoundingBox().containsPoint(touch->getLocation());
        if ( !b ) return false;
        newRadarP = touch->getLocation();
        return true;
    };
    radarListener->onTouchMoved = [=](Touch* touch,Event* event)
    {
        auto touchPoint = touch->getLocation();
        auto x = radarViewSprite->getPosition().x;
        auto y = radarViewSprite->getPosition().y+touchPoint.y-newRadarP.y;
        radarViewSprite->setPosition(x,y);
        newRadarP = touchPoint;
    };
    
    radarListener->onTouchEnded = [=](Touch* touch,Event* event)
    {
        newRadarP = originalRadarP;
        auto moveAction =  MoveTo :: create(0.5, originalRadarP);
        auto moveSpeed= EaseExponentialOut::create(moveAction);
        radarViewSprite->runAction(moveSpeed);
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(radarListener,radarViewSprite);
    
}

void RadarScene::movePointSprite(cocos2d::Ref *pSender)
{
    
    Sprite *pointSprite = (Sprite *)pSender;
    auto poinitListener = EventListenerTouchOneByOne::create();
    poinitListener->setSwallowTouches(true);
    poinitListener->onTouchBegan = [=](Touch* touch,Event* event)
    {
        auto s = pointSprite->getContentSize();
        bool b = pointSprite->getBoundingBox().containsPoint(touch->getLocation());
        if ( !b ) return false;
        newPointP= touch->getLocation();
        
        RadarScene :: beginMoveLabel();
        return true;
    };
    poinitListener->onTouchMoved = [=](Touch* touch,Event* event)
    {
        auto touchPoint = touch->getLocation();
        auto x = pointSprite->getPosition().x+touchPoint.x-newPointP.x;
        auto y = pointSprite->getPosition().y+touchPoint.y-newPointP.y;
        if(x < Left)
        {
            x = Left;
        }
        if(x > ScreenWidth - Left)
        {
            x = ScreenWidth - Left;
        }
        pointSprite->setPosition(x,y);
        newPointP = touchPoint;
        
        double total = ScreenWidth - 2 * Left;
        for(PersonBeacon *beacon : datas)
        {
            double moveDistance = total - (x-Left);
            beacon->moveAnim(total,moveDistance);
        }
        
        RadarScene :: updateMoveLabel(x,y);
    };
    
    poinitListener->onTouchEnded = [=](Touch* touch,Event* event)
    {
        newPointP.y = originalPointP.y;
        if(newPointP.x < Left)
        {
            newPointP.x = Left;
        }
        if(newPointP.x > ScreenWidth - Left)
        {
            newPointP.x = ScreenWidth - Left;
        }
        auto moveAction =  MoveTo :: create(0.5, newPointP);
        auto moveSpeed= EaseExponentialOut::create(moveAction);
        pointSprite->runAction(moveSpeed);
        
        RadarScene :: finishMoveLabel();
        
    };
    
    dispatcher->addEventListenerWithSceneGraphPriority(poinitListener,pointSprite);

}


void RadarScene :: initMotion()
{

    motionManager = [[CMMotionManager alloc] init];
    motionManager.deviceMotionUpdateInterval =1.0/60.0;
    if (motionManager.isDeviceMotionAvailable) {
        [motionManager startDeviceMotionUpdates];
    }
    this->schedule(schedule_selector(RadarScene :: updateMotion), 0.1, GID_MAX, 0.1);
}


void RadarScene::updateMotion(float t)
{
    
    CMDeviceMotion *currentDeviceMotion = motionManager.deviceMotion;
    CMAttitude *currentAttitude = currentDeviceMotion.attitude;
    
    float deg = roundf(MATH_RAD_TO_DEG(currentAttitude.yaw));
    
    auto radarViewSprite = this->getChildByTag(Tag_RadarViewSprte);
    radarViewSprite->runAction(RotateTo :: create(0.3, deg));
    
    auto bgWideSprite = radarViewSprite->getChildByTag(Tag_BgWideSprite);
    bgWideSprite->runAction(RotateTo :: create(0.3, -deg));
    
    auto centerSprite = radarViewSprite->getChildByTag(Tag_CenterSprite);
    centerSprite->runAction(RotateTo :: create(0.3, -deg));

}


void RadarScene::beginMoveLabel()
{
    auto maskNode = this->getChildByTag(Tag_MaskNode);
    auto maskLabel = this->getChildByTag(Tag_MaskLabel);

    maskNode->setVisible(true);
    maskLabel->setVisible(true);
}

void RadarScene::updateMoveLabel(double x,double y)
{
    
    double total = ScreenWidth - 2 * Left;
    double perValue =  (LargestDistance - SmallestDistance) /total;
    double distance = SmallestDistance + perValue * (x-Left);
    Label *label = (Label *)this->getChildByTag(Tag_Label);
    int count =0;
    for(PersonBeacon *beacon : datas)
    {
        if(beacon->positionInRadar)
        {
            count++;
        }
    }

    
    DrawNode *whiteLine =(DrawNode *) this->getChildByTag(Tag_WhiteLine);
    whiteLine -> clear();
    whiteLine -> drawSegment(Vec2(Left, Bottom), Vec2(x,Bottom), 2, Color4F::WHITE);
    
    DrawNode *maskNode = (DrawNode *)this->getChildByTag(Tag_MaskNode);
    maskNode -> clear();
    Vec2 vec[4];
    vec[0]=Vec2(x, 0);
    vec[1]=Vec2(ScreenWidth,0);
    vec[2]=Vec2(ScreenWidth, ScreenHeight);
    vec[3]=Vec2(x, ScreenHeight);
    maskNode -> drawPolygon(vec, 4, Color4F(0, 0, 0, 0.4f), 0, Color4F(0,0,0,0));
    
    Label *maskLabel = (Label *)this->getChildByTag(Tag_MaskLabel);
    if(y > ScreenHeight - 300)
    {
        y = ScreenHeight - 300;
    }
    if(x > ScreenWidth - 2 * Left)
    {
        maskLabel -> setPosition(Vec2(x - 80, y+200));
    }
    else
    {
        maskLabel -> setPosition(Vec2(x + 80, y+200));
    }
    
    if(distance < 1000)
    {
        label->setString(StringUtils::format("在%.fm内发现%d人",distance,count));
        maskLabel -> setString(StringUtils::format("%.fm\n %d人",distance,count));
    }
    else
    {
        distance = distance / 1000;
        label->setString(StringUtils::format("在%.1fkm内发现%d人",distance,count));
        maskLabel -> setString(StringUtils::format("%.1fkm\n %d人",distance,count));
    }

}

void RadarScene:: finishMoveLabel()
{
    auto maskNode = this->getChildByTag(Tag_MaskNode);
    auto maskLabel = this->getChildByTag(Tag_MaskLabel);

    maskNode->setVisible(false);
    maskLabel->setVisible(false);
}





//////////////////////////////////////////////////////////////////////////
//精灵模糊化函数
//////////////////////////////////////////////////////////////////////////
cocos2d::RenderTexture* RadarScene::SpriteBlurer(cocos2d::Sprite* pSprite,  float fRadius ,  float fSampleNum)
{
    //模糊化的临时精灵
    auto pSptBlur = SpriteBlur::create(pSprite, fRadius, fSampleNum);
    pSptBlur->setRotationSkewX(180.0f);
    pSptBlur->setPositionX(pSptBlur->getContentSize().width / 2);
    pSptBlur->setPositionY(pSptBlur->getContentSize().height / 2);
    //使用精灵尺寸初始化一个空的渲染纹理对象
    cocos2d::RenderTexture* textureScreen =
    cocos2d::RenderTexture::create(pSptBlur->getContentSize().width, pSptBlur->getContentSize().height);
    //开始获取
    textureScreen->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
    //遍历节点对象，填充纹理到texure中
    pSptBlur->visit();
    //结束获取
    textureScreen->end();
    return textureScreen;
}

//////////////////////////////////////////////////////////////////////////
//截屏函数
//////////////////////////////////////////////////////////////////////////


RenderTexture* RadarScene::ScreenShot(bool bIsSave)
{
    cocos2d ::Size size = Director::getInstance()->getWinSize();
    //使用屏幕尺寸初始化一个空的渲染纹理对象
    cocos2d::RenderTexture* textureScreen =
    cocos2d::RenderTexture::create(size.width, size.height);
    //清除数据并开始获取
    textureScreen->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
    //遍历场景节点对象，填充纹理到texure中
    cocos2d::Director::getInstance()->getRunningScene()->visit();
    //结束获取
    textureScreen->end();
    //保存为PNG图
    if (bIsSave)
    {
        static int s_iSerialNumber = 0;
        textureScreen->saveToFile(cocos2d::__String::createWithFormat("ScreenShot_%04d.png", ++s_iSerialNumber)->getCString(),
                                  cocos2d::Image::Format::PNG);
    }
    return textureScreen;

}



