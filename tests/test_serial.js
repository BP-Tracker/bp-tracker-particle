'use strict'

console.log("HELLO WORLD");
var SerialPort = require('serialport');

SerialPort.list(function(err, ports){
  ports.forEach(function(port){
    console.log(port.comName);
    console.log(port.pnpId);
    console.log(port.manufacturer);
  });
});
// var SerialPort = SerialPortLib.SerialPort;
//
//
// var serialPort = new SerialPort(device.port, { baudrate: 28800 }, false);
