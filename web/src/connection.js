//-----------------------------------------------------------------------
// copyright (c) 2024 Farnsworth Technology
//-----------------------------------------------------------------------

var hostIsConnected = false;
var hostNetworkLoaded = false;
var hostUrl = "/nn";

function connection_connect(func)
{
    connection_send_rest("getStatus", {}, connection_get_status_response, func);
}

function connection_get_status_response(response, func)
{
    if (response.result != 0)
    {
        console.log("getStatus error: " + response.error);
        func(false, {});
    }
    else
    {
        hostIsConnected = true;
        console.log(response.info);
        func(true, response.info);
    }
}

function connection_get_status(func)
{
    connection_send_command("getStatus", {}, connection_get_status_response, func);
}

function connection_get_network_response(response, func)
{
    if (response.result != 0)
    {
        console.log("getNetwork error: " + response.error);
        func(false, {});
    }
    else
    {
        hostNetworkLoaded = true;
        console.log(response.info);
        func(true, response.info);
    }
}

function connection_get_network(func)
{
    connection_send_command("getNetwork", {}, connection_get_network_response, func);
}

function connection_load_network(file, func)
{
    connection_send_command("loadNetwork", { "filename": file }, connection_get_network_response, func);
}

function connection_save_network_response(response, func)
{
    if (response.result != 0)
    {
        console.log("saveNetwork error: " + response.error);
    }
    func(response.result == 0);
}

function connection_save_network(file, func)
{
    connection_send_command("saveNetwork", { "filename": file }, connection_save_network_response, func);
}

function connection_set_network_response(response, func)
{
    if (response.result != 0)
    {
        console.log("setNetwork error: " + response.error);
    }
    func(response.result == 0);
}

function connection_set_network(func)
{
    let network = network_read_config();
    console.log(network);
    connection_send_command("setNetwork", network, connection_set_network_response, func);
}

function connection_get_measurement_response(response, func)
{
    if (response.result != 0)
    {
        console.log("getMeasurement error: " + response.error);
        func({});
    }
    else
    {
        func(response.info);
    }
}

function connection_get_measurement(inputs, func)
{
    connection_send_command("getMeasurement", inputs, connection_get_measurement_response, func);
}

function connection_start_training_response(response, func)
{
    if (response.result != 0)
    {
        console.log("startTraining error: " + response.error);
    }
    func(response.result == 0, response.info);
}

function connection_start_training(data, func)
{
    connection_send_command("startTraining", data, connection_start_training_response, func);
}

function connection_stop_training_response(response, func)
{
    if (response.result != 0)
    {
        console.log("stopTraining error: " + response.error);
    }
    func(response.result == 0);
}

function connection_stop_training(func)
{
    connection_send_command("stopTraining", {}, connection_stop_training_response, func);
}

function connection_get_training_state_response(response, func)
{
    if (response.result != 0)
    {
        console.log("getTrainingState error: " + response.error);
    }
    func(response.info, response.result == 0);
}

function connection_get_training_state(func)
{
    connection_send_command("getTrainingState", {}, connection_get_training_state_response, func);
}

function connection_send_command(cmd, data, respFunc, userFunc)
{
    if (!hostIsConnected) {
        respFunc({ result:-1, error:'not connected to host'}, userFunc);
        return;
    }
    connection_send_rest(cmd, data, respFunc, userFunc);
}

function connection_send_rest(cmd, data, respFunc, userFunc)
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
