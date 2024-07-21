//-----------------------------------------------------------------------
// copyright (c) 2024 Farnsworth Technology
//-----------------------------------------------------------------------

const DEFAULT_INPUTS = 32;
const DEFAULT_OUTPUTS = 4;
const DEFAULT_NODES = DEFAULT_OUTPUTS;
const DEFAULT_BIAS = -1.0;

var layerCopiesList = [];

const nodeInfo = {
    bias: 0.0,
    weights: []
};

const layerInfo = {
    inputs: 0,
    nodes: [],
    def_bias: DEFAULT_BIAS
};

var layerList = [];

function network_init()
{
    network_new();
    //undo_init();
    //head_zoom_update(1.0);
}

function network_new()
{
    $('#networkInputs').val(DEFAULT_INPUTS);
    $('#networkOutputs').val(DEFAULT_OUTPUTS);
    $('#layer-list').empty();
    $('#layer-list').append(`<option value="1">Layer1</option>`);
    $('#layerNodes').val(DEFAULT_NODES);
    $('#layerDefaultBias').val(DEFAULT_BIAS);
    layerList = []
    var layer = structuredClone(layerInfo);
    layer.inputs = DEFAULT_INPUTS;
    layer.def_bias = DEFAULT_BIAS;
    for (var i=0; i<DEFAULT_NODES; i++)
    {
        var node = structuredClone(nodeInfo);
        for (var j=0; j<DEFAULT_INPUTS; j++)
        {
            node.bias = DEFAULT_BIAS;
            node.weights.push(network_get_random_weight(DEFAULT_INPUTS));
        }
        layer.nodes.push( node );

    }
    layerList.push(layer);
    network_update();
}

function network_get_inputs(layerIx=0)
{
    if (layerIx < 0 || layerIx >= layerList.length)
        return 0;
    return layerList[layerIx].inputs;
}

function network_get_outputs()
{
    var lastLayerxIx = network_get_layers() - 1;
    if (lastLayerxIx >= 0)
        return layerList[lastLayerxIx].nodes.length;
}

function network_get_layers()
{
    return layerList.length;
}

function network_get_nodes(layerIx)
{
    if (layerIx < 0 || layerIx >= layerList.length)
        return 0;
    return layerList[layerIx].nodes.length;
}

function network_get_node_bias(layerIx, nodeIx)
{
    if (layerIx < 0 || layerIx >= layerList.length)
        return 0;
    if (nodeIx < 0 || nodeIx >= layerList[layerIx].nodes.length)
        return 0;
    return layerList[layerIx].nodes[nodeIx].bias;
}

function network_get_node_weights(layerIx, nodeIx)
{
    if (layerIx < 0 || layerIx >= layerList.length)
        return 0;
    if (nodeIx < 0 || nodeIx >= layerList[layerIx].nodes.length)
        return 0;
    return layerList[layerIx].nodes[nodeIx].weights;
}

function network_open_file()
{
    $('#file-open').trigger('click');
}

function network_check_changes(func, title)
{
    // TODO
    func();
}

function network_new_layer(optValue)
{
    var layer = structuredClone(layerInfo);
    if (optValue == 1) {    // insert as first layer
        layer.inputs = layerList[0].inputs;
        for (var i=0; i<layerList[0].nodes.length; i++)
        {
            var node = structuredClone(nodeInfo);
            node.bias = layer.def_bias;
            for (var j=0; j<layer.inputs; j++)
            {
                node.weights.push(network_get_random_weight(layer.inputs));
            }
            layer.nodes.push( node );
        }
        layerList.splice(0, 0, layer);  // insert in front
        layer[1].inputs = layerList[0].nodes.length;
        for (var i=0; i<layerList[1].nodes.length; i++)
        {
            layerList[1].nodes.bias = DEFAULT_BIAS;
            layerList[1].nodes.weights = [];
            for (var j=0; j<layer[0].nodes; j++)
            {
                layer[j].nodes.weights.push(network_get_random_weight(layerList[1].nodes.length));
            }
        }
    }
    else if (optValue > layerList.length) { // append as last layer
        layer.inputs = layerList[layerList.length-1].nodes.length;
        for (var i=0; i<layerList[layerList.length-1].nodes.length; i++)
        {
            var node = structuredClone(nodeInfo);
            node.bias = DEFAULT_BIAS;
            for (var j=0; j<layer.inputs; j++)
            {
                node.weights.push(network_get_random_weight(layer.inputs));
            }
            layer.nodes.push( node );
        }
        layerList.push(layer);  // append to end
    }
    else {  // insert in middle
        layer.inputs = layerList[optValue-2].nodes.length;
        layerList.splice(optValue-1, 0, layer);
        for (var i=0; i<layerList[optValue-2].nodes.length; i++)
        {
            var node = structuredClone(nodeInfo);
            node.bias = DEFAULT_BIAS;
            for (var j=0; j<layer.inputs; j++)
            {
                node.weights.push(network_get_random_weight(layer.inputs));
            }
            layer.nodes.push( node );
        }
        layerList[optValue].inputs = layer.nodes.length;
    }
    var length = $('#layer-list option').length + 1;
    var optText = "Layer" + length;
    $('#layer-list').append(`<option value="${length}">${optText}</option>`);
    network_update();
}

function network_add_layer()
{
    var all_selected = $("#layer-list :selected").map((_, e) => e.value).get();
    var lastValue = 1;
    if (all_selected.length != 0)
    {
        all_selected.sort(function(a, b){return b-a});
        lastValue = parseInt(all_selected[all_selected.length-1]);
    }
    else
    {
        var values = $('#layer-list>option').map( function() {
            return $(this).val();
        }).get();
        $.each(values, function(index,value) {
            if (parseInt(value) > lastValue)
                lastValue = parseInt(value);
        });
    }
    network_new_layer(lastValue+1);
}

function network_layer_remove(selected)
{
    if (selected >= 1 && selected < layerList.length)
    {
        let inputs = layerList.inputs;
        layerList.splice(selected-1,1);
        for (var i=0; i<layerList[selected-1].nodes.length; i++)
        {
            let node = layerList[0].nodes[i];
            node.bias = DEFAULT_BIAS;
            node.weights = [];
            for (var j=0; j<inputs; j++)
            {
                node.weights.push(network_get_random_weight(inputs));
            }
        }
    }
    else // delete last layer
    {
        layerList.pop();
    }
    $('#layer-list').children("option[value=" + selected + "]").remove();
    var length = $('#layer-list option').length;
    for (var i=selected; i<=length; i++) {
        var layer = $('#layer-list').children("option[value=" + (i + 1) + "]");
        var name = layer.text();
        layer.text("Layer" + i);
        layer.attr("value",i);
    }
    network_update();
}

function network_delete_layer()
{
    var all_selected = $("#layer-list :selected").map((_, e) => e.value).get();
    all_selected.sort(function(a, b){return b-a});
    all_selected.forEach(function(ix) {
        selected = parseInt(ix);
        network_layer_remove(selected);
    });
 }

function network_copy_layers()
{
    layerCopiesList = $("#layer-list :selected").map((_, e) => e.value).get();
    if (layerCopiesList.length > 0)
        $('#paste-menu-item').removeClass('disabled');
}

function network_paste_layers()
{
    if (layerCopiesList.length == 0)
        return;
    var values = $('#layer-list>option').map( function() {
        return $(this).val();
    }).get();
    var lastValue = 1;
    $.each(values, function(index,value) {
        if (parseInt(value) > lastValue)
            lastValue = parseInt(value);
    });
    var new_id = lastValue + 1;
    var id_list = []
    layerCopiesList.forEach(function(copy_id) {
        id_list.push(new_id);
        new_id++;
    });
    //layer_update(id_list);
}

function network_select_all_layers()
{
    all_selected = []
    for (var i=1; i<=layer_count(); i++)
        all_selected.push(i);
    //layer_update(all_selected);
}

function network_delete_all_layers()
{
    $('#layer-list').empty();
    //layer_clear();
}

function network_alert(head_text, body_text)
{
    $("#alert-head-text").text(head_text);
    $("#alert-body-text").text(body_text);
    $("#alert-modal").modal('show');
}

function network_parameter_change(layersSelected, param_id)
{
    let change = false;
    let lastSelected = false;
    layersSelected.forEach(function(idStr) {
        selected = parseInt(idStr) - 1;
        if (selected == layerList.length-1)
            lastSelected = true;
    });
    if (param_id == "networkInputs")
    {
        let inputCount = parseInt($('#networkInputs').val());
        if (!Number.isNaN(inputCount) && layerList.length > 0)
        {
            layerList[0].inputs = inputCount;
            var layer = layerList[0];
            for (var i=0; i<layer.nodes.length; i++)
            {
                let node = layer.nodes[i];
                if (node.weights.length > inputCount)
                {
                    while (node.weights.length > inputCount)
                    {
                        node.weights.pop();
                    }
                }
                else if (node.weights.length < inputCount)
                {
                    while (node.weights.length < inputCount)
                    {
                        node.weights.push(network_get_random_weight(inputCount));
                    }
                }
            }
            change = true;
        }
    }
    else if (param_id == "networkOutputs")
    {
        let outputCount = parseInt($('#networkOutputs').val());
        if (!Number.isNaN(outputCount) && layerList.length > 0)
        {
            if (layerList[layerList.length-1].nodes.length > outputCount)
            {
                layerList[layerList.length-1].nodes.pop();
            }
            else if (layerList[layerList.length-1].nodes.length < outputCount)
            {
                let node = structuredClone(nodeInfo);
                let nodeInputCnt = (layerList.length == 0) ? layerList[0].inputs :
                                    layerList[layerList.length-1].nodes.length;
                let biasStr = $('#layerDefaultBias').val();
                let biasValue = parseFloat(biasStr);
                node.bias = biasValue;
                for (var j=0; j<nodeInputCnt; j++)
                {
                    node.weights.push(network_get_random_weight(nodeInputCnt));
                }
                layerList[layerList.length-1].nodes.push(node);

            }
            if (lastSelected)
                $('#layerNodes').val(outputCount);
            change = true;
        }
    }
    if (change)
    {
        network_update();
    }
}

function network_save_config(network)
{
    layerList = []
    var inputs = network["inputs"];
    for (var i=0; i<network["layers"].length; i++)
    {
        let networkLayer = network["layers"][i];
        let layer = structuredClone(layerInfo);
        for (var j=0; j<networkLayer["nodes"].length; j++)
        {
            let networkNode = networkLayer["nodes"][j];
            let node = structuredClone(nodeInfo);
            node.bias = networkNode.bias;
            for (var k=0; k<networkNode.weights.length; k++)
            {
                node.weights.push(networkNode.weights[k]);
            }
            layer.nodes.push( node );
        }
        layer.def_bias = DEFAULT_BIAS;
        layer.inputs = inputs;
        inputs = layer.nodes.length;
        layerList.push(layer);
    }
    network_update();

    $('#networkInputs').val(network_get_inputs());
    $('#networkOutputs').val(network_get_outputs());
    $('#layer-list').empty();
    for (var i=1; i<=network_get_layers(); i++)
    {
        $('#layer-list').append("<option value=\"" + i + "\">Layer" + i + "</option");
    }
    $('#layerNodes').val(network_get_nodes(0));
    $('#layerDefaultBias').val(layerList[0].def_bias);

}

function network_read_config()
{
    var network = {};
    network["inputs"] = network_get_inputs();
    let layers = [];
    for (var i=0; i<network_get_layers(); i++)
    {
        let layer = {};
        layer["nodes"] = [];
        let nodeCnt = network_get_nodes(i);
        for (var j=0; j<nodeCnt; j++)
        {
            let node = {};
            let bias = network_get_node_bias(i, j);
            let weights = network_get_node_weights(i, j);
            if (weights)
            {
                node["bias"] = bias;
                node["weights"] = weights;
            }
            layer["nodes"].push(node);
        }
        layers.push(layer);
    }
    network["layers"] = layers;
    network["outputs"] = network_get_outputs();
    return network;
}

function network_training_change(id)
{
}

function network_layer_change(allLayers, id)
{
    let change = false;
    let selectedLayers = [];
    allLayers.forEach(function(idStr) {
        selected = parseInt(idStr) - 1;
        if (selected >= 0 && selected < layerList.length) {
            selectedLayers.push(selected);
        }
    });
    if (id == "layerNodes")
    {
        let nodesStr = $('#layerNodes').val();
        let nodesInt = parseInt(nodesStr);
        for (var i=0; i<selectedLayers.length; i++)
        {
            let layerIx = selectedLayers[i];
            var layer = layerList[layerIx];
            if (nodesInt < layer.nodes.length) {
                while (nodesInt < layer.nodes.length)
                {
                    layer.nodes.pop();
                }
            }
            else if (nodesInt > layer.nodes.length)
            {
                while (nodesInt > layer.nodes.length)
                {
                    let node = structuredClone(nodeInfo);
                    node.bias = layer.def_bias;
                    for (var j=0; j<layer.inputs; j++)
                    {
                        node.weights.push(network_get_random_weight(layer.inputs));
                    }
                    layer.nodes.push(node);
                }
            }
            if (layerIx == layerList.length-1)
                $('#networkOutputs').val(layer.nodes.length);
            change = true;
        }
    }
    else if (id == "layerDefaultBias")
    {
        let biasStr = $('#layerDefaultBias').val();
        let biasValue = parseFloat(biasStr);
        for (var i=0; i<selectedLayers.length; i++)
        {
            let layerIx = selectedLayers[i];
            var layer = layerList[layerIx];
            layer.def_bias = biasValue;
            for (var j=0; j<layer.nodes.length; j++)
            {
                layer.nodes[j].bias = layer.def_bias;
            }
        }
    }
    if (change)
    {
        network_update();
    }
}

function network_update()
{
    draw_redraw();
    measurement_set_inputs(network_get_inputs());
}

function network_layer_update(selected_layers)
{
    if (selected_layers.length == 0) {
        $('#layerDefaultBias').val("");
        $('#layerNodes').val("");
        $('#layerDefaultBias').prop('disabled', true);
        $('#layerNodes').prop('disabled', true);
    }
    else {
        var first_layer_bias = false;
        var first_layer_nodes = false;
        selected_layers.forEach(id => {
            selected = parseInt(id) - 1;
            if (selected >= 0 && selected < layerList.length) {
                if (!first_layer_bias) {
                    first_layer_bias = layerList[selected].def_bias;
                    first_layer_nodes = layerList[selected].nodes.length;
                    $('#layerDefaultBias').val(first_layer_bias);
                    $('#layerNodes').val(first_layer_nodes);
                    $('#layerDefaultBias').prop('disabled', false);
                    $('#layerNodes').prop('disabled', false);
                }
                else {
                    if (layerList[selected].def_bias != first_layer_bias)
                        $('#layerDefaultBias').val("");
                    if (layerList[selected].nodes.length != first_layer_nodes)
                        $('#layerNodes').val("");
                }
            }
        });
    }
    $('#head-list').val(selected_layers);
    all_selected = selected_layers.length >= layerList.length;
    $('#delete-button').prop('disabled', all_selected || (selected_layers.length == 0));
    network_update();
}

function network_refresh_all()
{
    network_update();
}

function network_get_random_weight(inputs)
{
    const distributionRangeHalfWidth = ( 2.4 / inputs );
    const standardDeviation = distributionRangeHalfWidth * 2 / 6;
    const u1 = Math.random();
    const u2 = Math.random();
    const z = Math.sqrt(-2.0 * Math.log(u1)) * Math.cos(2.0 * Math.PI * u2);
    return z * standardDeviation;
}

function network_set_output_values(outputs)
{
    var maxOutputs = network_get_outputs();
    for(let i=0; i< maxOutputs; i++)
    {
        draw_set_output(i, outputs[i]);
    }
}

function network_set_input_values(inputs)
{
    var maxInputs = network_get_inputs();
    for(let i=0; i< maxInputs; i++)
    {
        draw_set_input(i, inputs[i] / 255);
    }
}

function network_set_files(networkFiles)
{
    var length = $('#networkFile option').length;
    for (var i=1; i<=length; i++)
        $('#networkFile').children("option[value=" + i + "]").remove();
    var optValue = 1;
    for(var file of networkFiles)
    {
        $('#networkFile').append(`<option value="${optValue}">${file}</option>`);
        optValue++;
    }
}
