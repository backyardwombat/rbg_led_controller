var app = {};
var device_addresses = [];
var selected_device_address;
var selected_animation=-1;

app.device = {};
app.device.ADV_NAME              = 'PSoC animation';
app.device.ANIMATION_SERVICE     = 'e95d7170-251d-470a-a062-fa1922dfa9a8';
app.device.ANIMATION_TYPE        = 'e95dc306-251d-470a-a062-fa1922dfa9a8';
app.device.ANIMATION_STATUS      = 'e95d4592-251d-470a-a062-fa1922dfa9a8';
app.device.ANIMATION_CONTROL     = 'e95db84c-251d-470a-a062-fa1922dfa9a8';

var BLE_NOTIFICATION_UUID = '00002902-0000-1000-8000-00805f9b34fb';

var found_my_device = false;
var connected = false;
var odd = true;
var info_hidden = true;

app.findDevices = function() {
    app.clearDeviceList();
    app.startScan();
}

app.startScan = function()
{

    console.log("startScan");

    //TODO create onDeviceFound function
    function onDeviceFound(peripheral) {
        console.log("Found " + JSON.stringify(peripheral));
        if (app.isMyDevice(peripheral.name)) {
            found_my_device = true;
            app.showDiscoveredDevice(peripheral.id, peripheral.name);
        }
    }

    //TODO create scanFailure 
    function scanFailure(reason) {
        alert("Scan Failed: "+JSON.stringify(reason));
    }

    //TODO start scanning for devices for 5 seconds
    // service list, seconds to scan, on found, on failed
    ble.scan([], 10, onDeviceFound, scanFailure);
    showMessage("Scanning....");

    //TODO check outcome of scanning after 5 seconds have elapsed using a timer
    setTimeout(function() {
        showMessage("");
        if (!found_my_device) {
            alert("Could not find your device");
        }
    }, 5000);

};

app.isMyDevice = function(device_name)
{
    console.log("isMyDevice("+device_name+")");
    //TODO implement device name matching
    if (device_name == null) {
        return false;
    }
    console.log('device name: ' + device_name);
    return (device_name == app.device.ADV_NAME);
};

app.connectToSelected = function(address, name) {
    console.log("connectToSelected("+address+","+name+")");
    selected_device_address = address;
    app.connectToDevice(selected_device_address);
};

app.connectToDevice = function(device_address)
{
	showInfo('Connecting to '+device_address);

    //TODO start timer to check whether or not a connection was established
    connected = false;
    setTimeout(function() {
        if (!connected) {
            alert("Could not connect to selected device");
        }
        }, 5000);

    //TODO connect to the selected device
    ble.connect(device_address,
    //TODO implement actions to take when a connection was successfully established
    function(peripheral)
    {
        connected = true;
        console.log("connected");
        selected_device_address = device_address;
        // check we have the animation service
        if (app.hasService(peripheral,app.device.ANIMATION_SERVICE)) {
            app.showControls();
            showInfo("connected",0);
        } else {
            showInfo("ERROR: device does not have the Bluetooth Animation Service");
        }
    },
    function(peripheral)
    {
        // called if ble.connect fails OR if an established connection drops
        if (!connected) {
            console.log('Error: Connection failed');
            console.log(JSON.stringify(peripheral));
            alert("Error: could not connect to selected device");
        } else {
            connected = false;
            alert("No longer connected to selected device");
            app.disconnected();
        }
    });

    //TODO implement actions to take when a connection was successfully established

    //TODO implement actions to take when a connection was not established or was dropped

};

app.enableStatusNotifications = function() {
    console.log("enableStatusNotifications");

    //TODO start animation status notifications
    ble.startNotification(selected_device_address, app.device.ANIMATION_SERVICE, app.device.ANIMATION_STATUS, 
        function(buffer) {
            var data = new Uint8Array(buffer);
            console.log("animation status: "+data[0]);
            app.displayAnimationStatus(data[0]);
        },
        function(e) {
            console.log("Error handling notification:"+e);
        });
};

app.stopNotifications = function(device_address, service_uuid, characteristic_uuid)
{
    console.log("stopping notifications");
    //TODO switch off notifications
    ble.stopNotification(device_address,service_uuid,characteristic_uuid);
};

app.setUiCurrentStatus = function() {
    console.log("setUiCurrentStatus");
    //TODO read the current animation status value and use it to initialise the UI state
    ble.read(selected_device_address, app.device.ANIMATION_SERVICE, app.device.ANIMATION_STATUS, 
        function(data) {
            console.log("read current animation status OK");
            var animation_status_data = new Uint8Array(data);
            if (animation_status_data.length > 0) {
                console.log("animation_status="+animation_status_data[0]);
                app.displayAnimationStatus(animation_status_data[0]);
            }
        },
        function(err) {
            console.log("ERROR: reading current animation type: "+err);
        });
}

app.setUiCurrentAnimationType = function() {
    console.log("setUiCurrentAnimationType");
    //TODO read the current animation type value and use it to initialise the UI state
    ble.read(selected_device_address, app.device.ANIMATION_SERVICE, app.device.ANIMATION_TYPE, 
        function(data) {
            console.log("read current animation type OK");
            var animation_type_data = new Uint8Array(data);
            if (animation_type_data.length > 0) {
                console.log("animation_type="+animation_type_data[0]);
                app.changeAnimationSelection(animation_type_data[0]);
            }
        },
        function(err) {
            console.log("ERROR: reading current animation type: "+err);
        });
}

app.setAnimationType = function(animation_id, success_cb, error_cb) {
    console.log("setAnimationType:"+animation_id);
    rc = 0;
    var animation_type_data = new Uint8Array(1)
    animation_type_data[0] = animation_id;
    ble.write(
        selected_device_address, 
        app.device.ANIMATION_SERVICE, 
        app.device.ANIMATION_TYPE, 
        animation_type_data.buffer, 
        function() {
            console.log("written animation type OK");
            success_cb();
        },
        function() {
            console.log("ERROR: writing animation type");
            error_cb();
        });
};

app.exitControls = function() {
    //TODO stop notifications and disconnect 
    if (connected) {
        app.stopNotifications(selected_device_address, app.device.ANIMATION_SERVICE, app.device.ANIMATION_STATUS);
        showInfo("Disconnecting");
        ble.disconnect(
            selected_device_address,
            function() {
                console.log("disconnected OK");
                showInfo("");
                app.showDeviceList();
            }, 
            function(error) {
                console.log("disconnect failed: "+error);
                showInfo("Failed to disconnect");
                app.showDeviceList();
            });
    } else {
        showInfo("");
        app.showDeviceList();
    }
};

app.showDiscoveredDevice = function(address, name) {
    console.log("showDiscoveredDevice("+address+","+name+")");
    var tbl = document.getElementById("tbl_devices");
    if (tbl != undefined) {
        var row_count = tbl.rows.length;
        var rows = tbl.rows;
        var new_row = tbl.insertRow(row_count);
        var new_cell = new_row.insertCell(0);
        var button_class;
        if (odd) {
            button_class = "wide_button_odd";
        } else {
            button_class = "wide_button_even";
        }
        odd = !odd;
        var btn_connect = "<button class=\""+button_class+"\" onclick=\"app.connectToSelected(\'"+address+"\',\'"+name+"\')\">"+name+"<br>"+address+"</button>";
        new_cell.innerHTML =  btn_connect;
    }
};

app.clearDeviceList = function() {
    var tbl = document.getElementById("tbl_devices");
    tbl.innerHTML = "";
}

app.showDeviceList = function() {
    device_list_hidden = false;
    controls_hidden = true;
    message_hidden = false;
    setDivVisibility();
};

app.hasService = function(peripheral, service_uuid) {
    console.log("hasService("+peripheral+","+service_uuid+")");
    var services = peripheral.services;
    for (var i=0;i<services.length;i++) {
        if (services[i].toLowerCase() == service_uuid) {
            return true;
        }
    }
    return false;
};

app.disconnected = function() {
    connected = false;
    displayConnectionStatus();
};

app.showControls = function() {
    console.log("showControls");
    device_list_hidden = true;
    controls_hidden = false;
    message_hidden = true;
    setDivVisibility();
    document.getElementById('message').hidden = message_hidden;
    app.setUiCurrentAnimationType(); 
    app.setUiCurrentStatus();
    app.enableStatusNotifications();
};

app.changeAnimationSelection = function(animation_id) {
    console.log("changeAnimationSelection: "+animation_id);
    if (animation_id < 1 || animation_id > 3) {
        // default to the first animation type
        console.log("Defaulting to animation #1");
        animation_id = 1;
    }
    console.log('btn_animation_'+animation_id);
    var elem = document.getElementById('btn_animation_'+animation_id);
    elem.className = "control_button_selected";
    if (selected_animation > -1 && selected_animation != animation_id) {
        var elem_prev = document.getElementById('btn_animation_'+selected_animation);
        elem_prev.className = "control_button";
    }
    selected_animation = animation_id;
}

app.selectAnimationType = function(animation_id) {
    console.log("sending animation type "+animation_id);
    app.setAnimationType(
        animation_id,
        function () {
            app.changeAnimationSelection(animation_id);
        },
        function () {
            alert("Error: could not send animation type selection to device");
        }
        );
}

app.selectControl = function(control_id) {
    console.log("selectControl:"+control_id);
    //TODO write selected value to the animation control characteristic 
    var control_data = new Uint8Array(1)
    control_data[0] = control_id;
    ble.write(
        selected_device_address, 
        app.device.ANIMATION_SERVICE, 
        app.device.ANIMATION_CONTROL, 
        control_data.buffer, 
        function() {
            console.log("written control OK");
        },
        function() {
            console.log("ERROR: writing control");
            alert("Error: could not send control selection to device");
        });
};

app.displayAnimationStatus = function(status) {
    if (status == 1) {
        showAnimationStatus("on",0);
    } else if (status == 0) {
        showAnimationStatus("off",2);
    }
}

app.initialize = function()
{
	document.addEventListener(
		'deviceready',
		function() { 
          document.addEventListener("backbutton", app.onBackKeyDown, false);
          ready();
        },
		false);
};

app.onBackKeyDown = function() {
  if (controls_hidden == false ) {
      app.exitControls();
      return;
  }
  if (device_list_hidden == false) {
      showMessage("Exiting application",2);
      setTimeout(function() { 
          navigator.app.exitApp(); 
        }, 
        500);
  }
}

// Initialize the app.
app.initialize();
