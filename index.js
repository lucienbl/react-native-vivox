import { NativeModules } from 'react-native';

const { Vivox } = NativeModules;

/**
 * Connects the app to the Vivox server.
 * @param serverUrl
 * @param issuer
 * @param realm
 * @returns {Promise<void>}
 */
export const connect = async (serverUrl: string, issuer: string, realm: string) => {
  await Vivox.connect(serverUrl, issuer, realm);
};

/**
 * Log in the current player.
 * @param userId
 * @param token
 */
export const login = (userId: string, token: string) => {
  setTimeout(async () => {
    await Vivox.setLoginCredentials(userId, token);
  }, 2000);
};

/**
 * Join a voice channel.
 * @param channelId
 * @param token
 */
export const joinChannel = (channelId: string, token: string) => {
  this.setTimeout(async () => {
    await Vivox.joinMatch(channelId, token);
  }, 100);
};

/**
 * Leave the current channel
 * @returns {Promise<void>}
 */
export const leaveChannel = async () => {
  await Vivox.leaveMatch();
};

/**
 * Disconnect from the Vivox server.
 * @returns {Promise<void>}
 */
export const disconnect = async () => {
  await Vivox.disconnect();
};

/**
 * Retrieve the current state name.
 * @returns {Promise<*>}
 */
export const getStateName = async (): string => {
  return Vivox.getStateName();
};

/**
 * Retrieve the current state integer.
 * @returns {Promise<NavigationPreloadState>}
 */
export const getState = async (): number => {
  return Vivox.getState();
};

/**
 * Mute / Unmute the current player
 * @param value
 * @returns {Promise<*>}
 */
export const muteMyself = async (value: boolean): void => {
  return Vivox.muteMyself(value);
};

/**
 * Retrieve if the current player is muted or not.
 * @returns {Promise<*>}
 */
export const isMuted = async (): boolean => {
  return Vivox.isMuted();
};

/**
 * Retrieve the currently speaking players.
 * @returns {Promise<Object>}
 */
export const getSpeakingParticipants = async ():Object => {
  return Vivox.getSpeakingParticipants();
} ;
