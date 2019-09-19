# react-native-vivox

## Getting started

`$ yarn add react-native-vivox`

### Mostly automatic installation (RN < 0.60)

`$ react-native link react-native-vivox`

### Additional steps
#### Android
1. Unzip `aar.zip`[(download)](https://we.tl/t-gQtUUK1F6A) in `android`
2. Add the following code in `android/app/build.gradle`
```groovy
repositories {
    flatDir {
        dirs "../aar"
    }
}

dependencies {
    // ...
    debugImplementation(name: 'sdk-debug', ext: 'aar')
    releaseImplementation(name: 'sdk-release', ext: 'aar')
}
```
3. Add the following code in `android/app/src/main/java/com.package.name/MainApplication.java`
```java
import com.vivox.sdk.JniHelpers; // <-- this line

// ...

@Override
public void onCreate() {
  super.onCreate();
  JniHelpers.init(getApplicationContext(), null, new String[]{"mvc"}); // <-- this line
}
```

#### iOS
1. Add (link) `libvivoxsdk.a` (from `/react-native-vivox/ios/core/bin/${CONFIGURATION}/libvivoxsdk.a`) into Frameworks
2. Add (link) `libresolv.tbd` [(download)](https://we.tl/t-mEIWcVv26j) into Frameworks
3. Add the following to your App's `Info.plist`
```xml
<key>NSMicrophoneUsageDescription</key>
<string>Need microphone access to communicate with other players</string>
<key>UIBackgroundModes</key>
<array>
  <string>audio</string>
  <string>voip</string>
</array>
```

## Usage
### Example
This example will connect your app to your Vivox server and join the `test123` voice channel. Make sure you requested microphone permission before!
```javascript
import React from 'react';
import * as Vivox from 'react-native-vivox';

class App extends React.Component {
  async componentDidMount(): void {
    await Vivox.connect("https://vdx5.www.vivox.com/api2/", "issuer", "vdx5.vivox.com");
    await Vivox.login("userId", "loginToken");
    await Vivox.joinChannel("test123", "joinToken");
  }

  render() {
    return null;
  }
}

export default App;
```
Example app: https://github.com/lucienbl/react-native-vivox-demo
### Connect to the Vivox server
```javascript
Vivox.connect("server", "issuer", "realm"); // returns Promise<>
```

### Login the user
```javascript
Vivox.login("userId", "loginToken"); // returns Promise<>
```

### Join a channel
```javascript
Vivox.joinChannel("channelId", "joinToken"); // returns Promise<>
```

### Leave current channel
```javascript
Vivox.leaveChannel(); // returns Promise<>
```

### Disconnect from the Vivox server
```javascript
Vivox.disconnect(); // returns Promise<>
```

### Retrieve the current state integer
```javascript
Vivox.getState(); // returns Promise<number>
```

### Retrieve the current state name
```javascript
Vivox.getStateName(); // returns Promise<string>
```

### Mute the current player
```javascript
Vivox.muteMyself(true); // returns Promise<>
```

### Retrieve if the current player is muted or not
```javascript
Vivox.isMuted(); // returns Promise<boolean>
```

### Retrieve the currently speaking players
```javascript
Vivox.getSpeakingParticipants(); // returns Promise<Object>
```

### Mute / Unmute a player for the current user
```javascript
Vivox.setParticipantMutedForMe("targetUserId", true); // returns Promise<>
```
