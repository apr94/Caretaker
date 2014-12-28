
// A short and concise way of creating a XHR object to make GET requests.
// Used through the app to send locations, alerts and battery info

var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
	callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

function locationSuccess(pos) {
    // Construct URL to send location to ubuntu server
    var url = "http://ec2-54-69-236-58.us-west-2.compute.amazonaws.com:8000/caretaker/set_loc?loc="+pos.coords.latitude+","+pos.coords.longitude;

    xhrRequest(url, 'GET', function(responseText) {
	console.log(responseText);

	var dictionary = {
	    "LOC_RESULT": 1
	};

	// Send to Pebble
	Pebble.sendAppMessage(dictionary,
	    function(e) {
		console.log("Loc info sent successfully!");
	    },
	    function(e) {
		console.log("Error sending Loc info!");
	    });
    });
}

// Could not get our location :(
function locationError(err) {
    console.log("Error requesting location!");

    var dictionary = {
	"LOC_RESULT": 0
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
	    function(e) {
		console.log("Loc info sent successfully!");
	    },
	    function(e) {
		console.log("Error sending Loc info!");
	    });
}

// Need location services for this.

function getLoc() {
    navigator.geolocation.getCurrentPosition(
	    locationSuccess,
	    locationError,
	    {timeout: 15000, maximumAge: 60000});
}

// function to update the battery value in the ubuntu server

function getBat(batval){
    var url = "http://ec2-54-69-236-58.us-west-2.compute.amazonaws.com:8000/caretaker/set_battery?bat=Normal,23,"+batval;

    xhrRequest(url, 'GET', function(responseText) {
	console.log(responseText);
    });
}

// function to issue alert in the ubuntu server

function issueAlert(){

    var url = "http://ec2-54-69-236-58.us-west-2.compute.amazonaws.com:8000/caretaker/alert";

    xhrRequest(url, 'GET', function(responseText) {
	console.log(responseText);

	var dictionary = {
	    "ALERT_RESULT": 1
	};

	// Send to Pebble
	Pebble.sendAppMessage(dictionary,
	    function(e) {
		console.log("alert sent successfully!");
	    },
	    function(e) {
		console.log("alert not sent");
	    });
    });
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
	function(e) {
	    console.log("PebbleKit JS ready!");
	    // Get the initial loc
	    getLoc();
	});

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
	function(e) {
	    if (e.payload) {
		// There is some request by the watch to update stocks
		if (e.payload.KEY_ALERT) {
		    console.log("Received request to issue alert.");
		    issueAlert();
		}
		else if (e.payload.KEY_LOC){
		    console.log("Received request to update weather.");
		    getBat(e.payload.KEY_LOC);
		    getLoc();
		}
		else if (e.payload.KEY_BAT){
		    console.log("Received request to set battery level.");
		}
		else {
		    console.log("Received invalid request.");
		}
	    }
	});
