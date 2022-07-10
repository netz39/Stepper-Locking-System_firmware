#!/usr/bin/python3

import drawSvg as draw
import math

def getPointX(angleInDegrees, radius):
    return (radius * math.cos(math.radians(angleInDegrees)))

def getPointY(angleInDegrees, radius):
    return (radius * math.sin(math.radians(angleInDegrees)))

NUMBER_OF_LEDS_IN_CIRCLE = 32
NUMBER_OF_LEDS_IN_BAR = 10
RADIUS = 45
ANGLE = 11.25

RING_OUTER_DIAMETER = 99
RING_INNER_DIAMETER = 81

RECTANGLE_SIZE = 5

colorDict = {}

for i in range(0,32):
        colorDict[i] = "lime"

colorDict[38] = "red"

d = draw.Drawing(RING_OUTER_DIAMETER, RING_OUTER_DIAMETER, origin='center', displayInline=False)

BACKGROUND_COLOR = '#282828'

# draw PCB
d.append(draw.Rectangle(-RING_OUTER_DIAMETER/2,-RING_OUTER_DIAMETER/2,RING_OUTER_DIAMETER,RING_OUTER_DIAMETER,fill = BACKGROUND_COLOR))
d.append(draw.Circle(0,0,RING_OUTER_DIAMETER/2,fill = 'black'))
d.append(draw.Circle(0,0,RING_INNER_DIAMETER/2,fill = BACKGROUND_COLOR))
d.append(draw.Rectangle(-RING_INNER_DIAMETER/2,-4.5,RING_INNER_DIAMETER,9,fill='black'))

#text="LED Nr. {} \tx: {:.3f},\ty: {:.3f}, \tangle: {:.3f}"

for index in range(NUMBER_OF_LEDS_IN_CIRCLE):
        angle = -ANGLE * (index+1)
        valueX = getPointX(angle, RADIUS)
        valueY = getPointY(angle, RADIUS)
        transformArg= "rotate("+ str(angle) + ")"

        colorArg='slategrey'
        if index in colorDict.keys():
                colorArg = colorDict[index]

        d.append(draw.Rectangle(RADIUS -RECTANGLE_SIZE/2, -RECTANGLE_SIZE/2, 
        RECTANGLE_SIZE, RECTANGLE_SIZE, origin='center', fill=colorArg, transform=transformArg))
        #print(text.format(index + 1, valueX, valueY, (STARTANGLE - angle) % 360))

for index in range(NUMBER_OF_LEDS_IN_BAR):
        valueX = 36.818 - (index * 8.181)

        colorArg='slategrey'
        colorIndex=index + NUMBER_OF_LEDS_IN_CIRCLE
        if colorIndex in colorDict.keys():
                colorArg = colorDict[colorIndex]

        d.append(draw.Rectangle(valueX - RECTANGLE_SIZE / 2, -RECTANGLE_SIZE/2,
        RECTANGLE_SIZE, RECTANGLE_SIZE, origin='center', fill=colorArg)) 

d.setPixelScale(5)
d.saveSvg('example.svg')