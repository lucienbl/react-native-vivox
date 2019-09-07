package com.reactnative.vivox;

public class MatchVoiceChat {
    public static native boolean serverConnect(String vivoxServer, String issuer, String realm);

    public static native boolean serverDisconnect();

    public static native boolean matchJoin(String matchName, String matchToken);

    public static native boolean matchLeave();

    public static native int getState();

    public static native boolean setLoginCredentials(String userId, String userToken);

    public static native String getStateName(int state);

    public static native void setCallback(Class<?> theClass, String staticMethod);
}
