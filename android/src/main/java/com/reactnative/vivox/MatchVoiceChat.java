package com.reactnative.vivox;

public class MatchVoiceChat {
    public static native boolean serverConnect(String vivoxServer, String issuer, String key, String realm);

    public static native boolean serverDisconnect();

    public static native boolean matchJoin(String matchName);

    public static native boolean matchLeave();

    public static native int getState();

    public static native String getStateName(int state);

    public static native void setCallback(Class<?> theClass, String staticMethod);
}
