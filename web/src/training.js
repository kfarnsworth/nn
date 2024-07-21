//-----------------------------------------------------------------------
// copyright (c) 2024 Farnsworth Technology
//-----------------------------------------------------------------------

var trainingActive = false;

function training_toggle()
{
    if(!trainingActive)
    {
        var trainingFile = $( "#trainingFile option:selected" ).text();
        var trainingType = $( "#trainingType option:selected" ).text();
        var trainingRate = Number.parseFloat($('#learningRate').val());
        var batchSize = Number.parseInt($('#batchSize').val());
        var batchCount = Number.parseInt($('#batchCount').val());
        var outputType = -1;
        var outputTypeStr = $( "#outputType option:selected" ).text();
        if (outputTypeStr != "ALL") {
            outputType = Number.parseInt(outputTypeStr);
        }
        var obj = {
            "filename": trainingFile,
            "type": trainingType,
            "rate": trainingRate,
            "batchSize": batchSize,
            "batchCount": batchCount,
            "outputType": outputType
        }
        connection_start_training(obj, function(status) {
            if (status) {
                training_set_state(true);
                setTimeout(training_update_state, 100);
            }
        });
    }
    else
    {
        connection_stop_training(function(status) {
            training_set_state(false);
        });
    }
}

function training_update_state()
{
    connection_get_training_state(function(info, status) {
        if (status) {
            if (trainingActive != info.isTraining)
            {
                training_set_state(info.isTraining);
                if (!info.isTraining)
                    setTimeout(training_update_state, 300); // one last status update
            }
            network_set_input_values(info.inputs);
            network_set_output_values(info.activations[info.activations.length-1]);
            measurement_draw_inputs(info.inputs);
            $('#training-batch-status').html("Batch " + info.batchCount.toString() + " of "
                                                      + info.batchTotal.toString() + "\r\n"
                                                      + TimeElapsedString(info.totalTime));
            if (info.isTraining)
            {
                setTimeout(training_update_state, 100); // set next status update
            }
        }
    });
}

function training_set_state(isTraining)
{
    trainingActive = isTraining;
    if (isTraining)
    {
        $('#training-button').html("Stop Training");
        $('#training-button').removeClass("btn-success");
        $('#training-button').addClass("btn-danger");
        $('#training-indicator').prop('hidden', false);
        $('#training-batch-status').prop('hidden', false);
    }
    else
    {
        $('#training-button').html("Start Training");
        $('#training-button').removeClass("btn-danger");
        $('#training-button').addClass("btn-success");
        $('#training-indicator').prop('hidden', true);
    }
}

function training_set_files(trainingFiles)
{
    var length = $('#trainingFile option').length;
    for (var i=1; i<=length; i++)
        $('#trainingFile').children("option[value=" + i + "]").remove();
    var optValue = 1;
    for(var file of trainingFiles)
    {
        $('#trainingFile').append(`<option value="${optValue}">${file}</option>`);
        optValue++;
    }
}

function training_set_types(trainingTypes)
{
    var length = $('#trainingType option').length;
    for (var i=1; i<=length; i++)
        $('#trainingType').children("option[value=" + i + "]").remove();
    var optValue = 1;
    for(var type of trainingTypes)
    {
        $('#trainingType').append(`<option value="${optValue}">${type}</option>`);
        optValue++;
    }
}

function training_set_outputs(numOuputs)
{
    var length = $('#outputType option').length;
    for (var i=2; i<=length; i++)
        $('#outputType').children("option[value=" + i + "]").remove();
    var optValue = 2;
    for(var i=0; i<numOuputs; i++)
    {
        optText = i.toString();
        $('#outputType').append(`<option value="${optValue}">${optText}</option>`);
        optValue++;
    }
}

function TimeElapsedString(secs)
{
    let hr = Math.floor(secs / 3600);
    let remainSecs = secs % 3600;
    let min = Math.floor(remainSecs / 60);
    let sec = remainSecs % 60;
    // stupidest code ever!!!!!!!!
    var timeStr = hr == 0 ? "00" : (hr < 10 ? "0" + hr.toString() : hr.toString());
    timeStr += ":"
    timeStr += min == 0 ? "00" : (min < 10 ? "0" + min.toString() : min.toString());
    timeStr += ":"
    timeStr += sec == 0 ? "00" : (sec < 10 ? "0" + sec.toString() : sec.toString());
    return timeStr;
}
