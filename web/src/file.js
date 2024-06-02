//-----------------------------------------------------------------------
// copyright (c) 2024 Farnsworth Technology
//-----------------------------------------------------------------------

var filenameActive = "";
var filenameCallback = false;

function file_open(file, whendoneFunc) {
    var reader = new FileReader();
    reader.onload = function() {
        deserialize(reader.result, file.name, file.size)
    };
    reader.onloadend = function() {
        whendoneFunc();
    }
    reader.readAsArrayBuffer(file);
}

function file_load(dataBlob, filename, whendoneFunc) {
    var reader = new FileReader();
    reader.onload = function() {
        deserialize(reader.result, filename, dataBlob.size);
    };
    reader.onloadend = function() {
        whendoneFunc();
    }
    reader.readAsArrayBuffer(dataBlob);
}

function file_get_filename(filename_suggest, callback)
{
    $("#filename-entered").val(filename_suggest);
    filenameCallback = callback;
    $('#filename-modal').modal('show');
}


async function save_with_handle(handle, blob)
{
    const writable = await handle.createWritable();
    await writable.write(blob);
    await writable.close();
}

function file_savefile(filename, completeCallback)
{
    if (window.showSaveFilePicker) {
        var buffer = serialize(filename);
        var blob = new Blob([buffer], {type:"text/plain"});

        const opts = {
            suggestedName: filename,
            types: [{
            description: 'Network Configuration',
            accept: {'text/plain': ['.json']},
            }],
        };
        window.showSaveFilePicker(opts).then(handle => { 
            save_with_handle(handle, blob);
            if (completeCallback)
                completeCallback();
            filenameActive = filename;
        });
    }
    else {
        file_get_filename(filename, function(filename) {
            var buffer = serialize(filename);
            var blob = new Blob([buffer], {type:"text/plain"});

            var saveURL = window.URL.createObjectURL(blob);
            var downloadLink = document.createElement("a");
            downloadLink.download = filename;
            downloadLink.innerHTML = "Download File";
            downloadLink.href = saveURL;
            downloadLink.onclick = (function(event) {
                document.body.removeChild(event.target);
            });
            downloadLink.style.display = "none";
            document.body.appendChild(downloadLink);
    
            downloadLink.click();
            if (completeCallback)
                completeCallback();
            filenameActive = filename;
        });
    }
}

function deserialize(buffer, filename, filesize)
{
    try {
        var s = String.fromCharCode.apply(null, new Uint8Array(buffer));
        var config = JSON.parse(s);
        network_load_config(config["network"]);
    } catch (err) {
        console.log(err.message);
    }
}

function serialize(filename)
{
    var network = {};
    network["inputs"] = network_get_inputs();
    network["outputs"] = network_get_outputs();
    let layers = [];
    for (var i=0; i<network_get_layers(); i++)
    {
        let layer = {};
        layer["nodes"] = [];
        let nodeCnt = network_get_nodes(i);
        let nodes = [];
        for (var j=0; j<nodeCnt; j++)
        {
            let biases = network_get_node_biases(i, j);
            let weights = network_get_node_weights(i, j);
            if (biases && weights)
            {
                let node = {};
                node["biases"] = biases;
                node["weights"] = weights;
                nodes.push(node);
            }
            layer["nodes"].push(nodes);
        }
        layers.push(layer);
    }
    network["layers"] = layers;

    var measurement = {};

    var training = {};
    
    let config = {};
    config["network"] = network;
    config["measurement"] = measurement;
    config["training"] = training;

    return JSON.stringify(config);
}

///////////////////////////////////////////////////////////////////////
function file_save_now(filename)
{
    if (filenameCallback)
    {
        filenameCallback(filename);
        filenameCallback = false;
    }
}

function file_network_save(completeCallback=false)
{
    if (filenameActive.length > 0)
        file_savefile(filenameActive, completeCallback);
    else
        file_network_saveas(completeCallback);
}

function file_network_saveas(completeCallback=false)
{
    file_savefile("networkconfig.json", completeCallback);
}
