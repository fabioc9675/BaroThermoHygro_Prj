import React, { Component, useState } from "react";
import {
  SafeAreaView,
  StyleSheet,
  Text,
  View,
  StatusBar,
  Button,
  FlatList,
  TouchableOpacity,
  Image,
  Platform,
  PermissionsAndroid,
} from "react-native";
import base64 from "react-native-base64";
import { BleManager } from "react-native-ble-plx";

const transactionId = "moniter";

export default class SB_BLE extends Component {
  // creation of constructor for SB_BLE
  constructor() {
    super();
    this.manager = new BleManager();
    this.state = {
      devices: [],
      deviceid: "",
      serviceUUID: "",
      characteristicsUUID: "",
      characteristics_TXID: "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
      characteristics_RXID: "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
      characteristics_NOID: "6E400004-B5A3-F393-E0A9-E50E24DCCA9E",
      text1: "Hello BLE",
      text_rx: "None",
      text_no: "None",
      makedata: [],
      showToast: false,
      notificationReceiving: false,
    };
    console.log("App is running");
  }

  // function to unsafe mount
  UNSAFE_componentWillMount() {
    this.manager = new BleManager();
    if (Platform.OS === "android" && Platform.Version >= 23) {
      PermissionsAndroid.check(
        PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION
      ).then((result) => {
        if (result) {
          console.log("Permission is OK");
        } else {
          PermissionsAndroid.request(
            PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION
          ).then((result) => {
            if (result) {
              console.log("User accept");
            } else {
              console.log("User refuse");
            }
          });
        }
        console.log("Running on Android");
      });
    }
    if (Platform.OS === "ios") {
      console.log("Running on iOS");
    }
  }
  // function that is execute when component finish mount
  componentDidMount() {}

  componentWillUnmount() {
    this.manager.cancelTransaction(transactionId);
    this.manager.stopDeviceScan();
    this.manager.destroy();
    delete this.manager;
  }

  changeTextFunction = () => {
    this.setState({
      text1: "Disconnect",
      deviceid: "noNull",
    });
  };

  // function for disconnect device
  disconnect() {
    console.log("disconnect");

    return new Promise((resolve, reject) => {
      this.manager
        .cancelDeviceConnection(this.state.deviceid)
        .then((rest) => {
          console.log(rest);
          let cleanState = {};
          Object.keys(this.state).forEach((x) => {
            if (x == "makedata") {
              cleanState[x] = [];
            } else {
              cleanState[x] = null;
            }
          });
          this.setState(cleanState);
        })
        .catch((err) => console.log("error on cancel connection", err));
    });
  }

  // function fos scan and connect device
  async scanAndConnect() {
    console.log("scan and connect");
    this.setState({ text1: "Scanning..." });
    this.setState({ devices: [] });

    // begin device scanning
    this.manager.startDeviceScan(null, null, (error, device) => {
      console.log("Scanning...");
      if (null) {
        console.log("null");
      }
      if (error) {
        if (Platform.OS === "ios") {
          this.alert("Error in scan => " + error);
        } else {
          console.log("Error in scan => " + error);
        }

        this.setState({ text1: "" });
        this.manager.stopDeviceScan();
        return;
      }
      if (device.name) {
        console.log("testing");
        console.log(device.name);
        console.log(device.id);

        var alrExist = false;
        this.state.devices.forEach((element) => {
          if (element.id === device.id) {
            alrExist = true;
          }
        });

        if (alrExist === false) {
          this.setState((prevState) => ({
            devices: [
              ...prevState.devices,
              {
                name: device.name,
                id: device.id,
                key: device.id,
                dev: device,
              },
            ],
          }));
        }
      } else {
        console.log("Not devices...");
        this.manager.stopDeviceScan();
      }

      setTimeout(() => {
        this.manager.stopDeviceScan();
        console.log("Stop scanning");
      }, 2000);
    });
  }

  // function to connect to a device
  async connectBleDevice(id, dev) {
    this.setState({ text1: id });
    //console.log(dev);
    // connection sequence
    this.manager
      .connectToDevice(dev.id, { autoConnect: true })
      .then((dev) => {
        (async () => {
          const services = await dev.discoverAllServicesAndCharacteristics();
          //console.log("Services ....");
          //console.log(services);
          const characteristic = await this.getServicesAndCharacteristics(
            services
          );
          console.log(
            "Discovering services and characteristics",
            characteristic.uuid
          );
          console.log("fab serv----- ", characteristic.serviceUUID);
          console.log("fab char----- ", characteristic.uuid);
          this.setState({
            deviceid: dev.id,
            serviceUUID: characteristic.serviceUUID,
            characteristicsUUID: characteristic.uuid,
            device: dev,
          });
          //console.log(this.state);
          this.setState({ text1: "connected to " + dev.name });
        })();
        this.setState({ device: dev });
        return dev.discoverAllServicesAndCharacteristics();
      })
      .then((device) => {})
      .then(
        () => {
          console.log("Listenning...");
        },
        (error) => {
          if (Platform.OS === "ios") {
            this.alert("Connection error" + JSON.stringify(error));
          } else {
            console.log("Connection error" + JSON.stringify(error));
          }
        }
      );
  }

  // function to get services and characteristics
  getServicesAndCharacteristics(device) {
    return new Promise((resolve, reject) => {
      device.services().then((services) => {
        const characteristics = [];
        console.log("ashu_1", services);
        services.forEach((service, i) => {
          service.characteristics().then((c) => {
            console.log("service.characteristics");

            characteristics.push(c);
            console.log(characteristics);
            if (i === services.length - 1) {
              const temp = characteristics.reduce((acc, current) => {
                return [...acc, ...current];
              }, []);
              const dialog = temp.find(
                (characteristics) => characteristics.isWritableWithoutResponse
              );
              if (!dialog) {
                reject("No writable characteristic");
              }
              resolve(dialog);
            }
          });
        });
      });
    });
  }

  // function for write message through ble
  async writeMessage(code, message) {
    console.log(code);
    this.setState({
      text1: message,
    });

    this.manager.cancelTransaction(transactionId);
    var device = this.state.device;
    const sendData = base64.encode(message);
    console.log("Message to send ", sendData);
    if (device) {
      device
        .writeCharacteristicWithResponseForService(
          this.state.serviceUUID,
          this.state.characteristics_RXID,
          sendData
        )
        .then((characteristic) => {
          console.log("write response");

          // Sent message and start receiving data
          console.log("Service");
          console.log(
            this.state.serviceUUID,
            "Characteristic",
            this.state.characteristics_RXID
          );
        })
        .catch((error) => console.log(error));
    } else {
      console.log("No device is connected");
    }
  }

  // function for write message through ble
  async readMessage() {
    this.manager.cancelTransaction(transactionId);
    var device = this.state.device;
    console.log("Message to read ");
    if (device) {
      device
        .readCharacteristicForService(
          this.state.serviceUUID,
          this.state.characteristics_TXID
        )
        .then((character) => {
          console.log("Read.......");
          console.log(base64.decode(character.value));
          this.setState({
            text_rx: base64.decode(character.value),
          });
        })
        .catch((error) => console.log(error));
    } else {
      console.log("No device is connected");
    }
  }

  // function for write message through ble
  async monitorMessage() {
    this.manager.cancelTransaction(transactionId);
    var device = this.state.device;
    console.log("Message to monitor ");
    if (device) {
      device.monitorCharacteristicForService(
        this.state.serviceUUID,
        this.state.characteristics_NOID,
        (error, characteristic) => {
          if (error) {
            console.log(error);
          }
          if (characteristic) {
            console.log("monitor.......");
            console.log(base64.decode(characteristic.value));
            this.setState({
              text_no: base64.decode(characteristic.value),
            });
          }
        }
      );
    } else {
      console.log("No device is connected");
    }
  }

  // creation of render function to put the components in the screen
  render() {
    return (
      <SafeAreaView style={styles.container}>
        <Text style={styles.title}>Hello SmartBedding</Text>
        <View style={styles.bleContainer}>
          {this.state.deviceid ? (
            <Button title={"Disconnect"} onPress={() => this.disconnect()} />
          ) : (
            <Button title={"Scan"} onPress={() => this.scanAndConnect()} />
          )}
        </View>
        <View style={styles.textBox}>
          <Text>{this.state.text1}</Text>
        </View>
        <View style={styles.listBox}>
          <FlatList
            keyExtractor={(item) => item.id} // funtion for change the key
            data={this.state.devices}
            renderItem={(
              { item } // function to render the items inside the list
            ) => (
              <TouchableOpacity
                onPress={() => this.connectBleDevice(item.key, item.dev)} // touchable propertie with on press function
              >
                <Text style={styles.listText}>{item.name}</Text>
              </TouchableOpacity>
            )}
          />
        </View>
        <View style={styles.textContainer}>
          <Text style={styles.listText}> {this.state.text_rx}</Text>
          <Text style={styles.listText}> {this.state.text_no}</Text>
        </View>
        <View style={styles.fotter}>
          <Button title={"Ack"} onPress={() => this.readMessage()} />
          <Button
            title={"Ris 0"}
            onPress={() => this.writeMessage("Ris 0", "Ris 0 Writted")}
          />
          <Button title={"Ris 1"} onPress={() => this.monitorMessage()} />
        </View>
        {Platform.OS === "ios" && ( // add status bar just for ios component
          <StatusBar
            animated={true}
            backgroundColor="#61dafb"
            barStyle="dark-content"
            showHideTransition="none"
            hidden="Visible"
          />
        )}
      </SafeAreaView>
    );
  }
}

// styles object
const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#fff",
    alignItems: "center",
    justifyContent: "flex-start",
  },
  title: {
    fontSize: 18,
    paddingTop: 25,
  },
  bleContainer: {
    paddingTop: 30,
    alignItems: "center",
    justifyContent: "flex-start",
  },
  textBox: {
    alignItems: "center",
    marginVertical: 10,
  },
  listBox: {
    flex: 3,
    backgroundColor: "#f2f2f2",
    alignItems: "flex-start",
    marginVertical: 10,
    width: 300,
  },
  listText: {
    fontSize: 18,
    alignItems: "center",
    margin: 10,
    width: 280,
  },
  fotter: {
    flex: 1,
    flexDirection: "row",
    alignItems: "flex-end",
    marginBottom: 50,
    width: 280,
    justifyContent: "space-between",
  },
  textContainer: {
    flex: 1,
    flexDirection: "column",
    fontSize: 20,
  },
});
