import subprocess
import math

# Level Format:
# Levels are saved in packs, accessed using a pointer to the beginning of the pack and the level index.
# Byte 1: Number of nodes in the level
# Byte 2: High 4 bits = level width, low 4 bits = level height
# Following bytes: Each byte describes a node.
# High 4 bits = Node X, low 4 bits = Node Y

# since the nodes in numberlink start at 1, we can't directly convert to hex
def convertNodeNum(num):
    if (num == 'g'):
        return 16
    else:
        return int(num, 16) - 1

def genPack(width, height, numLevels, name):
    outPuzArray = bytearray(0)

    for i in range(numLevels):
        outPuz = bytearray(0)

        startPuz = subprocess.check_output(['python', 'numberlink/gen/gen.py', '--max', '16', '--terminal-only', '--no-colors', str(width), str(height), '1'])

        # remove starting line with puzzle dimensions
        startPuz = startPuz.split(b'\n', 1)[1]
        #print (startPuz.decode('utf-8'))
        # remove all other newlines
        startPuz = startPuz.decode('utf-8').replace("\n", "")

        outPuz.append(0) # placeholder for node count
        outPuz.append(((width - 1) << 4) | height - 1)

        curNodeNum = 0
        while (True):
            curPos = 0
            node1Pos = None
            node2Pos = None
            for c in startPuz:
                if c != '.' and convertNodeNum(c) == curNodeNum:
                    if node1Pos == None:
                        node1Pos = curPos
                    else:
                        node2Pos = curPos
                        break
                curPos += 1

            if (node1Pos == None):
                break
            outPuz.append(((node1Pos % width) << 4) | math.floor(node1Pos / width))
            outPuz.append(((node2Pos % width) << 4) | math.floor(node2Pos / width))
            #print("node " + str(curNodeNum) + "  x: " + str(node1Pos % width) + "  y: " + str(math.floor(node1Pos / width)))
            #print("node " + str(curNodeNum) + "  x: " + str(node2Pos % width) + "  y: " + str(math.floor(node2Pos / width)))
            curNodeNum += 1
        outPuz[0] = (curNodeNum - 1) * 2
        outPuzArray += outPuz
        #print (outPuz.hex())
    with open("levels/" + name + ".bin", "wb") as f:
        f.write(outPuzArray)
    print("Generated " + name)

genPack(8, 9, 100, "testlevels")