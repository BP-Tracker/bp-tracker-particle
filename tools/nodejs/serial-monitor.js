'use strict'

var inquirer = require('inquirer');
var chalk = require('chalk');
var _ = require('lodash');
var BPTSerial = require('./lib/bpt-serial');

/**
 * Set to true to forgo any formatting of the command data
 * @type {Boolean}
 */
var SHOW_RAW_DATA = false;

/**
 * Whether or not to monitor for EVENT_SERIAL_COMMAND events.
 * @type {Boolean}
 */
var SUPPRESS_SERIAL_EVENTS = true;


new BPTSerial({ baud: 9600 }, function(error, event, data){
  if(error){
    console.log(chalk.bold.red("! ") + chalk.bold.white(error));
  }

  if(SUPPRESS_SERIAL_EVENTS && event == "bpt:event"){
    var e = BPTSerial.prototype.EVENTS.EVENT_SERIAL_COMMAND;
    if(data.startsWith(e)){
      return;
    }
  }

  var date = new Date().toISOString();
  date = date.replace(/T/, ' ').replace(/\..+/,'');

  console.log( chalk.bold.green("! ")
    + chalk.bold.white(event) + " "
    + chalk.green( formatEvent(event, data) )
    + chalk.dim(" (" + date + ")") );
});


function formatEvent(event, data){
  if(SHOW_RAW_DATA){
    return data;
  }

  if(event == "bpt:event"){
    var tokens = data.split(',');
    var eventType = tokens.shift();
    var ackRequired = tokens.shift();
    var events = _.invert(BPTSerial.prototype.EVENTS);
    var states = _.invert(BPTSerial.prototype.STATES);
    var d;

    if(events[eventType]){
      d = events[eventType];
      var ar = ackRequired >= 1 ? (' ' + ackRequired + ' ') : ' '; // don't display false ack flag
      d += ar;

      if( eventType == BPTSerial.prototype.EVENTS.EVENT_STATE_CHANGE ){
        var fromState = tokens.shift();
        var toState = tokens.shift();

        d += states[fromState] ? ( states[fromState] ) : (fromState);
        d += '->';
        d += states[toState] ? ( states[toState] ) : (toState);
      }

      return d + tokens.join(',');
    }

  }else if(event == "bpt:state"){
    var states = _.invert(BPTSerial.prototype.STATES);
    if(states[data]){
      return states[data];
    }
  }
  return data;
}
