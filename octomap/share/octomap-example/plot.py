import numpy as np
import matplotlib.pyplot as plt


datasetNum = 1
sizeNum = 7
kickNum = 6
x = ["1","2","4","8","16","32","64"]

baseline = np.empty([3,1]) # record the mean value
lowerBound = np.empty([3,1]) # the ray tracing time as the lower bounds
raytracingRatio = np.array([0.7376, 0.4716, 0.2832])
cache = np.empty([3,sizeNum,kickNum]) # 3 datasets, 5 cache size, 3 kick speed


lineCount = 0

minValue = np.empty([10000,1])
maxValue = np.empty([10000,1])
medianValue = np.empty([10000,1])
meanValue = np.empty([10000,1])

lineCount = 0

# read the first 3 lines for baseline
with open('log.txt','r') as file_handle:
    for line in file_handle:
        data = line.split(",")
        data.pop()
        data = np.array(data)
        data = data.astype(np.float)
        data = np.sort(data)
        minValue[lineCount] = data[0]
        maxValue[lineCount] = data[-1]
        medianValue[lineCount] = np.median(data)
        meanValue[lineCount] = np.mean(data)
        lineCount += 1

lineCount = 0

for i in range(datasetNum):
    baseline[i] = meanValue[lineCount]
    lowerBound[i] = baseline[i] * raytracingRatio[i]
    lineCount += 1

for i in range(3):
    for j in range(sizeNum):
        for k in range(kickNum):
            cache[i][j][k] = meanValue[lineCount]
            lineCount += 1

# plot the figure
for i in range(datasetNum):
    x = x[0:sizeNum]
    y = []
    # plot the baseline
    for j in range(sizeNum):
        y.append(baseline[i])
    plt.plot(x,y,label='Baseline')
    
    y = []
    for j in range(sizeNum):
        y.append(lowerBound[i])
    plt.plot(x,y,label='Ray tracing time(lower bound)')
    # plt.show()
    for j in range(kickNum): # draw 3 lines of different eviction speed
        y = []
        for k in range(sizeNum):
            y.append(cache[i][k][j])
        print(y)
        size = pow(2,j)
        name = "Holding x"+str(size)+"PCs"
        plt.plot(x,y,label=name)
    plt.xlabel("Cache size is X times of the average PC size")
    plt.ylabel("End to end runtime(s)")
    # plt.xscale('log')
    plt.legend(loc='best')
    plt.savefig("Figures/dataset" + str(i+1) + ".png")
    plt.clf()


# read the remaining lines for cache (with size and eviction speed)

# for c in {15,16,17}:
#     y = []
#     for i in range(1<<c):
#         y.append(i / (1<<c) * 100)
#     for s in {5,10}:
#         lineCount = 0
#         data = []
#         for i in range(10):
#             data.append([])

#         # draw LG
#         filename = 'c' + str(c) + '_s' + str(s) + 'LG.txt'
#         # filename = "test.txt"
#         with open(filename,'r') as file_handle:
#             for line in file_handle:
#                 data[lineCount] = line.split(" ")
#                 data[lineCount].pop()
#                 lineCount += 1

#         for i in range(lineCount):
#             x = []
#             y = []
#             for item in data[i]:
#                 x.append(int(item))
#                 y.append((len(y) + 1) / HHNum)
#             plt.plot(x,y)
        
#         lineCount = 0
#         data = []
#         for i in range(10):
#             data.append([])
#         # draw DISCO
#         filename = 'c' + str(c) + '_s' + str(s) + 'DISCO.txt'
#         # filename = "test.txt"
#         with open(filename,'r') as file_handle:
#             for line in file_handle:
#                 data[lineCount] = line.split(" ")
#                 data[lineCount].pop()
#                 lineCount += 1

#         for i in range(1):
#             x = []
#             y = []
#             for item in data[i]:
#                 x.append(int(item))
#                 y.append((len(y) + 1) / HHNum)
#             plt.plot(x,y)

#         plt.title("Sketch size 2^" + str(c) + " sample rate 1/" + str(int(100/s)))
#         plt.xlabel("Packet count at switch")
#         plt.ylabel("Heavy hitter detected ratio")
#         plt.savefig("logc" + str(c) + "s" + str(s) + ".png")
#         plt.clf()

