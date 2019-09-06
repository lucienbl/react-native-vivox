package com.reactnative.vivox;

import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.bridge.Callback;

public class VivoxModule extends ReactContextBaseJavaModule {

    private final ReactApplicationContext reactContext;

    public VivoxModule(ReactApplicationContext reactContext) {
        super(reactContext);
        this.reactContext = reactContext;
    }

    @Override
    public String getName() {
        return "Vivox";
    }

    @ReactMethod
    public void connect(String vivoxServer, String issuer, String key, String realm, Promise promise) {
        try {
            promise.resolve(MatchVoiceChat.serverConnect(vivoxServer, issuer, key, realm));
        } catch (Exception e) {
            promise.reject(e);
        }
    }

    @ReactMethod
    public void joinMatch(String matchName, Promise promise) {
        try {
            promise.resolve(MatchVoiceChat.matchJoin(matchName));
        } catch (Exception e) {
            promise.reject(e);
        }
    }

    @ReactMethod
    public void leaveMatch(Promise promise) {
        try {
            promise.resolve(MatchVoiceChat.matchLeave());
        } catch (Exception e) {
            promise.reject(e);
        }
    }

    @ReactMethod
    public void disconnect(Promise promise) {
        try {
            promise.resolve(MatchVoiceChat.serverDisconnect());
        } catch (Exception e) {
            promise.reject(e);
        }
    }
}
