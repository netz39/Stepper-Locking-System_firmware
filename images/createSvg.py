#!/usr/bin/python3

import drawSvg as draw
import math

NUMBER_OF_LEDS_IN_CIRCLE = 32
NUMBER_OF_LEDS_IN_BAR = 10
RADIUS = 45
ANGLE = 11.25

RING_OUTER_DIAMETER = 99
RING_INNER_DIAMETER = 81

RECTANGLE_SIZE = 5

colorDict = {}

colorDict[0] = "yellow"
colorDict[14] = "yellow"
colorDict[16] = "yellow"
colorDict[30] = "yellow"

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
        transformArg= "rotate("+ str(angle) + ")"

        colorArg='slategrey'
        if index in colorDict.keys():
                colorArg = colorDict[index]

        rect = draw.Rectangle(RADIUS -RECTANGLE_SIZE/2, -RECTANGLE_SIZE/2, 
        RECTANGLE_SIZE, RECTANGLE_SIZE, origin='center', fill=colorArg, transform=transformArg)

        if(index == 0 or index == 14 or index == 16 or index == 30):
               rect.appendAnim(draw.Animate('fill', '0.3s', 'yellow;slategrey', calcMode='discrete', repeatCount='indefinite'))

        if(index == 1 or index == 15 or index == 17 or index == 31):
                rect.appendAnim(draw.Animate('fill', '0.3s', 'slategrey;yellow', calcMode='discrete', repeatCount='indefinite'))

        d.append(rect)
        #print(text.format(index + 1, valueX, valueY, (STARTANGLE - angle) % 360))

for index in range(NUMBER_OF_LEDS_IN_BAR):
        valueX = 36.818 - (index * 8.181)

        colorArg='slategrey'
        colorIndex=index + NUMBER_OF_LEDS_IN_CIRCLE
        if colorIndex in colorDict.keys():
                colorArg = colorDict[colorIndex]

        rect = draw.Rectangle(valueX - RECTANGLE_SIZE / 2, -RECTANGLE_SIZE/2,
        RECTANGLE_SIZE, RECTANGLE_SIZE, origin='center', fill=colorArg)
        d.append(rect) 

d.setPixelScale(5)
d.saveSvg('example.svg')