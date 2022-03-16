# Convert a level pack from .txt to the packed .bin format
# Arg 1 = Input File Path
# Arg 2 = Output File Path

# Level Format:
# Levels are saved in packs, accessed using a pointer to the beginning of the pack and the level index.
# Byte 1: Number of nodes in the level
# Byte 2: High 4 bits = level width, low 4 bits = level height
# Following bytes: Each byte describes a node.
# High 4 bits = Node X, low 4 bits = Node Y
import sys
import math

nodeChars = ["1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "g"]

inputText = ""
with open(sys.argv[1], 'r') as f:
    inputText = f.read()

outByteArray = bytearray(0)
inputLevelArray = inputText.split("Level ")[1:] # remove first entry, which is empty
for inputLevel in inputLevelArray:
    inputLevel = inputLevel.split("\n")[1:] # remove level number
    levelWidth = int(inputLevel[0].split(" ")[0][2:]) # parse width and height from first line
    levelHeight = int(inputLevel[0].split(" ")[1][2:])
    inputLevel = "".join(inputLevel[1:]).replace("\n", "") # recombine the puzzle definition into a single string

    levelByteArray = bytearray(0)

    levelByteArray.append(0) # placeholder for node count
    levelByteArray.append(((levelWidth - 1) << 4) | levelHeight - 1)

    nodeIndex = 0
    for nodeIndex, node in enumerate(nodeChars):
        curPos = 0
        node1Pos = None
        node2Pos = None
        for c in inputLevel:
            if c == node:
                if node1Pos == None:
                    node1Pos = curPos
                else:
                    node2Pos = curPos
                    break
            curPos += 1
        
        if (node1Pos == None):
            break
        levelByteArray.append(((node1Pos % levelWidth) << 4) | math.floor(node1Pos / levelWidth))
        levelByteArray.append(((node2Pos % levelWidth) << 4) | math.floor(node2Pos / levelWidth))
    levelByteArray[0] = nodeIndex * 2
    outByteArray += levelByteArray

with open(sys.argv[2], "wb") as f:
    f.write(outByteArray)