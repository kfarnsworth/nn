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
var canvasWidth = 0;
var canvasHeight = 0;
var pixels = [];
var sideLen = 0;
var numPixels = 0;
var pixWidth = 0;
var pixHeight = 0;
var brushSizeInches = 0.15;
var brushSizePixels = Math.floor(MEASURE_PIXELS_PERINCH * brushSizeInches);
const invertedPic = true;

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
    pixWidth = Math.floor(canvasWidth / sideLen);
    pixHeight = Math.floor(canvasHeight / sideLen);

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
    for (var row=0; row<sideLen; row++)
    {
        for (var col=0; col<sideLen; col++)
        {
            measurement_draw_pixel(row * pixWidth, col * pixHeight, 255 - measurement_get_pixel_value(row,col));
        }
    }
}

function measurement_set_pixel_value(row, col, value)
{
    var index;
    row = row % sideLen;
    col = col % sideLen;
    if (invertedPic)
    {
        index = col * sideLen + row;
    }
    else
    {
        index = row * sideLen + col;
    }
    pixels[index] = value;
}

function measurement_get_pixel_value(row, col)
{
    var index;
    if (invertedPic)
    {
        index = col * sideLen + row;
    }
    else
    {
        index = row * sideLen + col;
    }
    return pixels[index];
}

var pixLastDrawX = -1;
var pixLastDrawY = -1;
function measurement_draw_pixel(X, Y, C)
{
    var pixLocX = X - (X % pixWidth);
    var pixLocY = Y - (Y % pixHeight);
    var ctx = drawcanvasChild.getContext('2d');
    ctx.fillStyle = `rgb(${C}, ${C}, ${C})`;
    ctx.strokeStyle = `rgb(${C}, ${C}, ${C})`;
    ctx.lineWidth = "1";
    ctx.fillRect(pixLocX, pixLocY, pixWidth, pixHeight);
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
    network_set_input_values(pixels);
    $('#x-size').val("");
    $('#y-size').val("");
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

function measurement_draw_inputs(inputs)
{
    for (var i=0; i<inputs.length && i<numPixels; i++)
    {
        pixels[i] = Math.floor(255 * inputs[i]);
    }
    measurement_draw();
}

//=======================================================================
var pressed = false;
var mouseXloc = 0;
var mouseYloc = 0;
const COLOR_GREY_LIGHTEST = 220;
const COLOR_GREY_DARKEST = 0;

function measurement_udpate_pixel(x,y,c)
{
    var xPix = Math.floor(x / pixWidth);
    var yPix = Math.floor(y / pixHeight);
    var colorPix = 255 - c;
    if (colorPix < measurement_get_pixel_value(xPix, yPix))
        colorPix = measurement_get_pixel_value(xPix, yPix);
    measurement_set_pixel_value(xPix, yPix, colorPix);
    measurement_draw_pixel(x, y, 255 - colorPix);
}

function measurement_udpate_xy()
{
    $('#x-size').val(Math.floor(mouseXloc / pixWidth));
    $('#y-size').val(Math.floor(mouseYloc / pixHeight));
}

function measurement_update_brush()
{
    var startPixelX = mouseXloc - brushSizePixels/2;
    var startPixelY = mouseYloc - brushSizePixels/2;
    var endPixelX = mouseXloc + brushSizePixels/2;
    var endPixelY = mouseYloc + brushSizePixels/2;
    var stride = 2 * Math.abs(COLOR_GREY_DARKEST - COLOR_GREY_LIGHTEST) / brushSizePixels;
    for (var x=startPixelX; x<endPixelX; x++) {
        var distX = x - mouseXloc;
        for (var y=startPixelY; y<endPixelY; y++) {
            var distY = y - mouseYloc;
            var dist = Math.sqrt( distX * distX + distY * distY);
            var color = Math.abs(COLOR_GREY_DARKEST - Math.floor(dist * stride));
            if (x >= 0 && x < canvasWidth && y >=0 && y < canvasHeight) {
                measurement_udpate_pixel(x,y,color);
            }
        }
    }
}

function measurement_mouse_down(mouseX, mouseY)
{
    //console.log("DOWN " + mouseX + ":" + mouseY);
    pressed = true;
    mouseXloc = mouseX;
    mouseYloc = mouseY;
    measurement_update_brush();
    network_set_input_values(pixels);
    measurement_udpate_xy();
}

function measurement_mouse_up(mouseX, mouseY)
{
    //console.log("UP " + mouseX + ":" + mouseY);
    pressed = false;
}

function measurement_mouse_move(deltaX, deltaY)
{
    if (pressed)
    {
        mouseXloc += deltaX;
        mouseYloc += deltaY;
        //console.log("MOVE " + mouseXloc + ":" + mouseYloc);
        measurement_update_brush();
        measurement_udpate_xy();
    }
}

function measurement_mouse_out()
{
    //console.log("OUT ");
    pressed = false;
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
