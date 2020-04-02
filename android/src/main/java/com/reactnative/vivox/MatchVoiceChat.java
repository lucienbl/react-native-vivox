package com.reactnative.vivox;

import java.util.HashMap;

public class MatchVoiceChat {
    public static native boolean serverConnect(String vivoxServer, String issuer, String realm);

    public static native boolean serverDisconnect();

    public static native boolean matchJoin(String matchName, String matchToken);

    public static native boolean matchLeave();

    public static native int getState();

    public static native boolean setLoginCredentials(String userId, String userToken);

    public static native String getStateName(int state);

    public static native void setCallback(Class<?> theClass, String staticMethod);

    public static native boolean muteMyself(boolean value);

    public static native boolean isMuted();

    public static native HashMap<String, String> getSpeakingParticipants();

    public static native boolean setParticipantMutedForMe(String targetUserId, boolean muted);

    public static native boolean setAudioOutputDeviceMuted(boolean muted);
}
