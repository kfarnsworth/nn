//-----------------------------------------------------------------------
// copyright (c) 2024 Farnsworth Technology
//-----------------------------------------------------------------------

var hostIsConnected = false;
var hostNetworkLoaded = false;
var hostUrl = "/nn";

function connection_connect(host, func)
{
    connection_get_status(func);
}

function connection_get_status_response(response, func)
{
    func(response.result == 0);
    if (response.result != 0)
    {
        console.log("getStatus error: " + response.error);
    }
    else
    {
        hostIsConnected = true;
        // TODO set inputs and activations
        console.log(response.info);
    }
}

function connection_get_status(func)
{
    connection_send_command("getStatus", {}, connection_get_status_response, func);
}

function connection_get_network_response(response)
{
    if (response.result != 0)
    {
        console.log("getNetwork error: " + response.error);
    }
    else
    {
        hostNetworkLoaded = true;
        console.log(response.info);
        network_save_config(response.info);
    }
}

function connection_get_network()
{
    connection_send_command("getNetwork", {}, connection_get_network_response);
}

function connection_set_network_response(response)
{
    if (response.result != 0)
    {
        console.log("setNetwork error: " + response.error);
    }
}

function connection_set_network()
{
    let network = network_read_config();
    console.log(network);
    connection_send_command("setNetwork", network, connection_set_network_response);
}

function connection_get_measurement_response(response, func)
{
    if (response.result != 0)
    {
        console.log("getMeasurement error: " + response.error);
    }
    else
    {
        func(response.info);
    }
}

function connection_get_measurement(func)
{
    connection_send_command("getMeasurement", measurement_get_input(), connection_get_measurement_response, func);
}


function connection_send_command(cmd, data, respFunc, userFunc)
{
    var xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == 4) {
            if (xmlhttp.status == 200) {
                var data = JSON.parse(xmlhttp.responseText);
                if (data.cmd == cmd) {
                    respFunc(data.response, userFunc);
                }
                else {
                    console.log("Unknown command in response - " + data.cmd);
                }
            }
        }
    }
    xmlhttp.open("POST", hostUrl, true);
    xmlhttp.setRequestHeader("Content-type", "application/json");
    var data = JSON.stringify( {"cmd": cmd, "data": data }
                             );
    xmlhttp.send(data);
}

