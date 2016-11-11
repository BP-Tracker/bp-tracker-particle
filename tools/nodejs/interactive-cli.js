'use strict'

var inquirer = require('inquirer');
var chalk = require('chalk');
var _ = require('lodash');
var BPTSerial = require('./lib/bpt-serial');

var bpt = new BPTSerial({ baud: 9600, monitor: false });

/**
 * The time to wait before showing the main choices
 * @type {Number} Time in ms
 */
var choiceDelay = 500;

var mainChoices = [
  'bpt:ack',
  'bpt:state',
  'bpt:probe',
  'bpt:gps',
  'bpt:status',
  'bpt:register',
  'bpt:diag',
  'bpt:test',
  'bpt:reset',
  'Show options again',
];

var funcMap = {
  'Show options again' : callFunctionPrompt,
  'bpt:ack'            : callAckPrompt,
  'bpt:status'         : callStatusPrompt,
  'bpt:state'          : callStatePrompt,
  'bpt:gps'            : callGpsPrompt,
  'bpt:diag'           : callDiagPrompt,
  'bpt:register'       : callRegisterPrompt,
  'bpt:probe'          : callProbePrompt,
  'bpt:test'           : callTestPrompt,
  'bpt:reset'          : callResetPrompt
};

/**
 * Send any initial commands to the devices (optional)
 */
function runSetupCommands(){
  var d = function(){ };
  var e = function(error){
    console.log("Initial setup error: ", error)
  }

  //TODO: are the commands sent too fast?
  bpt.sendCommand("bpt:status", "" ).then(function(){
    return bpt.sendCommand("bpt:diag", "1" ).then(
      function(){
        return bpt.sendCommand("bpt:state", "" ).done(d, e);
      }, e
    );
  }, e);
}

function reportError(error){
  console.log(chalk.bold.red("! ") + "error " + error);
}

function reportDone(command){
  console.log(chalk.yellow("! " + command));
}

function callRegisterPrompt(){ //TODO
  callFunctionPrompt();
}

function callResetPrompt(){
  inquirer.prompt([{
    type: 'rawlist',
    message: chalk.bold.white('Are you sure you want to reset?'),
    name: 'bpt_opt',
    choices: [
      'Yes',
      'No',
      'Cancel'
    ],
    default: 2
  }]).then(function(ans){
      if(ans.bpt_opt == 'Yes'){
        bpt.sendCommand("bpt:reset", "" ).done(reportDone, reportError);
      }
      setTimeout(callFunctionPrompt, ans.bpt_opt  == "Yes" ? choiceDelay : 0);
  });
}

function callAckPrompt() {
  inquirer.prompt([{
    type: 'rawlist',
    message: chalk.bold.white('Choose an event'),
    name: 'bpt_event_id',
    pageSize: _.keys(bpt.EVENTS).length,
    choices: _.keys(bpt.EVENTS),
  },{
    type: 'input',
    message: chalk.dim("[data1[,data2..]]"),
    name: 'bpt_data',
  }]).then(function(ans){
      var cmd = bpt.EVENTS[ans.bpt_event_id];

      if(ans.bpt_data){
        cmd = cmd + "," + ans.bpt_data;
      }

      bpt.sendCommand("bpt:ack", cmd ).done(reportDone, reportError);
      setTimeout(callFunctionPrompt, choiceDelay);
  });
}

function callTestPrompt(){
  inquirer.prompt([{
    type: 'rawlist',
    message: chalk.bold.white('Choose a test input'),
    name: 'bpt_test_id',
    pageSize: _.keys(bpt.TEST_INPUTS).length,
    choices: _.keys(bpt.TEST_INPUTS),
  },{
    type: 'input',
    message: chalk.dim("[data1[,data2..]]"),
    name: 'bpt_data',
  }]).then(function(ans){
      var cmd = bpt.TEST_INPUTS[ans.bpt_test_id];

      if(ans.bpt_data){
        cmd = cmd + "," + ans.bpt_data;
      }

      bpt.sendCommand("bpt:test", cmd ).done(reportDone, reportError);
      setTimeout(callFunctionPrompt, choiceDelay);
  });
}

function callFunctionPrompt(){

  inquirer.prompt([
  	{
  		type: 'rawlist',
  		name: 'func',
  		message: chalk.bold.white('Select a function to call'),
  		choices: mainChoices,
      pageSize: mainChoices.length,
      default: 9
  	}
  ]).then(function(ans) {
    //console.log('ans', ans);

    if(funcMap[ans.func]){
      funcMap[ans.func]();
    }else{
        callFunctionPrompt();
    }
  });
}

function callGpsPrompt(){
  inquirer.prompt([{
    type: 'rawlist',
    message: chalk.bold.white('Choose an option'),
    name: 'bpt_opt',
    choices: [
      "Get GPS",
      "Send Remote GPS",
      "Cancel"
    ]
  },{
    type: 'input',
    message: chalk.dim(" [deviceNum:]lat,long"),
    name: 'bpt_cmd',
    when: function(ans){
      return ans.bpt_opt === "Send Remote GPS";
    }
  }]).then(function(ans){
    if(ans.bpt_opt === "Send Remote GPS"){
      bpt.sendCommand("bpt:gps", ans.bpt_cmd).done(reportDone, reportError);

    }else if(ans.bpt_opt === "Get GPS"){
      bpt.sendCommand("bpt:gps", "").done(reportDone, reportError);
    }

    setTimeout(callFunctionPrompt, ans.bpt_opt  == "Cancel" ? 0 : choiceDelay);
  });
}

//TODO: logic to pass command data
function callDiagPrompt(){
  bpt.sendCommand("bpt:diag", "1").done(reportDone, reportError);
  setTimeout(callFunctionPrompt, choiceDelay);
}

function callStatusPrompt(){
  bpt.sendCommand("bpt:status", "").done(reportDone, reportError);
  setTimeout(callFunctionPrompt, choiceDelay);
}

function callProbePrompt(){
  inquirer.prompt([{
    type: 'rawlist',
    message: chalk.bold.white('Choose an option'),
    name: 'bpt_opt',
    choices: [
      "Probe controller",
      "Probe properties",
      "Cancel"
    ]
  },{
    type: 'rawlist',
    message: chalk.bold.white('Select a property'),
    name: 'bpt_prop',
    pageSize: _.keys(bpt.APPLICATION_PROPERTIES).length,
    choices: _.keys(bpt.APPLICATION_PROPERTIES),
    when: function(ans){
      return ans.bpt_opt === "Probe properties";
    }
  }]).then(function(ans){
    if(ans.bpt_opt === "Probe properties"){ //TODO: NYI on the controller

      console.log(chalk.yellow("Warning: Not yet implemented on the controller"));

      var cmd = bpt.APPLICATION_PROPERTIES[ans.bpt_prop];
      bpt.sendCommand("bpt:probe", cmd).done(reportDone, reportError);

    }else if(ans.bpt_opt === "Probe controller"){
      bpt.sendCommand("bpt:probe", "").done(reportDone, reportError);
    }

    setTimeout(callFunctionPrompt, ans.bpt_opt  == "Cancel" ? 0 : choiceDelay);
  });
}

function callStatePrompt(){
  inquirer.prompt([{
    type: 'rawlist',
    message: chalk.bold.white('Choose an option'),
    name: 'bpt_state_opt',
    choices: [
      "Get state",
      "Set state",
      "Cancel"
    ]
  },{
    type: 'rawlist',
    message: chalk.bold.white('Choose a state'),
    name: 'bpt_state_id',
    pageSize: _.keys(bpt.STATES).length,
    choices: _.keys(bpt.STATES).filter(
        function(v){
          return !v.startsWith("INTERNAL");
        }
      ),
    when: function(ans){
      return ans.bpt_state_opt === "Set state"
    }
  }]).then(function(ans){
    if(ans.bpt_state_opt === "Set state"){
      bpt.sendCommand("bpt:state", bpt.STATES[ans.bpt_state_id]).done(reportDone, reportError);

    }else if(ans.bpt_state_opt === "Get state"){
      bpt.sendCommand("bpt:state", "").done(reportDone, reportError);

    }

    setTimeout(callFunctionPrompt, ans.bpt_state_opt  == "Cancel" ? 0 : choiceDelay);
  });
}

runSetupCommands();
callFunctionPrompt();
