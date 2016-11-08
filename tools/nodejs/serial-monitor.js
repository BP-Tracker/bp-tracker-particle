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



new BPTSerial({ baud: 9600 }, function(error, event, data){
  if(error){
    console.log(chalk.bold.red("! ") + chalk.bold.white(error));
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
    var events = _.invert(BPTSerial.prototype.EVENTS);

    if(events[eventType]){
      return events[eventType] + ' ' + tokens.join(',');
    }
  }else if(event == "bpt:state"){
    var states = _.invert(BPTSerial.prototype.STATES);

    if(states[data]){
      return states[data];
    }
  }
  return data;
}
