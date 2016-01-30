
local MainScene = class("MainScene", cc.load("mvc").ViewBase)

-- MainScene.RESOURCE_FILENAME = "MainScene.csb"

function MainScene:onCreate()
    -- printf("resource node = %s", tostring(self:getResourceNode()))
    cc.MenuItemFont:setFontName("Arial")
    cc.Menu:create(
                   cc.MenuItemFont:create("clickme"):onClicked(function()
                        print("on click")
                        self:invokeFun()
                    end)
                   )
        :move(display.cx, display.cy)
        :addTo(self)
        :alignItemsVerticallyWithPadding(20)

    cc.Label:createWithSystemFont("Hello Lua", "Arial", 40)
            :move(display.cx, display.height*9/10)
            :addTo(self)

    self:initSDK()
end

function MainScene:initSDK()
    if not sdkbox then
        print "sdkbox is nil"
        return
    end

    print "Entry initSDK"

    if sdkbox.PluginShare then
        local plugin = sdkbox.PluginShare
        plugin:setListener(function(args)
            local event = args.event
            dump(args, "PluginShare share listener info:")
        end)
        plugin:init()
    else
        print "not found plugin"
    end
end

function MainScene:invokeFun()
    if not sdkbox then
        print "sdkbox is nil"
        return
    end

    print "Entry invokeFun"

    if sdkbox.PluginShare then
        local plugin = sdkbox.PluginShare
        local function addRandomSuffix(s)
            local str = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefhijklmnopqrstuvwxyz'
            local len = math.random(5, 10)
            for i=5, len do
                local v = math.random(1, string.len(str))
                s = s .. string.sub(str, v, v)
            end

            return s
        end
        plugin:share({text=addRandomSuffix('#sdkbox(www.sdkbox.com) - the cure for sdk fatigue - from lua - ')})
    else
        print "not found plugin"
    end
end

return MainScene
