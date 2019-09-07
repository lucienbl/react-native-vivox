import { NativeModules } from 'react-native';

const { Vivox } = NativeModules;

export const connect = async (serverUrl: string, issuer: string, realm: string) => {
  await Vivox.connect(serverUrl, issuer, realm);
};

export const login = (userId: string, token: string) => {
  setTimeout(async () => {
    await Vivox.setLoginCredentials(userId, token);
  }, 100);
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
