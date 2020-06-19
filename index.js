import { NativeModules } from 'react-native';

const { Vivox } = NativeModules;

if (!Vivox) {
  Vivox = {
    connect: () => Promise.resolve(),
    login: () => Promise.resolve(),
    joinChannel: () => Promise.resolve(),
    leaveChannel: () => Promise.resolve(),
    disconnect: () => Promise.resolve(),
    getStateName: () => Promise.resolve('Invalid state, ios simulator'),
    getState: () => Promise.resolve(0),
    muteMyself: () => Promise.resolve(),
    isMuted: () => Promise.resolve(false),
    getSpeakingParticipants: () => Promise.resolve({}),
    setParticipantMutedForMe: () => Promise.resolve(),
    setAudioOutputDeviceMuted: () => Promise.resolve(),
    setParticipantAudioOutputDeviceVolumeForMe: () => Promise.resolve(),
  };
} else {
  // Support deprecated methods
  Vivox.joinChannel = (channelId: string, token: string) => Vivox.joinMatch(channelId, token);
  Vivox.leaveChannel = () => Vivox.leaveMatch();
  Vivox.login = (userId: string, token: string) => Vivox.setLoginCredentials(userId, token);
}

Vivox.State = {
  stateUninitialized: 0,
  stateThreadStarted: 1,
  stateSDKInitializing: 2,
  stateSDKInitFailed: 3,
  stateSDKInitialized: 4,
  stateSDKUninitializing: 5,
  stateConnecting: 6,
  stateConnectRetry: 7,
  stateConnected: 8,
  stateDisconnecting: 9,
  stateLoggingIn: 10,
  stateLoginRetry: 11,
  stateInvalidLoginCredentials: 12,
  stateLoggedIn: 13,
  stateLoggingOut: 14,
  stateHaveChannelID: 15,
  stateJoiningChannel: 16,
  stateJoinChannelRetry: 17,
  stateInvalidChannelCredentials: 18,
  stateInMatch: 19,
  stateLeavingChannel: 20
}

export default Vivox;