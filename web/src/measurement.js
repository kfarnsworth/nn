//-----------------------------------------------------------------------
// copyright (c) 2024 Farnsworth Technology
//-----------------------------------------------------------------------

var inputSize = 0;
var drawcanvasChild = 0;
const MEASURE_TWIPS_PERINCH = 1440.0;            // 1 pt == 1/72, 20 pts per inch
const MEASURE_TWIPS_PERPIXEL = 15;
const MEASURE_PIXELS_PERINCH = MEASURE_TWIPS_PERINCH / MEASURE_TWIPS_PERPIXEL;
const MEASURE_WIDTH_INCHES = 1;
const MEASURE_HEIGHT_INCHES = 1;
var pixels = [];
var sideLen = 0;
var numPixels = 0;

function measurement_set_inputs(numInputs)
{
    var drawarea = document.getElementById("drawareadiv");

    // clear canvas
    if (drawcanvasChild) {
        drawarea.removeChild(drawcanvasChild);
    }
    if (numInputs == 0)
    {
        drawcanvasChild = 0;
        return;
    }
    var canvas = document.createElement('canvas');
    drawcanvasChild = canvas;
    sideLen = Math.ceil(Math.sqrt(numInputs));
    numPixels = Math.pow(sideLen, 2);
    var canvasAppoxWidth = Math.ceil(MEASURE_WIDTH_INCHES * MEASURE_PIXELS_PERINCH);
    var canvasAppoxHeight = Math.ceil(MEASURE_HEIGHT_INCHES * MEASURE_PIXELS_PERINCH);
    canvasWidth = Math.ceil(canvasAppoxWidth / sideLen) * sideLen;
    canvasHeight = Math.ceil(canvasAppoxHeight / sideLen) * sideLen;
    if (canvasWidth > drawarea.offsetWidth) 
        canvasWidth = Math.floor(canvasAppoxWidth / sideLen) * sideLen;
    if (canvasHeight > drawarea.offsetHeight) 
        canvasHeight = Math.floor(canvasAppoxHeight / sideLen) * sideLen;
    canvas.width = canvasWidth;
    canvas.height = canvasHeight;
    canvas.style.zIndex = 1;
    canvas.style.position = "absolute";
    canvas.style.border = "none";
    drawarea.appendChild(canvas);
    
    pixels = [];
    for (var i=0; i<numPixels; i++)
    {
        pixels.push(0);
    }
    measurement_draw();
    measurement_setup_mouse_events(canvas);
}

function measurement_draw()
{
    var pixWidth = Math.floor(drawcanvasChild.width / sideLen);
    var pixHeight = Math.floor(drawcanvasChild.height / sideLen);
    var ctx = drawcanvasChild.getContext('2d');
    for (var row=0; row<sideLen; row++)
    {
        for (var col=0; col<sideLen; col++)
        {
            measurement_draw_pixel(row * pixWidth, col * pixHeight, 255 - pixels[row * sideLen + col]);
        }
    }
}

function measurement_update_pixel(row, col, value)
{
    pixels[row * sideLen + col] = value;
}

var pixLastDrawX = -1;
var pixLastDrawY = -1;
function measurement_draw_pixel(X, Y, C)
{
    var pixWidth = Math.floor(drawcanvasChild.width / sideLen);
    var pixHeight = Math.floor(drawcanvasChild.height / sideLen);
    var pixLocX = X - (X % pixWidth);
    var pixLocY = Y - (Y % pixHeight);
    var ctx = drawcanvasChild.getContext('2d');
    ctx.fillStyle = `rgb(${C}, ${C}, ${C})`;
    ctx.strokeStyle = `rgb(${C}, ${C}, ${C})`;
    ctx.lineWidth = "1";
    ctx.fillRect(pixLocX, pixLocY, pixWidth, pixHeight);
    measurement_update_pixel(Math.floor(pixLocX/pixWidth), Math.floor(pixLocY/pixHeight), 255 - C);
    var newPix =  pixLocX != pixLastDrawX || pixLocY != pixLastDrawY;
    pixLastDrawX = pixLocX;
    pixLastDrawY = pixLocY;
    return newPix;
}

function measurement_clear()
{
    pixels = [];
    for (var i=0; i<numPixels; i++)
    {
        pixels.push(0);
    }
    measurement_draw();
}

function measurement_get_input()
{
    data = {};
    data["divisor"] = 255;
    data["input"] = pixels;
    data["width"] = sideLen;
    data["height"] = sideLen;
    return data;
}

//=======================================================================
var pressed = false;
var mouseXloc = 0;
var mouseYloc = 0;
const COLOR_GREY_LIGHTEST = 220;
const COLOR_GREY_DARKEST = 0;
const COLOR_GREY_INCR = 15;
var mouseColor = COLOR_GREY_LIGHTEST;

function measurement_mouse_down(mouseX, mouseY)
{
    //console.log("DOWN " + mouseX + ":" + mouseY);
    pressed = true;
    mouseXloc = mouseX;
    mouseYloc = mouseY;
}

function measurement_mouse_up(mouseX, mouseY)
{
    //console.log("UP " + mouseX + ":" + mouseY);
    pressed = false;
    mouseColor = COLOR_GREY_LIGHTEST;
    network_set_input_values(pixels);
}

function measurement_mouse_move(deltaX, deltaY)
{
    if (pressed)
    {
        mouseXloc += deltaX;
        mouseYloc += deltaY;
        //console.log("MOVE " + mouseXloc + ":" + mouseYloc + ":" +  mouseColor);
        if (measurement_draw_pixel(mouseXloc, mouseYloc, mouseColor) == true)
        {
            mouseColor = COLOR_GREY_LIGHTEST;
        }
        else
        {
            mouseColor -= COLOR_GREY_INCR;
            if (mouseColor < COLOR_GREY_DARKEST)
                mouseColor = COLOR_GREY_DARKEST;
        }
    }
}

function measurement_mouse_out()
{
    console.log("OUT ");
    pressed = false;
    mouseColor = COLOR_GREY_LIGHTEST;
    network_set_input_values(pixels);
}

function measurement_setup_mouse_events(canvas)
{
    canvas.onmousedown = function(e) {
        measurement_mouse_down(e.offsetX, e.offsetY);
    };

    canvas.onmousemove = function(e) {
        measurement_mouse_move(e.movementX, e.movementY);
    };

    canvas.onmouseup = function(e)
    {
        measurement_mouse_up(e.offsetX, e.offsetY);
    };
    canvas.onmouseout = function(e)
    {
        measurement_mouse_out();
    };

    //canvas.addEventListener("touchstart", function (e) {
    //    var touch = e.touches[0];
    //    var rect = canvas.getBoundingClientRect();
    //    touchPosX = touch.clientX - rect.left;
    //    touchPosY = touch.clientY - rect.top;
    //    measurement_mouse_down(touchPosX, touchPosY);
    //});

    //canvas.addEventListener("touchend", function (e) {
    //    measurement_mouse_up();
    //});

    //canvas.addEventListener("touchmove", function (e) {
    //    var touch = e.touches[0];
    //    var rect = canvas.getBoundingClientRect();
    //    var nextPosX = touch.clientX - rect.left;
    //    var nextPosY = touch.clientY - rect.top;
    //    measurement_mouse_move(nextPosX - touchPosX, nextPosY - touchPosY);
    //    touchPosX = nextPosX;
    //    touchPosY = nextPosY;
    //    if (draggable)
    //        e.preventDefault();
    //});
}
