import subprocess
import math

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
        outPuz.append((width << 4) | height)

        curNodeNum = 1
        while (True):
            curPos = 0
            node1Pos = None
            node2Pos = None
            for c in startPuz:
                if c != '.' and int(c, 16) == curNodeNum:
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
        outPuz[0] = curNodeNum - 1
        outPuzArray += outPuz
        #print (outPuz.hex())
    with open("levels/" + name + ".bin", "wb") as f:
        f.write(outPuzArray)
    print("Generated " + name)

genPack(8, 9, 100, "testlevels")