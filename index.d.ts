export = Vivox;
export as namespace Vivox;

namespace Vivox {
  /**
   * Connects the app to the Vivox server.
   * @param serverUrl The vivox server url to connect to
   * @param issuer The vivox issuer
   * @param realm The vivox realm
   * 
   * @returns {Promise<any>}
   */
  function connect(serverUrl: string, issuer: string, realm: string): Promise<any>;

  /**
   * Log in the current user
   * 
   * @param userId The id of the current user
   * @param token The Vivox authorization token to login
   * 
   * @returns {Promise<any>}
   * 
   * @deprecated Use {@link setLoginCredentials} instead
   */
  function login(userId: string, token: string): Promise<any>;

  /**
   * Set the credentials for the current user
   * 
   * @param userId The id of the current user
   * @param token The Vivox authorization token to login
   * 
   * @returns {Promise<any>}
   */
  function setLoginCredentials(userId: string, token: string): Promise<any>;

  /**
   * Join a voice channel
   * 
   * @param channelId The channel id to join
   * @param token The Vivox authorization token to join a channel
   * 
   * @returns {Promise<any>}
   * 
   * @deprecated Use {@link joinMatch} instead
   */
  function joinChannel(channelId: string, token: string): Promise<any>;

  /**
   * Leaves the current channel
   * 
   * @returns {Promise<any>}
   * 
   * @deprecated Use {@link leaveMatch} instead
   */
  function leaveChannel(): Promise<any>;

  /**
   * Disconnects from the Vivox server.
   * 
   * @returns {Promise<any>}
   */
  function disconnect(): Promise<any>;

  /**
   * Returns the current state name.
   * 
   * @returns {Promise<keyof State>}
   */
  function getStateName(): Promise<keyof State>;

  /**
   * Returns the current state.
   * 
   * @returns {Promise<State>}
   */
  function getState(): Promise<State>;

  /**
   * Mute / Unmute the current user.
   * 
   * @param value Whether to mute or unmute
   */
  function muteMyself(value: boolean): Promise<any>;

  /**
   * Returns the mute state of the current user.
   * 
   * @returns {Promise<boolean>}
   */
  function isMuted(): Promise<boolean>;

  /**
   * Returns a map of the currently speaking participants.
   * 
   * @returns {Promise<Object<userId, SpeakingParticipant>>}
   */
  function getSpeakingParticipants(): Promise<Object<userId, SpeakingParticipant>>;

  /**
   * Mute / Unmute a player for myself.
   * 
   * @param targetUserId The id of the user to mute for myself.
   * @param muted Whether to mute or unmute that user.
   * 
   * @returns {Promise<any>}
   */
  function setParticipantMutedForMe(targetUserId: string, muted: boolean): Promise<any>;

  /**
   * Mute / Unmute everyone for me.
   * 
   * @param muted Whether to mute the device or not.
   * 
   * @returns {Promise<any>}
   */
  function setAudioOutputDeviceMuted(muted: boolean): Promise<any>;

  /**
   * Set participant output volume for me
   * 
   * @param targetUserId The id of the user the volume should be modified.
   * @param volume The volume to set. Between 0 and 100.
   * 
   * @returns {Promise<any>}
   */
  function setParticipantAudioOutputDeviceVolumeForMe(targetUserId: string, volume: Range<0, 100>): Promise<any>;
}

export enum State {
  stateUninitialized,
  stateThreadStarted,
  stateSDKInitializing,
  stateSDKInitFailed,
  stateSDKInitialized,
  stateSDKUninitializing,
  stateConnecting,
  stateConnectRetry,
  stateConnected,
  stateDisconnecting,
  stateLoggingIn,
  stateLoginRetry,
  stateInvalidLoginCredentials,
  stateLoggedIn,
  stateLoggingOut,
  stateHaveChannelID,
  stateJoiningChannel,
  stateJoinChannelRetry,
  stateInvalidChannelCredentials,
  stateInMatch,
  stateLeavingChannel,
}

export interface SpeakingParticipant {
  isSpeaking: boolean
}
