#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "PluginShare/PluginShare.h"

USING_NS_CC;

using namespace cocostudio::timeline;

class SListener : public sdkbox::ShareListener {
public:
    virtual void onShareState(const sdkbox::PluginShare::ShareResponse& response) {
        switch (response.state) {
            case sdkbox::PluginShare::ShareState::ShareStateNone: {
                CCLOG("SharePlugin::onShareState none");
                break;
            }
            case sdkbox::PluginShare::ShareState::ShareStateUnkonw: {
                CCLOG("SharePlugin::onShareState unkonw");
                break;
            }
            case sdkbox::PluginShare::ShareState::ShareStateBegin: {
                CCLOG("SharePlugin::onShareState begin");
                break;
            }
            case sdkbox::PluginShare::ShareState::ShareStateSuccess: {
                CCLOG("SharePlugin::onShareState success");
                break;
            }
            case sdkbox::PluginShare::ShareState::ShareStateFail: {
                CCLOG("SharePlugin::onShareState fail, error:%s", response.error.c_str());
                break;
            }
            case sdkbox::PluginShare::ShareState::ShareStateCancelled: {
                CCLOG("SharePlugin::onShareState cancelled");
                break;
            }
            default: {
                CCLOG("SharePlugin::onShareState");
                break;
            }
        }
    }
};

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
//    auto rootNode = CSLoader::createNode("MainScene.csb");
//
//    addChild(rootNode);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    auto label = Label::createWithTTF("InMobi Sample", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));
    
    // add the label as a child to this layer
    this->addChild(label, 1);
    
    Menu* list = Menu::create(
                              MenuItemFont::create("Share", CC_CALLBACK_1(HelloWorld::onShareClick, this)),
                              NULL);
    list->alignItemsVerticallyWithPadding(5);
    list->setPosition(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2);
    addChild(list);

    sdkbox::PluginShare::setListener(new SListener());
    sdkbox::PluginShare::init();

    return true;
}

void HelloWorld::onShareClick(Ref *sender) {
    sdkbox::PluginShare::ShareInfo info;
    info.text = getRandomString("#sdkbox(www.sdkbox.com) - the cure for sdk fatigue ");
    sdkbox::PluginShare::share(info);
}

std::string HelloWorld::getRandomString(const std::string& str) {
    char alphabet[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x', 'y', 'z'};
    
    std::stringstream ss;
    ss << str;
    int max = random(5, 10);
    for (int i = 0; i < max; i++) {
        ss << alphabet[random(0, 25)];
    }
    
    return ss.str();
}
