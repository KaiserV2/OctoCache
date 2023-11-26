# plot me a box chart figure
# x axis is the "morton order", "order by x", "order by y", "order by z", "random order"
# I give you the data for "morton order", "order by x", "order by y", "order by z", "random order", by 10%, 30%, 70% and 90% runtime numbers

morton = [0.135, 0.135, 0.135, 0.135]
x = [0.158, 0.158, 0.158, 0.158]
y = [0.162, 0.162, 0.162, 0.162]
z = [0.171, 0.171, 0.171, 0.171]
octomap = [0.191, 0.191, 0.191, 0.191]
random = [0.290, 0.315, 0.359, 0.386]


import matplotlib.pyplot as plt
import numpy as np

import matplotlib
matplotlib.use('Agg')

# plot the 5 boxes of morton, x, y, z, random
# the 5 legends should be above the figure and in a row

# the x axis range shall be 0 to 4
plt.xlim(0, 4)

# x axis is the "morton order", "order by x", "order by y", "order by z", "random order"
# plt.xlabel("morton order, order by x, order by y, order by z, random order", fontsize=20)
# y axis is the "runtime"
plt.ylabel("runtime in s", fontsize=20)

# now plot the boxplot
plt.boxplot([morton, x, y, z, octomap, random], labels=["morton", "order by x", "order by y", "order by z", "octomap", "random order"])

# save the figure to "boxplot.png"
plt.savefig("boxplot.png")
