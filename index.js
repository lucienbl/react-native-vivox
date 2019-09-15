import { NativeModules } from 'react-native';

const { Vivox } = NativeModules;

export const connect = async (serverUrl: string, issuer: string, realm: string) => {
  await Vivox.connect(serverUrl, issuer, realm);
};

export const login = (userId: string, token: string) => {
  setTimeout(async () => {
    await Vivox.setLoginCredentials(userId, token);
  }, 2000);
};

export const joinChannel = (channelId: string, token: string) => {
  this.setTimeout(async () => {
    await Vivox.joinMatch(channelId, token);
  }, 100);
};

export const leaveChannel = async () => {
  await Vivox.leaveMatch();
};

export const disconnect = async () => {
  await Vivox.disconnect();
};

export const getStateName = async (): string => {
  return Vivox.getStateName();
};

export const getState = async (): number => {
  return Vivox.getState();
};

export const muteMyself = async (value: boolean): void => {
  return Vivox.muteMyself(value);
};

export const isMuted = async (): boolean => {
  return Vivox.isMuted();
};
