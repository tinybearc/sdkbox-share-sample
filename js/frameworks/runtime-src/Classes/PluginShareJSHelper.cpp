#include "PluginShareJSHelper.h"
#include "cocos2d_specifics.hpp"
#include "PluginShare/PluginShare.h"
#include "SDKBoxJSHelper.h"

#include "js_manual_conversions.h"

jsval shareInfo_to_jsval(JSContext* cx, const sdkbox::PluginShare::ShareResponse& response) {
    JS::RootedObject proto(cx);
    JS::RootedObject parent(cx);
#if defined(MOZJS_MAJOR_VERSION) and MOZJS_MAJOR_VERSION >= 26
    JS::RootedObject jsRet(cx, JS_NewObject(cx, NULL, proto, parent));
#else
    JSObject *jsRet = JS_NewObject(cx, NULL, NULL, NULL);
#endif

#if defined(MOZJS_MAJOR_VERSION) and MOZJS_MAJOR_VERSION >= 26
    JS::RootedValue state(cx);
#else
    jsval state;
#endif
    state = int32_to_jsval(cx, response.state);
#if defined(MOZJS_MAJOR_VERSION) and MOZJS_MAJOR_VERSION >= 26
    JS_SetProperty(cx, jsRet, "state", state);
#else
    JS_SetProperty(cx, jsRet, "state", &state);
#endif

#if defined(MOZJS_MAJOR_VERSION) and MOZJS_MAJOR_VERSION >= 26
    JS::RootedValue errStr(cx);
#else
    jsval errStr;
#endif
    errStr = std_string_to_jsval(cx, response.error);
#if defined(MOZJS_MAJOR_VERSION) and MOZJS_MAJOR_VERSION >= 26
    JS_SetProperty(cx, jsRet, "error", errStr);
#else
    JS_SetProperty(cx, jsRet, "error", &errStr);
#endif

    return OBJECT_TO_JSVAL(jsRet);
}

extern JSObject* jsb_sdkbox_PluginShare_prototype;
static JSContext* s_cx = nullptr;

class IMCallbackJS: public cocos2d::CCObject {
public:
    IMCallbackJS();
    void schedule();
    void notityJs(float dt);
    int transParams(jsval** pp);

    std::string _name;

    jsval _paramVal[2];
    int _paramLen;
    sdkbox::PluginShare::ShareResponse _shareResponse;
};

class ShareListenerJS : public sdkbox::ShareListener {
private:
    JSObject* _JSDelegate;
public:
    void setJSDelegate(JSObject* delegate) {
        _JSDelegate = delegate;
    }

    JSObject* getJSDelegate() {
        return _JSDelegate;
    }

    void onShareState(const sdkbox::PluginShare::ShareResponse& response) {
        JSContext* cx = s_cx;
        IMCallbackJS* cb = new IMCallbackJS();
        cb->_shareResponse = response;
        cb->_name = "onShareState";
        cb->_paramLen = 1;
        cb->schedule();
    }

    void invokeJS(const char* func, IMCallbackJS* cb) {
        if (!s_cx) {
            return;
        }
        JSContext* cx = s_cx;
        const char* func_name = func;
        JS::RootedObject obj(cx, _JSDelegate);
        JSAutoCompartment ac(cx, obj);

#if defined(MOZJS_MAJOR_VERSION)
#if MOZJS_MAJOR_VERSION >= 33
        bool hasAction;
        JS::RootedValue retval(cx);
        JS::RootedValue func_handle(cx);
#else
        bool hasAction;
        jsval retval;
        JS::RootedValue func_handle(cx);
#endif
#elif defined(JS_VERSION)
        JSBool hasAction;
        jsval retval;
        jsval func_handle;
#endif
        int valueSize = 0;
        jsval* pVals = nullptr;
        valueSize = cb->transParams(&pVals);

        if (JS_HasProperty(cx, obj, func_name, &hasAction) && hasAction) {
            if(!JS_GetProperty(cx, obj, func_name, &func_handle)) {
                return;
            }
            if(func_handle == JSVAL_VOID) {
                return;
            }

#if MOZJS_MAJOR_VERSION >= 31
            if (0 == valueSize) {
                JS_CallFunctionName(cx, obj, func_name, JS::HandleValueArray::empty(), &retval);
            } else {
                JS_CallFunctionName(cx, obj, func_name, JS::HandleValueArray::fromMarkedLocation(valueSize, pVals), &retval);
            }
#else
            if (0 == valueSize) {
                JS_CallFunctionName(cx, obj, func_name, 0, nullptr, &retval);
            } else {
                JS_CallFunctionName(cx, obj, func_name, valueSize, pVals, &retval);
            }
#endif
        }
    }

};


IMCallbackJS::IMCallbackJS():
_paramLen(0) {
}

void IMCallbackJS::schedule() {
    retain();
    cocos2d::CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(IMCallbackJS::notityJs), this, 0.1, false);
    autorelease();
}

void IMCallbackJS::notityJs(float dt) {
    sdkbox::ShareListener* lis = sdkbox::PluginShare::getListener();
    ShareListenerJS* l = dynamic_cast<ShareListenerJS*>(lis);
    if (l) {
        l->invokeJS(_name.c_str(), this);
    }
    cocos2d::CCDirector::sharedDirector()->getScheduler()->unscheduleAllForTarget(this);
    release();
}

int IMCallbackJS::transParams(jsval** pp) {
    JSContext* cx = s_cx;
    _paramVal[0] = shareInfo_to_jsval(cx, _shareResponse);

    *pp = _paramVal;
    return _paramLen;
}


#if defined(MOZJS_MAJOR_VERSION)
#if MOZJS_MAJOR_VERSION >= 33
bool js_PluginShareJS_PluginShare_setListener(JSContext *cx, uint32_t argc, jsval *vp)
#else
bool js_PluginShareJS_PluginShare_setListener(JSContext *cx, uint32_t argc, jsval *vp)
#endif
#elif defined(JS_VERSION)
JSBool js_PluginShareJS_PluginShare_setListener(JSContext *cx, uint32_t argc, jsval *vp)
#endif
{
    s_cx = cx;
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;

    if (argc == 1) {

        if (!args.get(0).isObject())
        {
            ok = false;
        }
        JSObject *tmpObj = args.get(0).toObjectOrNull();

        JSB_PRECONDITION2(ok, cx, false, "js_PluginShareJS_PluginShare_setIAPListener : Error processing arguments");
        ShareListenerJS* wrapper = new ShareListenerJS();
        wrapper->setJSDelegate(tmpObj);
        sdkbox::PluginShare::setListener(wrapper);

        args.rval().setUndefined();
        return true;
    }
    JS_ReportError(cx, "js_PluginShareJS_PluginShare_setIAPListener : wrong number of arguments");
    return false;
}

#if defined(MOZJS_MAJOR_VERSION)
#if MOZJS_MAJOR_VERSION >= 33
bool js_PluginShareJS_PluginShare_share(JSContext *cx, uint32_t argc, jsval *vp)
#else
bool js_PluginShareJS_PluginShare_share(JSContext *cx, uint32_t argc, jsval *vp)
#endif
#elif defined(JS_VERSION)
JSBool js_PluginShareJS_PluginShare_share(JSContext *cx, uint32_t argc, jsval *vp)
#endif
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;

    do {
        if (argc == 1) {
            sdkbox::PluginShare::ShareInfo shareInfo;
            cocos2d::ValueMap map;
            ok &= jsval_to_ccvaluemap(cx, args.get(0), &map);
            cocos2d::Value v = map["text"];
            shareInfo.text = v.asString();
            sdkbox::PluginShare::share(shareInfo);

            return true;
        }
    } while (0);

    JS_ReportError(cx, "js_PluginShareJS_PluginShare_share : wrong number of arguments");
    return false;
}

// #if defined(MOZJS_MAJOR_VERSION)
// #if MOZJS_MAJOR_VERSION >= 33
// bool js_PluginShareJS_PluginShare_shareDialog(JSContext *cx, uint32_t argc, jsval *vp)
// #else
// bool js_PluginShareJS_PluginShare_shareDialog(JSContext *cx, uint32_t argc, jsval *vp)
// #endif
// #elif defined(JS_VERSION)
// JSBool js_PluginShareJS_PluginShare_shareDialog(JSContext *cx, uint32_t argc, jsval *vp)
// #endif
// {
//     JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
//     bool ok = true;

//     do {
//         if (argc == 1) {
//             sdkbox::PluginShare::ShareInfo shareInfo;
//             cocos2d::ValueMap map;
//             ok &= jsval_to_ccvaluemap(cx, args.get(0), &map);
//             cocos2d::Value v = map["text"];
//             shareInfo.text = v.asString();
//             sdkbox::PluginShare::shareDialog(shareInfo);

//             return true;
//         }
//     } while (0);

//     JS_ReportError(cx, "js_PluginShareJS_PluginShare_shareDialog : wrong number of arguments");
//     return false;
// }

#if defined(MOZJS_MAJOR_VERSION) and MOZJS_MAJOR_VERSION >= 26
void set_constants(JSContext* cx, const JS::RootedObject& obj, const std::string& name, const std::map<std::string, int>& params)
#else
void set_constants(JSContext* cx, JSObject* obj, const std::string& name, const std::map<std::string, int>& params)
#endif
{
    jsval val = sdkbox::std_map_string_int_to_jsval(cx, params);

    JS::RootedValue rv(cx);
    rv = val;
#if defined(MOZJS_MAJOR_VERSION) and MOZJS_MAJOR_VERSION >= 26
    JS_SetProperty(cx, obj, name.c_str(), rv);
#else
    JS_SetProperty(cx, obj, name.c_str(), rv.address());
#endif
}

#if defined(MOZJS_MAJOR_VERSION) and MOZJS_MAJOR_VERSION >= 26
void register_constants(JSContext* cx, const JS::RootedObject& obj)
#else
void register_constants(JSContext* cx, JSObject* obj)
#endif
{
    std::map<std::string, int> enums;
    enums.clear();
    enums.insert(std::make_pair("ShareStateNone", 0));
    enums.insert(std::make_pair("ShareStateUnkonw", 1));
    enums.insert(std::make_pair("ShareStateBegin", 2));
    enums.insert(std::make_pair("ShareStateSuccess", 3));
    enums.insert(std::make_pair("ShareStateFail", 4));
    enums.insert(std::make_pair("ShareStateCancelled", 5));
    set_constants(cx, obj, "ShareState", enums);
}

#if defined(MOZJS_MAJOR_VERSION)
#if MOZJS_MAJOR_VERSION >= 33
void register_all_PluginShareJS_helper(JSContext* cx, JS::HandleObject global) {
    JS::RootedObject pluginObj(cx);
    sdkbox::getJsObjOrCreat(cx, global, "sdkbox.PluginShare", &pluginObj);

    JS_DefineFunction(cx, pluginObj, "setListener", js_PluginShareJS_PluginShare_setListener, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, pluginObj, "share", js_PluginShareJS_PluginShare_share, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    // JS_DefineFunction(cx, pluginObj, "shareDialog", js_PluginShareJS_PluginShare_shareDialog, 1, JSPROP_READONLY | JSPROP_PERMANENT);

    register_constants(cx, pluginObj);
}
#else
void register_all_PluginShareJS_helper(JSContext* cx, JSObject* global) {
    JS::RootedObject pluginObj(cx);
    sdkbox::getJsObjOrCreat(cx, JS::RootedObject(cx, global), "sdkbox.PluginShare", &pluginObj);

    JS_DefineFunction(cx, pluginObj, "setListener", js_PluginShareJS_PluginShare_setListener, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, pluginObj, "share", js_PluginShareJS_PluginShare_share, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    // JS_DefineFunction(cx, pluginObj, "shareDialog", js_PluginShareJS_PluginShare_shareDialog, 1, JSPROP_READONLY | JSPROP_PERMANENT);

    register_constants(cx, pluginObj);
}
#endif
#elif defined(JS_VERSION)
void register_all_PluginShareJS_helper(JSContext* cx, JSObject* global) {
    jsval pluginVal;
    JSObject* pluginObj;
    pluginVal = sdkbox::getJsObjOrCreat(cx, global, "sdkbox.PluginShare", &pluginObj);

    JS_DefineFunction(cx, pluginObj, "setListener", js_PluginShareJS_PluginShare_setListener, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    JS_DefineFunction(cx, pluginObj, "share", js_PluginShareJS_PluginShare_share, 1, JSPROP_READONLY | JSPROP_PERMANENT);
    // JS_DefineFunction(cx, pluginObj, "shareDialog", js_PluginShareJS_PluginShare_shareDialog, 1, JSPROP_READONLY | JSPROP_PERMANENT);

    register_constants(cx, pluginObj);
}
#endif
