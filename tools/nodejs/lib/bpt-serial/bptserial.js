'use strict'

/**
 * @module BPTSerial
 * @copyright Derek Benda 2016
 */

var BPT = require('./bpt.js');
var BPTEventMonitor = require('./bpteventmonitor.js');
var SerialPort = require('serialport');

var _ = require('lodash');
var extend = require('xtend');
var util = require('util');
var chalk = require('chalk');
var when = require('when');
var path = require('path');
var specs = require('./particledevicespecs.js');

/**
 * Send and receive cloud functions via the serial interface
 * @class
 * @param {object} options          configuration options:
 *                                  timeout: the timeout on a write. Defaults to 5000 ms
 *                                  baud: baud rate | 9600
 *                                  port: the tty serial port
 *                                  coreType: the type of core for ex. Electron
 *                                  captureAllData: true|false true to show all data
 *                                    captured on the interface. Default is false.s
 * @param {funcion} bptEventCallback callback called when a btp:event is captured on
 *                                    the serial interface (optional)
 */
var BPTSerial = function (options, bptEventCallback) {

  this.defaults = {
      baud: 9600, // TODO: is the data rate ignored when connecting with a USB?
      captureAllData: false, //TODO: implement
      coreType: 'unknown',
      timeout: 5000,
      monitor: true
  };
	this.options = extend(this.defaults, options);
  this.btpEventCallback = bptEventCallback || function(){};

  // Using baud of 14400 will place core into DFU mode and 28800
  // into a YMODEM protocol file transfer. Do not allow these.
  if(this.options.baud == 14400 || this.options.baud == 28800 ){
    console.warn("Cannot use the reserved baud rate "
      + this.options.baud + ", using default instead");

    this.options.baud = this.defaults.baud;
  }

	this._init();

  process.on('SIGINT', this._terminate.bind(this));
	process.on('SIGTERM', this._terminate.bind(this));

};
util.inherits(BPTSerial, BPT);

BPTSerial.prototype = extend(BPT.prototype, {

  /**
   * Send a command to the core
   * @param  {String} eventName the name of the event
   * @param  {String} data      data
   * @return {Promise}          a promise object
   */
  sendCommand: function(eventName, data){
    //console.log('sendCommand called');

    return this.getDevice().then( function(device){
      var cmd = "CALL[" + eventName + "~" + data + "]\n";
      return this._issueSerialCommand(device, cmd);
    }.bind(this) );
  },


  _init: function(){
    //console.log("init called");

    this.getDevice().then(function(device){

      this.device = device;

      var p = path.join(__dirname, "bpteventmonitor.js");

      if(this.options.monitor){
        console.log('Monitoring on: ' + device.port);

        var monitor = new BPTEventMonitor(device.port, device.baud);

        monitor.on('onMonitorStart', this._onMonitorStart.bind(this));
        monitor.on('onMonitorStop', this._onMonitorStop.bind(this));
        monitor.on('onMonitorError', this._onMonitorError.bind(this));
        monitor.on('onPublishedEvent', this._onPublishedEvent.bind(this));

        monitor.start();
        this.monitor = monitor;
      }

    }.bind(this), this._throwException);

  },

  _onMonitorError: function(monitor, error){
    this._throwException(error);
  },

  _onMonitorStart: function(monitor){
    //console.log("_onMonitorStart");
  },

  _onMonitorStop: function(monitor){
    //console.log("_onMonitorStop");
  },

  _onPublishedEvent: function(monitor, eventName, data){
    this.btpEventCallback(null, eventName, data);
  },

  //credit: https://github.com/spark/particle-cli
  //console.log("_issueSerialCommand", device, command);
  _issueSerialCommand: function(device, command) {

    if (!device) {
      return when.reject('no serial port provided');
    }
    var failDelay = this.timeout || 5000;

    var serialPort;
    return when.promise(function (resolve, reject) {
      serialPort = new SerialPort(device.port, {
        baudrate: device.baud || 9600,
        autoOpen: false
      });

      var failTimer = setTimeout(function () {
        reject('Serial timed out');
      }, failDelay);

      //FIXME: not working
      /*
      serialPort.on('data', function (data) {
        //console.log("FDFSFDF", data.toString());
        clearTimeout(failTimer);
        resolve(data);
      });
      */

      serialPort.open(function (err) {
        if (err) {
          //console.error('Serial err: ' + err);
          //console.error('Serial problems, please reconnect the device.');
          reject('Serial problems, please reconnect the device.');
          return;
        }

        serialPort.write(command, function (werr) {
          serialPort.drain(function(werr){
            if(werr){
              reject(werr);
              return;
            }
            clearTimeout(failTimer);
            resolve(command);
          });
        });
      });
    }).finally(function () {
      if (serialPort) {
        serialPort.removeAllListeners('open');
        serialPort.removeAllListeners('data');
        if (serialPort.isOpen()) {
          serialPort.close();
        }
      }
    });
  },

  // credit: https://github.com/spark/particle-cli
  // returns a promise object
  getDevice: function () { //TODO: refactor
		var devices = [];

    if(!this.device && this.options && this.options.port){
      this.device = {
        port: this.options.port,
        coreType: this.options.coreType ? this.options.coreType : "unknown",
        baud: this.options.baud
      };
    }

    if(this.device){
      return when.resolve(this.device);
    }

    return when.promise(function(resolve, reject, notify){
      SerialPort.list(function (err, ports) {

  			if (err) {
          reject('Error listing serial ports: ' + err);
          return;
  			}

  			ports.forEach(function (port) {
  				// manufacturer value
  				// Mac - Spark devices
  				// Devices on old driver - Spark Core, Photon
  				// Devices on new driver - Particle IO (https://github.com/spark/firmware/pull/447)
  				// Windows only contains the pnpId field

  				var device;
  				var serialDeviceSpec = _.find(specs, function (deviceSpec) {
  					if (!deviceSpec.serial) {
  						return false;
  					}
  					var vid = deviceSpec.serial.vid;
  					var pid = deviceSpec.serial.pid;
  					var serialNumber = deviceSpec.serial.serialNumber;

  					var usbMatches = (port.vendorId === '0x' + vid.toLowerCase()
              && port.productId === '0x' + pid.toLowerCase());

  					var pnpMatches = !!(port.pnpId
              && (port.pnpId.indexOf('VID_' + vid.toUpperCase()) >= 0)
              && (port.pnpId.indexOf('PID_' + pid.toUpperCase()) >= 0));

  					var serialNumberMatches = port.serialNumber
              && port.serialNumber.indexOf(serialNumber) >= 0;

  					if (usbMatches || pnpMatches || serialNumberMatches) {
  						return true;
  					}
  					return false;
  				});
  				if (serialDeviceSpec) {
  					device = {
  						port: port.comName,
  						type: serialDeviceSpec.productName
  					};
  				}

  				var matchesManufacturer = port.manufacturer
            && (port.manufacturer.indexOf('Particle') >= 0
              || port.manufacturer.indexOf('Spark') >= 0
              || port.manufacturer.indexOf('Photon') >= 0);

  				if (!device && matchesManufacturer) {
  					device = { port: port.comName, type: 'Core' };
  				}

  				if (device) {
  					devices.push(device);
  				}
  			});

  			//if I didn't find anything, grab any 'ttyACM's
  			if (devices.length === 0) {
  				ports.forEach(function (port) {
  					//if it doesn't have a manufacturer or pnpId set, but it's a ttyACM port, then lets grab it.
  					if (port.comName.indexOf('/dev/ttyACM') === 0) {
  						devices.push({ port: port.comName, type: '' });
  					} else if (port.comName.indexOf('/dev/cuaU') === 0) {
  						devices.push({ port: port.comName, type: '' });
  					}
  				});
  			}

        //reject if more than one device is found
        if( devices.length > 1 ){
          reject("More than one particle device discovered."
               + " Please specify the port in the configuration.");
        }else if( devices.length == 0){
          reject("No particle devices discovered.");
        } else {
          //this.device = devices[0];
          devices[0].baud = 9600; //TODO
          resolve(devices[0]);
        }

  		});
    });
	},

  _throwException: function (str, exit) {
		if (!str) {
			str = 'Unknown error';
		}

		console.error();
		console.error(chalk.bold.red('!'), chalk.bold.white(str));
		if (exit || exit === undefined) {
			process.exit(1);
		}
	},

  _terminate: function() {
    if(this.monitor){
      this.monitor.stop();
    }
    process.exit(0);
  }

});

module.exports = BPTSerial;
