/**
 * Copyright 2014 IBM Corp. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * Modified by Sayan A Ghosh (sayan.ghosh@in.ibm.com) to support development
 * of a voice control for the Rover Project entered into the
 * Watson Challenge 2015
 */

'use strict';

var bluemix = require('./config/bluemix');
var SpeechToText = require('./speech-to-text');
var extend = require('util')._extend;
var fs = require('fs');

// if bluemix credentials exists, then override local
var credentials = extend({
  username: '<username>',
  password: '<password>'
}, bluemix.getServiceCreds('speech_to_text')); // VCAP_SERVICES


// Create the service wrapper
var speechToText = new SpeechToText(credentials);

console.log('Testing...');
var sessions = [];
//console.log(speechToText.create_session({}, function (err, session) {
speechToText.create_session({}, function(err, session) {
  if (err) {
    console.log('The server could not create a session');
  } else {
    sessions[0] = session;
    console.log('created session');
    console.log('The system has:', sessions.length, 'sessions.', sessions[0].session_id);
  }
});

var audio = fs.createReadStream(process.argv[2]);
var fdoptions = { flags: 'a+' };

var transcript = fs.createWriteStream(process.argv[3], fdoptions);
speechToText.recognize({
  audio: audio
}, function(err, response) {
  if (err)
    console.log('500 - Error');
  else {
    console.log('Transcript:', response);
    if (typeof(response.results[0]) !== 'undefined') {
      console.log('Received transcript:', response.results[0].alternatives[0].transcript);
      transcript.write(response.results[0].alternatives[0].transcript + '\n');
    } else {
      console.log('Cannot convert');
    }
  }
  shutDownSession(sessions[0]);
  delete sessions[0];
});


function shutDownSession(session) {
  console.log('Closing session');
  speechToText.delete_session(session, function() {
    console.log('Session deleted');
  });
}