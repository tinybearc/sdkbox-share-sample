
var HelloWorldLayer = cc.Layer.extend({
    sprite:null,
    ctor:function () {
        //////////////////////////////
        // 1. super init first
        this._super();

        /////////////////////////////
        // 2. add a menu item with "X" image, which is clicked to quit the program
        //    you may modify it.
        // ask the window size
        var size = cc.winSize;

        // var mainscene = ccs.load(res.MainScene_json);
        // this.addChild(mainscene.node);

        var coinsLabel = cc.Label.createWithSystemFont("Hello Js", "Arial", 40);
        coinsLabel.setPosition(size.width/2, size.height*9/10);
        this.addChild(coinsLabel);

        cc.MenuItemFont.setFontName('arial');
        cc.MenuItemFont.setFontSize(32);

        var menu = new cc.Menu(
            new cc.MenuItemFont("share", function () {
                invokeSDK();
            }, this)
            );
        menu.setPosition(size.width/2, size.height/2);
        menu.alignItemsVerticallyWithPadding(20);
        this.addChild(menu);

        var initSDK = function() {
            if ("undefined" == typeof(sdkbox)) {
                console.log("sdkbox is not exist")
                return
            }

            if ("undefined" != typeof(sdkbox.PluginShare)) {
                var plugin = sdkbox.PluginShare
                plugin.setListener({
                  onShareState: function(response) {
                    console.log("PluginShare onSharestate:" + response.state + " error:" + response.error)
                    if (response.state == sdkbox.PluginShare.ShareState.ShareStateSuccess) {
                        console.log("post success")
                    }
                }
                })
                plugin.init()
                console.log('Share plugin initialized')
            } else {
                console.log("no plugin init")
            }
        }
        var invokeSDK = function() {
            if ("undefined" == typeof(sdkbox)) {
                console.log("sdkbox is not exist")
                return
            }

            function addRandomSuffix(s) {
                var $chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefhijklmnopqrstuvwxyz';
                var maxPos = $chars.length;
                var len = Math.floor(Math.random() * 5) + 5
                for (i = 5; i < len; i++) {
                    s += $chars.charAt(Math.floor(Math.random() * maxPos));
                }
                return s;
            }

            if ("undefined" != typeof(sdkbox.PluginShare)) {
                console.log('share post')
                var plugin = sdkbox.PluginShare
                plugin.share( {text : addRandomSuffix("#sdkbox(www.sdkbox.com) - the cure for sdk fatigue - from js - ")} );
            } else {
                console.log("no plugin invoked")
            }
        }

        initSDK();

        return true;
    }
});

var HelloWorldScene = cc.Scene.extend({
    onEnter:function () {
        this._super();
        var layer = new HelloWorldLayer();
        this.addChild(layer);
    }
});

