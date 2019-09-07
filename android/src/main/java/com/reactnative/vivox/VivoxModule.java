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
    public void connect(String vivoxServer, String issuer, String realm, Promise promise) {
        try {
            promise.resolve(MatchVoiceChat.serverConnect(vivoxServer, issuer, realm));
        } catch (Exception e) {
            promise.reject(e);
        }
    }

    @ReactMethod
    public void setLoginCredentials(String userId, String userToken, Promise promise) {
        try {
            promise.resolve(MatchVoiceChat.setLoginCredentials(userId, userToken));
        } catch (Exception e) {
            promise.reject(e);
        }
    }

    @ReactMethod
    public void joinMatch(String matchName, String matchToken, Promise promise) {
        try {
            promise.resolve(MatchVoiceChat.matchJoin(matchName, matchToken));
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
    public void getStateName(Promise promise) {
        try {
            promise.resolve(MatchVoiceChat.getStateName(MatchVoiceChat.getState()));
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
