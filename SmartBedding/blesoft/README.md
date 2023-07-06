# Bluetooth Low Energy

Tutorial Febrero 2023. [Link del tutorial](https://www.youtube.com/watch?v=UuHLPsjp6fM&t=958s)

## Creation of BLE Project

Use the following commands:

- Generate the project `npx create-expo-app -t expo-template-blank-typescript@47.0.12 ble-control-esp`
- Install BLE Package `npx expo install react-native-ble-plx @config-plugins/react-native-ble-plx`
- Other libraries `npx expo install expo-device react-native-base64`
- Install skia `npx expo install @shopify/react-native-skia`

### create eas.json

create the following file:
`eas.json`

```json
{
  "build": {
    "development": {
      "developmentClient": true,
      "distribution": "internal"
    },
    "preview": {
      "distribution": "internal"
    },
    "production": {}
  }
}
```

In `app.json` add the following bellow `"userInterfaceStyle": "light",`

```json
"plugins": [
      [
        "@config-plugins/react-native-ble-plx",
        {
          "isBackgroundEnabled": true,
          "modes": [
            "peripheral",
            "central"
          ],
          "bluetoothAlwaysPermission": "Allow $(PRODUCT_NAME) to connect to bluetooth devices"
        }
      ]
    ],
```

- Install eas-cli `npx npm install eas-cli`

## Prebuild the project

- run prebuild the project `npx expo prebuild`
  On this, use a bundle identifiar like "com.bleesp", or try "com.fabioc9675.bleesp", but it is possible the last one has an issue

- If you want to run the app in simulator using expo, run `npx expo install expo-dev-client`

## Run the app

- use the following command `npx expo run:android`
  si sale error se debe crear el archivo `local.properties` y agregar la direccion del SDK `sdk.dir = /Users/fabiancastano/Library/Android/sdk`

### Release on Android

Para lanzar la aplicacion y dejarla instalada en Android usar `npx react-native run-android --variant=release`
