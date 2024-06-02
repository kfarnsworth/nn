//-----------------------------------------------------------------------
// copyright (c) 2024 Farnsworth Technology
//-----------------------------------------------------------------------

const DEFAULT_UNIT = 32;                 // Per inch
const TWIPS_PERINCH = 1440.0;            // 1 pt == 1/72, 20 pts per inch
const TWIPS_PERPIXEL = 15;
const PIXELS_PERINCH = TWIPS_PERINCH / TWIPS_PERPIXEL;

const SCALE_ZOOM_INC = 0.1;
const SCALE_ZOOM_MIN = 0.5;
const SCALE_ZOOM_MAX = 4.0;

const DRAW_LEFT_MARGIN = 0.5;
const DRAW_TOP_MARGIN = 0.5;
const DRAW_ROW_PACING = 0.3;
const DRAW_COLUMN_PACING = 1.5;
const DRAW_MAX_PER_COLUMN = 15;
const DRAW_START_COUNT = 10;
const DRAW_STOP_COUNT = 2;
const DRAW_INPUT_BOX_SIZE = 0.2;
const DRAW_NODE_CIRCLE_SIZE = 0.2;
const DRAW_DOT_SIZE = 0.1;

var draw_info = {
    index: 0,
    canvas: null,
    x: 0,
    y: 0,
    headId: "",
    textStartX: 0,
    textStartY: 0,
    textHeight: 0,
    textWidth: 0,
    labelHeight: 0,
    labelWidth: 0,
    xBoxStart: 0,
    yBoxStart: 0,
    xBoxWidth: 0,
    yBoxHeight: 0,
    selected: false,
    qrc: false,
    xQrc: 0,
    yQrc: 0,
    xQrcStart: 0,
    yQrcStart: 0,
    qrcSize: 0,
    headLinked: true
}

var canvasChild = 0;

var drawCanvas = [];
var draggable = false;
var isQrc = false;
var draw_first_x = 0;
var draw_first_y = 0;
var draw_last_x = 0;
var draw_last_y = 0;
var zoom = 1.0;
var zoom_scale = 1.0;
var arrowKeyActed = false;
var keyReleased = false;
var shiftKey = false;
var cntrlKey = false;
var rightKey = false;
var leftKey = false;
var upKey = false;
var downKey = false;
var touchPosX = 0;
var touchPosY = 0;

function draw_init()
{
    drawCanvas = [];
    draggable = false;
    draw_last_x = draw_canvas_width();
    draw_last_y = draw_canvas_height();
    window.addEventListener('keydown', draw_keydown);
    window.addEventListener('keyup', draw_keyup);
}

function draw_inches_to_pixels(inches)
{
    return Math.round(inches * PIXELS_PERINCH);
}

function draw_twips_to_pixels(twips)
{
    return Math.round(twips / TWIPS_PERPIXEL);
}

function draw_pixels_to_twips(pixels)
{
    return Math.round(pixels * TWIPS_PERPIXEL);
}

function draw_zoom_scale()
{
    return zoom * zoom_scale;
}

function draw_page_width()
{
    return 7.5;
}

function draw_page_height()
{
    return 6.0;
}

function draw_canvas_width()
{
    return Math.ceil(draw_page_width() * PIXELS_PERINCH * zoom);
}

function draw_canvas_height()
{
    return Math.ceil(draw_page_height() * PIXELS_PERINCH * zoom);
}

function draw_input_box(ctx, xPos, yPos)
{
    var zoom_draw = draw_zoom_scale();
    var xPosPixels = draw_inches_to_pixels(xPos);
    var yPosPixels = draw_inches_to_pixels(yPos);
    var xPoint = Math.ceil(xPosPixels * zoom_draw);
    var yPoint = Math.ceil(yPosPixels * zoom_draw);
    var hPixels = draw_inches_to_pixels(0.2 * zoom_draw);
    var wPixels = draw_inches_to_pixels(0.2 * zoom_draw);
    ctx.beginPath();
    ctx.rect(xPoint, yPoint, hPixels, wPixels);
    ctx.stroke();
}

function draw_continued_dot(ctx, xPos, yPos)
{
    var zoom_draw = draw_zoom_scale();
    var xPosPixels = draw_inches_to_pixels(xPos + DRAW_DOT_SIZE);
    var yPosPixels = draw_inches_to_pixels(yPos + DRAW_DOT_SIZE);
    var radiusPixels = draw_inches_to_pixels(DRAW_DOT_SIZE / 2);
    var xPoint = Math.ceil(xPosPixels * zoom_draw);
    var yPoint = Math.ceil(yPosPixels * zoom_draw);
    var radius = Math.ceil(radiusPixels * zoom_draw);
    ctx.beginPath();
    ctx.arc(xPoint, yPoint, radius, 0, 2 * Math.PI);
    ctx.fillStyle = "black";
    ctx.fill();
}

function draw_connect_line(ctx, xPosStart, yPosStart, xPosEnd, yPosEnd)
{
    var zoom_draw = draw_zoom_scale();
    var xPosStartPixels = draw_inches_to_pixels(xPosStart);
    var yPosStartPixels = draw_inches_to_pixels(yPosStart);
    var xPointStart = Math.ceil(xPosStartPixels * zoom_draw);
    var yPointStart = Math.ceil(yPosStartPixels * zoom_draw);
    var xPosEndPixels = draw_inches_to_pixels(xPosEnd);
    var yPosEndPixels = draw_inches_to_pixels(yPosEnd);
    var xPointEnd = Math.ceil(xPosEndPixels * zoom_draw);
    var yPointEnd = Math.ceil(yPosEndPixels * zoom_draw);
    ctx.beginPath();
    ctx.moveTo(xPointStart, yPointStart);
    ctx.lineTo(xPointEnd, yPointEnd);
    ctx.stroke();
}

function draw_node_circle(ctx, xPos, yPos, nodeInputCnt)
{
    var zoom_draw = draw_zoom_scale();
    var xPosPixels = draw_inches_to_pixels(xPos + DRAW_NODE_CIRCLE_SIZE/2);
    var yPosPixels = draw_inches_to_pixels(yPos + DRAW_NODE_CIRCLE_SIZE/2);
    var radiusPixels = draw_inches_to_pixels(DRAW_NODE_CIRCLE_SIZE/2);
    var xPoint = Math.ceil(xPosPixels * zoom_draw);
    var yPoint = Math.ceil(yPosPixels * zoom_draw);
    var radius = Math.ceil(radiusPixels * zoom_draw);
    ctx.beginPath();
    ctx.arc(xPoint, yPoint, radius, 0, 2 * Math.PI);
    ctx.stroke();

    var inputs = nodeInputCnt;
    if (inputs > DRAW_START_COUNT)
        inputs = DRAW_START_COUNT;
    var xPosInput = xPos - DRAW_COLUMN_PACING + DRAW_NODE_CIRCLE_SIZE;
    var yPosInput = DRAW_TOP_MARGIN + DRAW_NODE_CIRCLE_SIZE/2;
    var xPosNode = xPos;
    var yPosNode = yPos + DRAW_NODE_CIRCLE_SIZE/2;
    for (i=0; i<inputs; i++)
    {
        draw_connect_line(ctx, xPosInput, yPosInput, xPosNode, yPosNode);
        yPosInput += DRAW_ROW_PACING; 
    }
    if (inputs < nodeInputCnt)
    {
        yPosInput += DRAW_ROW_PACING * 3 + 0.2;
        for (var i=0; i<DRAW_STOP_COUNT; i++)
        {
            draw_connect_line(ctx, xPosInput, yPosInput, xPosNode, yPosNode);
            yPosInput += DRAW_ROW_PACING;
        }
    }
}

function draw_output_box(ctx, xPos, yPos)
{
    var zoom_draw = draw_zoom_scale();
    var xPosPixels = draw_inches_to_pixels(xPos);
    var yPosPixels = draw_inches_to_pixels(yPos);
    var xPoint = Math.ceil(xPosPixels * zoom_draw);
    var yPoint = Math.ceil(yPosPixels * zoom_draw);
    var hPixels = draw_inches_to_pixels(0.2 * zoom_draw);
    var wPixels = draw_inches_to_pixels(0.2 * zoom_draw);
    ctx.beginPath();
    ctx.rect(xPoint, yPoint, hPixels, wPixels);
    ctx.stroke();
    draw_connect_line(ctx, xPos - DRAW_COLUMN_PACING + DRAW_NODE_CIRCLE_SIZE, 
                    yPos + 0.1, xPos, yPos + 0.1);
}

function draw_redraw()
{
    var area = document.getElementById("canvasesdiv");

    // clear canvas
    if (canvasChild) {
        area.removeChild(canvasChild);
    }

    var canvas = document.createElement('canvas');
    canvasChild = canvas;
    var xPos = DRAW_LEFT_MARGIN;
    var yPos = DRAW_TOP_MARGIN;
    var zoom_draw = draw_zoom_scale();

    canvas.width = draw_canvas_width();
    canvas.height = draw_canvas_height();
    canvas.style.zIndex = 1;
    canvas.style.position = "absolute";
    canvas.style.border = "none";
    area.appendChild(canvas);
    var ctx = canvas.getContext('2d');
    ctx.fillStyle = "rgba(255, 0, 0, 0.2)";
    ctx.strokeStyle = "black";
    ctx.lineWidth = "1";
    //ctx.clearRect(0, 0, canvas.width, canvas.height);

    // draw inputs
    var inputCnt = network_get_inputs();
    if (inputCnt > DRAW_MAX_PER_COLUMN)
        inputCnt = DRAW_START_COUNT;
    for (var i=0; i<inputCnt; i++) {
        draw_input_box(ctx, xPos, yPos);
        yPos += DRAW_ROW_PACING;
    }
    if (network_get_inputs() > inputCnt)
    {
        yPos += 0.1;
        for (var i=0; i<3; i++)
        {
            draw_continued_dot(ctx, xPos, yPos);
            yPos += DRAW_ROW_PACING;
        }
        yPos += 0.1;
        for (var i=network_get_inputs()-DRAW_STOP_COUNT; i<network_get_inputs(); i++) {
            draw_input_box(ctx, xPos, yPos);
            yPos += DRAW_ROW_PACING;
        }
    }
    xPos += DRAW_COLUMN_PACING;

    // draw each layer
    yPos = DRAW_TOP_MARGIN;
    var nodeInputCnt = network_get_inputs();
    for (var i=0; i<network_get_layers(); i++)
    {
        nodeCnt = network_get_nodes(i);
        if (nodeCnt > DRAW_MAX_PER_COLUMN)
            nodeCnt = DRAW_START_COUNT;
        for (var j=0; j<nodeCnt; j++) {
            draw_node_circle(ctx, xPos, yPos, nodeInputCnt);
            yPos += DRAW_ROW_PACING;
        }
        if (network_get_nodes(i) > nodeCnt)
        {
            yPos += 0.1;
            for (var j=0; j<3; j++)
            {
                draw_continued_dot(ctx, xPos, yPos);
                yPos += DRAW_ROW_PACING;
            }
            yPos += 0.1;
            for (var j=network_get_nodes(i)-DRAW_STOP_COUNT; j<network_get_nodes(i); j++) {
                draw_node_circle(ctx, xPos, yPos, nodeInputCnt);
                yPos += DRAW_ROW_PACING;
            }
        }
        nodeInputCnt = network_get_nodes(i);
        xPos += DRAW_COLUMN_PACING;
        yPos = DRAW_TOP_MARGIN;
    }

    // draw outputs
    yPos = DRAW_TOP_MARGIN;
    var outputCnt = network_get_outputs();
    if (outputCnt > DRAW_MAX_PER_COLUMN)
    outputCnt = DRAW_START_COUNT;
    for (var i=0; i<outputCnt; i++) {
        draw_output_box(ctx, xPos, yPos);
        yPos += DRAW_ROW_PACING;
    }
    if (network_get_outputs() > outputCnt)
    {
        yPos += 0.1;
        for (var i=0; i<3; i++)
        {
            draw_continued_dot(ctx, xPos, yPos);
            yPos += DRAW_ROW_PACING;
        }
        yPos += 0.1;
        for (var i=network_get_outputs()-DRAW_STOP_COUNT; i<network_get_outputs(); i++) {
            draw_output_box(ctx, xPos, yPos);
            yPos += DRAW_ROW_PACING;
        }
    }
}

function keyRepeat()
{
    if (arrowKeyActed) {
        arrowKeyActed = false;
        setTimeout(keyRepeat, 100);
    }
}

function draw_keydown(e)
{
    var active = document.activeElement;
    if (active.id != "")
        return;
    if (e.keyCode == 16)
        shiftKey = true;
    else if (e.keyCode == 17)
        cntrlKey = true;
    else if (e.keyCode == 39)
        rightKey = !arrowKeyActed;
    else if (e.keyCode == 37)
        leftKey = !arrowKeyActed;
    else if (e.keyCode == 38)
        upKey = !arrowKeyActed;
    else if (e.keyCode == 40)
        downKey = !arrowKeyActed;
    else if (e.keyCode == 46) {
        main_delete_head();
        e.preventDefault();
    }
    else if (e.keyCode == 90 && cntrlKey) {
        undo_pop();
        e.preventDefault();
    }
    else if (e.keyCode == 80 && cntrlKey) {
        main_paste_heads();
        e.preventDefault();
    }
    else if (e.keyCode == 67 && cntrlKey) {
        main_copy_heads()
        e.preventDefault();
    }
    else if (e.keyCode == 83 && cntrlKey) {
        file_save();
        e.preventDefault();
    }
    else if (e.keyCode == 65 && cntrlKey) {
        select_all_heads();
        e.preventDefault();
    }
    if (e.keyCode >= 37 && e.keyCode <= 40) {
        //setTimeout(keyRepeat, 100);
        var nothingSelected = true;
        for (var i=0; i<drawCanvas.length; i++) {
            if (drawCanvas[i].selected) {
                nothingSelected = false;
                break;
            }
        }
        if (!nothingSelected) {
            if (!keyReleased)
                arrowKeyActed = false;
            e.preventDefault();
        }
    }
}

function draw_keyup(e)
{
    var active = document.activeElement;
    if (active.id != "")
        return;
    if (e.keyCode == 16)
        shiftKey = false;
    else if (e.keyCode == 17)
        cntrlKey = false;
    else if (e.keyCode == 39)
        rightKey = false;
    else if (e.keyCode == 37)
        leftKey = false;
    else if (e.keyCode == 38)
        upKey = false;
    else if (e.keyCode == 40)
        downKey = false;
    if (e.keyCode >= 37 && e.keyCode <= 40)
    {
        arrowKeyActed = false;
        head_move_stopped();
        keyReleased = true;
    }
}

function draw_mouse_down(mouseX, mouseY)
{
    //console.log("DOWN " + mouseX + ":" + mouseY);
    draggable = false;
    for (var i=0; i<drawCanvas.length; i++)
    {
        info = drawCanvas[i];
        if (mouseX >= info.xBoxStart &&
            mouseX <= (info.xBoxStart + info.xBoxWidth) &&
            mouseY >= info.yBoxStart &&
            mouseY <= (info.yBoxStart + info.yBoxHeight))
        {
            draggable = info;
            isQrc = false;
            if (!draggable.selected)
                head_selected(draggable.index, cntrlKey);
            break;
        }
        if (info.qrc &&
            mouseX >= info.xQrcStart &&
            mouseX <= (info.xQrcStart + info.qrcSize) &&
            mouseY >= info.yQrcStart &&
            mouseY <= (info.yQrcStart + info.qrcSize))
        {
            draggable = info;
            isQrc = true;
            if (!draggable.selected)
                head_selected(draggable.index, cntrlKey);
            break;
        }
    }
    if (!draggable)
    {
        head_unselected();
        draw_unselect_all();
    }
    else
    {
        head_move_started();
    }
}

function draw_setup_mouse_events(canvas)
{
    canvas.onmousedown = function(e) {
        draw_mouse_down(e.offsetX, e.offsetY);
    };

    canvas.onmousemove = function(e) {
        draw_mouse_move(e.movementX, e.movementY);
    };

    canvas.onmouseup = function(e)
    {
        draw_mouse_up();
    };
    canvas.onmouseout = function(e)
    {
        draw_mouse_out();
    };

    canvas.addEventListener("touchstart", function (e) {
        var touch = e.touches[0];
        var rect = canvas.getBoundingClientRect();
        touchPosX = touch.clientX - rect.left;
        touchPosY = touch.clientY - rect.top;
        draw_mouse_down(touchPosX, touchPosY);
    });

    canvas.addEventListener("touchend", function (e) {
        draw_mouse_up();
    });

    canvas.addEventListener("touchmove", function (e) {
        var touch = e.touches[0];
        var rect = canvas.getBoundingClientRect();
        var nextPosX = touch.clientX - rect.left;
        var nextPosY = touch.clientY - rect.top;
        draw_mouse_move(nextPosX - touchPosX, nextPosY - touchPosY);
        touchPosX = nextPosX;
        touchPosY = nextPosY;
        if (draggable)
            e.preventDefault();
    });

    setInterval(function() {
        if (draggable)
        {
            var reselect = !draggable.selected;
            draggable.selected = true;
            if (reselect) {
                for (var i=0; i<drawCanvas.length; i++) {
                    var info = drawCanvas[i];
                    if (info != draggable && info.selected)
                    {
                        draw_clear(info);
                        info.selected = false;
                        draw_draw(info);
                    }
                }
                draw_clear(draggable);
                draw_draw(draggable);
            }
            else {
                for (var i=0; i<drawCanvas.length; i++) {
                    var info = drawCanvas[i];
                    if (info.selected) {
                        draw_clear(info);
                        draw_draw(info);
                        head_move(info.index,
                            draw_pixels_to_twips(info.x),
                            -draw_pixels_to_twips(info.y),
                            draw_pixels_to_twips(info.xQrc),
                            -draw_pixels_to_twips(info.yQrc));
                    }
                }
            }
        }
        // key press checks
        else
        {
            if (upKey || downKey || rightKey || leftKey) {
                arrowKeyActed = true;
                var delta = 5;
                if (cntrlKey) {
                    delta -= 3;
                    if (shiftKey)
                        delta -= 1;
                }
                var xMove = 0;
                var yMove = 0;
                if (upKey)
                    yMove = -delta;
                else if (downKey)
                    yMove = delta;
                else if (rightKey)
                    xMove = delta;
                else if (leftKey)
                    xMove = -delta;
                for (var i=0; i<drawCanvas.length; i++) {
                    var info = drawCanvas[i];
                    if (info.selected) {
                        if (info.xBoxStart + xMove + info.xBoxWidth > draw_last_x) {
                            xMove -= (info.xBoxStart + info.xBoxWidth) - draw_last_x;
                        }
                        else if (info.xBoxStart + xMove < draw_first_x) {
                            xMove += draw_first_x - info.xBoxStart;
                        }
                        if (info.yBoxStart + yMove + info.yBoxHeight > draw_last_y) {
                            yMove -= (info.yBoxStart + info.yBoxHeight) - draw_last_y;
                        }
                        else if (info.yBoxStart + yMove < draw_first_y) {
                            yMove += draw_first_y - info.yBoxStart;
                        }
                        if (info.qrc) {
                            if (info.xQrcStart + xMove + info.qrcSize > draw_last_x) {
                                xMove -= (info.xQrcStart + info.qrcSize) - draw_last_x;
                            }
                            else if (info.xQrcStart + xMove < draw_first_x) {
                                xMove += draw_first_x - info.xQrcStart;
                            }
                            if (info.yQrcStart + yMove + info.qrcSize > draw_last_y) {
                                yMove -= (info.yQrcStart + info.qrcSize) - draw_last_y;
                            }
                            else if (info.yQrcStart + yMove < draw_first_y) {
                                yMove += draw_first_y - info.yQrcStart;
                            }
                        }
                    }
                }
                if (keyReleased) {
                    head_move_started();
                    keyReleased = false;
                }
                for (var i=0; i<drawCanvas.length; i++) {
                    var info = drawCanvas[i];
                    if (info.selected) {
                        info.x += xMove;
                        info.y += yMove;
                        if (info.qrc) {
                            info.xQrc += xMove;
                            info.yQrc += yMove;
                        }
                        head_move_moving();
                        draw_clear(info);
                        draw_draw(info);
                        head_move(info.index,
                            draw_pixels_to_twips(info.x),
                            -draw_pixels_to_twips(info.y),
                            draw_pixels_to_twips(info.xQrc),
                            -draw_pixels_to_twips(info.yQrc));
                    }
                }
                upKey = downKey = rightKey = leftKey = false;
            }
        }
    }, 1000/30);
}

function draw_selected(index, reselect=true)
{
    if (index > 0 && index <= drawCanvas.length) {
        for (var i=0; i<drawCanvas.length; i++) {
            var info = drawCanvas[i];
            if (info.index == index || info.selected)
            {
                if (reselect) {
                    info.selected = info.index == index;
                }
                else if (info.index == index) {
                    info.selected = true;
                }
                draw_clear(info);
                draw_draw(info);
            }
        }
    }
}

function draw_zoom_number(z)
{
    if (z > SCALE_ZOOM_MAX)
        z = SCALE_ZOOM_MAX;
    else if (z < SCALE_ZOOM_MIN)
        z = SCALE_ZOOM_MIN;
    zoom = z;
    draw_redraw()
}

function draw_zoom(zoom_in)
{
    if (zoom_in) {
        if (zoom < SCALE_ZOOM_MAX) {
            draw_zoom_number(zoom + SCALE_ZOOM_INC);
        }
    }
    else {
        if (zoom > SCALE_ZOOM_MIN) {
            draw_zoom_number(zoom  - SCALE_ZOOM_INC);
        }
    }
    $('#zoom-box').val(parseFloat(zoom).toFixed(2));
}
