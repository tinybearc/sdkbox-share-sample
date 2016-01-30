/****************************************************************************

 Copyright (c) 2014-2015 SDKBOX Inc

 ****************************************************************************/

#ifndef _PLUGIN_SHARE_H_
#define _PLUGIN_SHARE_H_

#include <string>
#include <map>

namespace sdkbox {

    class ShareListener;
    class PluginShare {
    public:

        enum ShareState {
            ShareStateNone,
            ShareStateUnkonw,
            ShareStateBegin,
            ShareStateSuccess,
            ShareStateFail,
            ShareStateCancelled
        };

        struct ShareResponse {
            ShareState state;
            std::string error;
            //SharePlatform platform;
        };

        struct ShareInfo {
            std::string text;
        };

        /**
         *  initialize the plugin instance.
         */
        static bool init();

        /**
         * Set listener to listen for share events
         */
        static void setListener(ShareListener* listener);

        /**
         * Get the listener
         */
        static ShareListener* getListener();

        /**
         * Remove the listener, and can't listen to events anymore
         */
        static void removeListener();

        /**
         * Share content
         */
        static void share(const ShareInfo& info);

        };

    class ShareListener {
    public:

        /**
         * Notifies the delegate that share completion
         */
        virtual void onShareState(const PluginShare::ShareResponse& response) {};

    };
}

#endif
