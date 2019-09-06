# react-native-vivox

## Getting started

`$ yarn add react-native-vivox`

### Mostly automatic installation (RN < 0.60)

`$ react-native link react-native-vivox`

### Additional steps
#### Android
1. Unzip `aar.zip` in `android`
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


## Usage
### Example
This example will connect your app to your Vivox server and join the `test123` voice channel. Make sure you requested microphone permission before!
```javascript
import React from 'react';
import Vivox from 'react-native-vivox';

class App extends React.Component {
  async componentDidMount(): void {
    await Vivox.connect("https://vdx5.www.vivox.com/api2/", "issuer", "key", "vdx5.vivox.com");
    await Vivox.joinMatch("test123");
  }

  render() {
    return null;
  }
}

export default App;

```
### Connect to the Vivox server
```javascript
Vivox.connect("server", "issuer", "key", "realm"); // returns Promise<>
```
