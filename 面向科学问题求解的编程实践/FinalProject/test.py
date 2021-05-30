__author__ = 'PE20060014'

import glob
import sys
import os
import numpy as np
import scipy.stats as stats
import math
import matplotlib.pyplot as plt
import datetime


# calculate the gini index
def gini(initial_money, people, graph):
    cumul = 0
    ave = np.mean(graph)
    #   ave = 100
    print(ave)
    max = ave * people * (people + 1) / 2
    for i in range(people):
        cumul += (people - i) * (ave - graph[i])
    gini_index = cumul / max
    return gini_index


# the guassian fitline

def fit1(initial_money, people, step):
    maximum = 2 * initial_money
    dev = (step * (people - 1) / people) ** 0.5
    y = np.arange(0.1, maximum + 0.1, 0.1)
    y = np.reshape(y, (len(y), 1))
    x = stats.norm.cdf(y, initial_money, dev)
    x = np.reshape(x, (len(x), 1))
    fitline = np.hstack((people * x, y))
    return fitline


# the exponential fitline

def fit2(initial_money, people, step):
    x = np.arange(0.5, 0.5 + people)
    y = np.zeros(people)
    for i in range(people):
        y[i] = -initial_money * math.log(1 - x[i] * 1.0 / people)
    x = np.reshape(x, (len(x), 1))
    y = np.reshape(y, (len(y), 1))
    fitline = np.hstack((x, y))
    return fitline


# the experiment of changing the income and spend, optional

def reform(initial_money, people, graph):
    # rerank the money
    for i in range(people):
        graph[2, i] = 1
        for j in range(people):
            if graph[1, j] > graph[1, i]:
                graph[2, i] += 1
    #    for i in range(people):
    #        graph[4,i] = 0.111 * graph[1,i]**0.5  # the salary of one person
    #        graph[3,i] = 0.1 + 0.1 * graph[1,i]**0.5

    return graph


people = 100
step = 100000
initial_money = 100.0
interval = 100

graph = np.zeros(5 * people)
graph = np.reshape(graph, (5, people))
# 0 is for number, 1 is for money, 2 is for rank, 3 is for time when someone's money get zero, 4 is for number of earning

for i in range(people):
    graph[0, i] = i + 1
graph[1, :] = initial_money
graph[2, :] = 1
graph[3, :] = 0
graph[4, :] = 1.0

gini_list = []
print(graph)

for i_step in range(step):
    for j_p in range(people):
        graph[1, j_p] -= 1.0
        rand = np.random.random(1)

        k_select = int(rand * people)
        if graph[1, j_p] < 0:
            k_select = j_p
            graph[3, j_p] = i_step
        #          graph[1,j_p] += 1
        # ----------the method of weighted sampling
        # totalweight = sum(graph[3,:])
        # ------------------------
        #    for k_select in range(people):
        #    rand = rand - graph[3,k_select]/totalweight
        #    if rand<0:
        #        break
        # --------------------------
        graph[1, k_select] += graph[4, k_select]
    #    print graph[1,:]
    mini = min(graph[1, :])
    average = np.mean(graph[1, :])
    print(i_step)
    print(mini, average)
    if ((i_step + 1) % interval == 0):
        rank = sorted(graph[1, :])
        gini_index = gini(initial_money, people, rank)
        gini_list.append(gini_index)
        #    graph = reform(initial_money, people, graph)

        # -----------------draw the picture ---------------
        if 1 == 1:
            plt.bar(np.arange(0.5, 0.5 + len(rank)), rank)
            plt.title("time: " + str(i_step + 1) + "     gini:" + str(gini_index))
            plt.ylim((0, 600))
            plt.legend()
            plt.draw()
            plt.pause(0.00001)
            plt.close()

# if mini<1:
#     break

#np.savetxt("gini_index.txt", gini_list)
print("-----------", i_step)
step = i_step
print(graph[1, :])

# gini_index = gini(initial_money,people,rank)
# print gini_index
rank = sorted(graph[1, :])
# fitline = fit1(initial_money,people,step)

fitline = fit2(initial_money, people, step)
# print fitline
plt.bar(np.arange(0.5, 0.5 + len(rank)), rank)
plt.ylim((0, 7 * initial_money))
plt.savefig("experiment.png")
plt.plot(fitline[:, 0], fitline[:, 1], 'r')
np.savetxt("rank.dat", rank)
plt.savefig("experiment-fit.png")
plt.show()

graph = reform(initial_money, people, graph)
print("money, zerotime, ranking ")
for i in range(people):
    print(i, "\t   ", graph[1, i], "\t   ", graph[3, i], "\t   ", graph[2, i])


