var json_file_url = 'http://willwinder.com/baby';

function stringToTimestamp(datetime) {
  var time=datetime.split(" ");
  var date=time[0];
  time=time[1].split(":");
  var seconds = time[0] * 60 * 60 + time[1] * 60;
  return (new Date(date).getTime()/1000) + seconds;
}

function updateWatch(asleep, lastSleeping, lastBottle, lastNursing, lastDiaper, lastPumping) {
  var sleep = 0;
  if (asleep === 'true') {
    sleep = 1;
  }
  
  // Split up the sends to avoid buffer overflow
  Pebble.sendAppMessage({
      "IS_SLEEPING":sleep,
      "LAST_SLEEPING":stringToTimestamp(lastSleeping),
      "LAST_BOTTLE":stringToTimestamp(lastBottle),
      "LAST_NURSING":stringToTimestamp(lastNursing),
      "LAST_DIAPER":stringToTimestamp(lastDiaper),
      "LAST_PUMPING":stringToTimestamp(lastPumping),
    },  function(e) {
      console.log('Successfully delivered message with transactionId=' + e.data.transactionId);
    }, function(e) {
      console.log('Unable to deliver message with transactionId='+ e.data.transactionId + ' Error is: ' + e.error.message);
    }
  );
}

function processStatusJson(json) {
  var obj = JSON.parse(json);
  var summary = obj.summaries[0];
  console.log("summary:\n" + JSON.stringify(summary));

  console.log("Is Sleeping: " + summary.isSleeping);
  console.log("Last sleep  : " + summary.timeOfLastSleeping);
  console.log("Last bottle : " + summary.timeOfLastBottle);
  console.log("Last nursing: " + summary.timeOfLastNursing);
  console.log("Last diaper : " + summary.timeOfLastDiaper);
  console.log("Last pumping: " + summary.timeOfLastPumping);
  
  updateWatch(JSON.stringify(summary.isSleeping),
              summary.timeOfLastSleeping,
              summary.timeOfLastBottle,
              summary.timeOfLastNursing,
              summary.timeOfLastDiaper,
              summary.timeOfLastPumping);
  /*
  Pebble.sendAppMessage({
    "IS_SLEEPING":"1",
    "LAST_FEEDING":"2",
    "LAST_BOTTLE":"3",
    "LAST_NURSING":"4",
    "LAST_DIAPER":"5",
    "LAST_PUMPING":"6",
  },  function(e) {
    console.log('Successfully delivered message with transactionId=' + e.data.transactionId);
  },
  function(e) {
    console.log('Unable to deliver message with transactionId='+ e.data.transactionId + ' Error is: ' + e.error.message);
  }
  );
  */
}

function logWithPrefix(prefix, msg) {
  console.log('(' + prefix + ') ' + msg);
}

function requestHandler(response, num) {
  try {
    logWithPrefix(num, "requestHandler callback");
    logWithPrefix(num, "Ready State = " + response.readyState);
    logWithPrefix(num, "Status = " + response.status);
    if (response.readyState == 4 && response.status == 200) {
      logWithPrefix(num, 'Good response.');
      logWithPrefix(num, 'response type:' + response.responseType);
      logWithPrefix(num, "response:\n" + response.responseText);
      
      processStatusJson(response.responseText);
    } else {
      logWithPrefix(num, response.readyState + ' state and ' + response.status + ' status onreadystatechange event ' + response.responseText);
    }
  }
  catch(err) {
    logWithPrefix(num, "There was an error: " + err);
  }
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    var req = new XMLHttpRequest();
    req.open("GET", json_file_url, true);
    req.onreadystatechange = function(){ requestHandler(req, 1); };
    req.send();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
  }                     
);

/*
var URL_BASE="http://www.baby-connect.com";
var URL_AUTH="/Cmd?cmd=UserAuth";
var URL_MAIN="/home";

function logWithPrefix(prefix, msg) {
  console.log('(' + prefix + ') ' + msg);
}

function requestHandler(response, num) {
  try {
    logWithPrefix(num, "requestHandler callback");
    logWithPrefix(num, "Ready State = " + response.readyState);
    logWithPrefix(num, "Status = " + response.status);
    if (response.readyState == 4 && response.status == 200) {
      logWithPrefix(num, 'Good response.');
      logWithPrefix(num, 'response type:' + response.responseType);
      //logWithPrefix(num, "response:\n" + JSON.stringify(response));
  
      if (num === 1) {
        var url = URL_BASE + URL_MAIN;
        logWithPrefix(num, 'Sending GET request to: ' + url);
        var req2 = new XMLHttpRequest();
        req2.open("POST", url, true);
        req2.onreadystatechange = requestHandler(req2, 2);
        //req2.withCredentials = true;
        req2.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        var params = "email=<email>&pass=<password>";

        req2.send(params);
      }
    } else {
      logWithPrefix(num, response.readyState + ' state and ' + response.status + ' status onreadystatechange event ' + response.responseText);
    }
  }
  catch(err) {
    logWithPrefix(num, "There was an error: " + err);
  }
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');

    var authurl = URL_BASE + URL_AUTH;
    console.log("Auth URL = " + authurl);
    var params       = "email=<email>&pass=<password>";
    
    var req = new XMLHttpRequest();

    // POST works.
    // With content header
    // http://www.w3schools.com/ajax/ajax_xmlhttprequest_send.asp
    req.open("POST", authurl, true);
    req.onreadystatechange = function(){ requestHandler(req, 1); };
    req.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    console.log('Sending POST request');
    req.send(params);
    
    // POST works.
    // Full content header
    // http://stackoverflow.com/questions/9713058/sending-post-data-with-a-xmlhttprequest
    //req.open("POST", authurl, true);
    //req.onreadystatechange = function(){ requestHandler(req, 1); };
    //req.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    //req.setRequestHeader("Content-length", params.length);
    //req.setRequestHeader("Connection", "close");
    //console.log('Sending POST request');
    //req.send(params);

    // Doesn't work, parameters not received by target.
    // Cookie
    //params.replace('&',';');
    //req.open("POST", authurl, true);
    //req.onreadystatechange = function(){ requestHandler(req, 1); };
    //req.setRequestHeader("Cookie",params);
    //console.log('Sending POST request');
    //req.send();
    
    // Doesn't work, parameters not received by target.
    // open with credentials
    //req.open("POST", authurl, true, '<email>', '<password>');
    //req.onreadystatechange = function(){ requestHandler(req, 1); };
    //console.log('Sending POST request');
    //req.send();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
  }                     
);
*/
