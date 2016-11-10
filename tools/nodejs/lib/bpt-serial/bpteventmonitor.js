'use strict'

/**
 * @module BPTEventMonitor
 * @copyright Derek Benda 2016
 */

var BPT = require('./bpt.js'); // TODO
var SerialPort = require('serialport');
var util = require('util');
var extend = require('xtend');
var EventEmitter = require('events').EventEmitter;


/**
 * Opens a serial connection to the device and monitors
 * the output looking for specific "btp:event" messages.
 * @class
 * @param {String} port The tty serial port (required)
 * @param {Number} baud Baud rate (optional) Defaults to 9600
 */
var BPTEventMonitor = function (port, baud) {

  EventEmitter.call(this);

  if(!port){
    throw new Error("port is required")
  }

  this.port = port;
  this.baud = baud || 9600;

  this._init();

};
util.inherits(BPTEventMonitor, EventEmitter);

/**
 * Called after calling start()
 * @event module:BPTEventMonitor#onMonitorStart
 */

/**
 * Called after calling stop()
 * @event module:BPTEventMonitor#onMonitorStop
 */

/**
 * Called when an error occured
 * @event module:BPTEventMonitor#onMonitorError
 */

/**
 * Called when data exist on the serial port
 * @event module:BPTEventMonitor#onPublishedEvent
 */

BPTEventMonitor.prototype = extend(EventEmitter.prototype, {

  _init: function(){

    this.serialPort = new SerialPort(this.port, {
      baudrate: this.baud,
      lock: false,
      autoOpen: false,
      parser: this._publishParser()
    });

    this.serialPort.on('open', this._onPortOpen.bind(this));
    this.serialPort.on('close', this._onPortClose.bind(this));
    this.serialPort.on('error', this._onPortError.bind(this));
    this.serialPort.on('data', this._onPortData.bind(this));

  },

  _onPortClose: function(){
    this.emit('onMonitorStop', this);
  },

  _onPortOpen: function(){
    //console.log('_onPortOpen');
    this.emit('onMonitorStart', this);
  },

  _onPortError: function(error){
    //console.log('_onPortError', error);
    this.emit('onMonitorError', this, error);
  },

  _onPortData: function(data){
    var a = data.split("~", 2); // event~data
    this.emit('onPublishedEvent', this, a[0], a[1]);
  },

  start: function(){
    if(!this.serialPort.isOpen()){
      this.serialPort.open();
    }
  },

  stop: function(){
    if(this.serialPort.isOpen()){
      this.serialPort.close();
    }
  },

  /**
   * Finds strings of the form: PUBLISH[event~data]\n in a buffer and
   * when found emits an event containing the parsed out data
   * @return {function} A function parser for SerialPort
   */
  _publishParser: function(){
    var buf = [];
    var foundStart = false;
    var startSeq = "PUBLISH[";

    var endChar = "\n";
    var startIndex = 0;

    return function (emitter, buffer) {
      var b = buffer.toString();

      for(var i = 0; i < b.length; i++){
        var c = b[i];
        //console.log("C=", c, foundStart, startIndex);

        if(!foundStart && c === startSeq.charAt(startIndex++)){
          buf.push(c);

          if(buf.length == startSeq.length){
            foundStart = true;
          }

        }else{
            if(foundStart){

              if(c === endChar){
                // emit event
                var p = buf.join('').replace(startSeq, "");
                p = p.substring(0, p.lastIndexOf("]")) //TODO: error prone

                emitter.emit('data', p);

                foundStart = false
                startIndex = 0;
                buf = [];

              }else{
                buf.push(c); //TODO: add a bound condition?
              }

            }else{
              foundStart = false;
              startIndex = 0;
              buf = [];
            }
        }
      }
    };
  },

});

module.exports = BPTEventMonitor;
