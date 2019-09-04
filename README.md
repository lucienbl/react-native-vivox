# react-native-vivox

## Getting started

`$ npm install react-native-vivox --save`

### Mostly automatic installation

`$ react-native link react-native-vivox`

### Manual installation


#### iOS

1. In XCode, in the project navigator, right click `Libraries` ➜ `Add Files to [your project's name]`
2. Go to `node_modules` ➜ `react-native-vivox` and add `Vivox.xcodeproj`
3. In XCode, in the project navigator, select your project. Add `libVivox.a` to your project's `Build Phases` ➜ `Link Binary With Libraries`
4. Run your project (`Cmd+R`)<

#### Android

1. Open up `android/app/src/main/java/[...]/MainApplication.java`
  - Add `import com.reactlibrary.VivoxPackage;` to the imports at the top of the file
  - Add `new VivoxPackage()` to the list returned by the `getPackages()` method
2. Append the following lines to `android/settings.gradle`:
  	```
  	include ':react-native-vivox'
  	project(':react-native-vivox').projectDir = new File(rootProject.projectDir, 	'../node_modules/react-native-vivox/android')
  	```
3. Insert the following lines inside the dependencies block in `android/app/build.gradle`:
  	```
      compile project(':react-native-vivox')
  	```


## Usage
```javascript
import Vivox from 'react-native-vivox';

// TODO: What to do with the module?
Vivox;
```
