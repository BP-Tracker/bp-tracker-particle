'use strict'

var inquirer = require('inquirer');
var chalk = require('chalk');
var _ = require('lodash');
var BPTSerial = require('./lib/bpt-serial');

var bpt = new BPTSerial({ baud: 9600, monitor: false });


/**
 * Reports the result code of the API function returned from
 * the controller
 * @return {[type]} [description]
 */
function reportFunctionResp(){



}

function reportError(error){
  console.log("error " + error);

}

function callFunctionPrompt(){

  inquirer.prompt([
  	{
  		type: 'rawlist',
  		name: 'func',
  		message: chalk.bold.white('Select a function to call'),
  		choices: [
        "Show options again",
        'bpt:ack',
        'bpt:status',
        'bpt:state',
        'bpt:gps',
        'bpt:diag',
        'bpt:register',
        'bpt:probe',
        'bpt:test'
      ],
      default: 0
  	}
  ]).then(function(ans) {
    console.log('ans', ans);

    if(ans.func === "Show options again"){
      callFunctionPrompt();
    }else if(ans.func === 'bpt:state'){
      callStatePrompt();
    }

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
    choices: _.keys(bpt.STATES),
    when: function(ans){
      return ans.bpt_state_opt === "Set state"
    }
  }]).then(function(ans){
    if(ans.bpt_state_opt === "Cancel"){
      callFunctionPrompt();
    }else if(ans.bpt_state_opt === "Get state"){
      bpt.sendCommand("bpt:state", "").then(function(res){
        //console.log("resp=[" + res + "]");
      }, reportError);

    }else{
      //TODO
    }

  });
}

callFunctionPrompt();
