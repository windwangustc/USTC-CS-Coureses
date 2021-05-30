__author__ = 'PE20060014 Chen Wang'

import os
import math
import numpy as np
import random
import matplotlib.pyplot as plt
from savegif import create_gif

# 计算基尼系数
def gini(initial_money, people, wealth):
    cumul = 0
    ave = np.mean(wealth)
    #print(ave)
    max = ave * people * (people + 1) / 2
    for i in range(people):
        cumul += (people - i) * (ave - wealth[i])
    gini_index = cumul / max
    return gini_index

# 玻尔兹曼分布曲线
def fit1(initial_money, people, step):
    x = np.arange(0.5, 0.5 + people)
    y = np.zeros(people)
    for i in range(people):
        y[i] = -initial_money * math.log(1 - x[i] * 1.0 / people)
    x = np.reshape(x, (len(x), 1))
    y = np.reshape(y, (len(y), 1))
    fitline = np.hstack((x, y))
    return fitline

#S1:不允许负债的情况
def generate1(days):
    import random
    wealth = []
    order = []
    for i in range(100):    #定义100个人的序号
        order.append(i+1)
    for i in range(100):    #定义初始财富
        wealth.append(100)
    for j in range(days):   #模拟22-65岁 共15695天
        for i in range(100):    #模拟每天100个人的财富交换
            if wealth[i]==0:    #不可负债
                #break          #如果有人财富为0了 就停止游戏
                continue        #财富为0时，不支出，游戏继续
            else:
                wealth[i] = wealth[i] - 1
                rand_guy=random.choice([x for x in range(0,100)  ])  #100平均分布取出一个人
                wealth[rand_guy] = wealth[rand_guy] + 1
        wealth.sort()#对财富排序
        # 检验财富有无流失，return 10000则没有流失
        # print(sum(wealth))
        gini_index = gini(100, 100, wealth)  # 计算基尼系数
        #print(gini_index)
        # 显示100个人的财富柱状图
        #fig1 = plt.figure()
        #玻尔兹曼分布曲线
        fitline = fit1(100, 100, j)
        if ( j % 200 == 0):
            plt.plot(fitline[:, 0], fitline[:, 1], 'r')
            plt.bar(order,wealth,0.5,color="blue")
            plt.title('Scenario1:Debt Disabled\nDays:' + str(j) + '  Gini Index:' + str(gini_index))
            plt.xlabel('Order of participants')
            plt.ylabel('Wealth')
            #plt.legend()
            plt.draw()
            plt.pause(0.00001)
            plt.savefig('../pythonProject/Scenario1/'+j//200*str(1)+'.png')
            if(j!=days):            #保留最运行结束时的数据
                plt.close()
    gif_name = 'Scenario1.gif'
    pic_path = '../pythonProject/Scenario1/'  # 指定文件路径
    duration = 0.1
    create_gif(gif_name, pic_path, duration)
    return wealth, order

#S2:允许负债的情况
def generate2(days):
    wealth = []
    order = []
    for i in range(100):
        order.append(i+1)
    for i in range(100):
        wealth.append(100)
    for j in range(days):#可负债情况不需判断某人的金钱是否为0
        for i in range(100):
            wealth[i]=wealth[i]-1
            rand_guy=random.choice([x for x in range(0,100) if x!=i])
            wealth[rand_guy]=wealth[rand_guy]+1
        wealth.sort()
        gini_index = gini(100, 100, wealth)  # 计算基尼系数
    # 显示100个人的财富柱状图
    # fig1 = plt.figure()
        if ( j % 200 == 0):
            plt.bar(order,wealth,0.5,color="blue")
            plt.title('Scenario2:Debt Enabled\nDays:' + str(j) + '  Gini Index:' + str(gini_index))
            plt.xlabel('Order of participants')
            plt.ylabel('Wealth')
            #plt.legend()
            plt.draw()
            plt.pause(0.00001)
            plt.savefig('../pythonProject/Scenario2/'+j//200*str(1)+'.png')
            if(j!=days):            #保留最运行结束时的数据
                plt.close()
    gif_name = 'Scenario2.gif'
    pic_path = '../pythonProject/Scenario2/'  # 指定文件路径
    duration = 0.1
    create_gif(gif_name, pic_path, duration)
    return wealth, order
#wealth,order=generate()

#S3:对富人收税的情况：
def generate3(days):
    import random
    wealth = []
    order = []
    for i in range(100):
        order.append(i+1)
    for i in range(100):
        wealth.append(100.0)
    for j in range(days):
        for i in range(100):
            wealth[i]=wealth[i]-1
            rand_guy=random.choice([x for x in range(0,100) if x!=i])
            if wealth[rand_guy]>=200.0:#对拥有大于200元的玩家征收20%的税
                wealth[rand_guy] = wealth[rand_guy] + 0.5
                wealth_transi = wealth[:]
                for m in range(5):#对最贫穷的2位玩家每人补贴0.25元
                    ind=wealth_transi.index(min(wealth_transi))
                    wealth[ind]=wealth[ind]+0.1
                    wealth_transi[ind]=100000000
            else:
                wealth[rand_guy] = wealth[rand_guy] + 1
        wealth.sort()
        gini_index = gini(100, 100, wealth)  # 计算基尼系数
        # 显示100个人的财富柱状图
        # fig1 = plt.figure()
        if (j % 200 == 0):
            plt.bar(order,wealth,0.5,color="blue")
            plt.title('Scenario3:Debt and 50%TAX Enabled \nDays:' + str(j) + '  Gini Index:' + str(gini_index))
            plt.xlabel('Order of participants')
            plt.ylabel('Wealth')
            #plt.legend()
            plt.draw()
            plt.pause(0.00001)
            plt.savefig('../pythonProject/Scenario3/' + j // 200 * str(1) + '.png')
            if(j!=days):            #保留最运行结束时的数据
                plt.close()
    gif_name = 'Scenario3.gif'
    pic_path = '../pythonProject/Scenario3/'  # 指定文件路径
    duration = 0.1
    create_gif(gif_name, pic_path, duration)
    return wealth, order
#wealth,order=generate()

#S4：富二代出现
def generate4(days):
    wealth = []
    order = []
    for i in range(100):
        order.append(i+1)
    for i in range(10):#前10位玩家是富二代，生来就比普通玩家多2倍财富
        wealth.append(400.0)
    for i in range(10,100):
        wealth.append(100.0)
    for j in range(days):#模拟15695轮财富分配
        for i in range(100):
            wealth[i]=wealth[i]-1
            rand_guy=random.choice([x for x in range(0,100) if x!=i])
            wealth[rand_guy]=wealth[rand_guy]+1
        # 排序
        wealth_transi = wealth[:]
        wealth_sort = []
        order_rich = []
        wealth_rich = []
        order_normal = []
        wealth_normal = []
        for i in range(100):
            ind_min = wealth_transi.index(min(wealth_transi))
            wealth_sort.append(min(wealth_transi))
            wealth_transi[ind_min] = float('inf')
            if ind_min in [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]:
                order_rich.append(len(wealth_sort) - 1)
                wealth_rich.append(wealth_sort[-1])
            else:
                order_normal.append(len(wealth_sort) - 1)
                wealth_normal.append(wealth_sort[-1])
        gini_index = gini(100, 100, wealth_sort)  # 计算基尼系数
        print(gini_index)
        # barchart
        # fig1 = plt.figure()
        if (j % 200 == 0):
            plt.bar(order_rich, wealth_rich, 0.5, color="red")
            plt.bar(order_normal, wealth_normal, 0.5, color="blue")
            plt.title('Scenario4:Born Rich with 400\nDays:' + str(j) + '  Gini Index:' + str(gini_index))
            plt.xlabel('Order of participants')
            plt.ylabel('Wealth')
            plt.legend()
            plt.draw()
            plt.pause(0.00001)
            plt.savefig('../pythonProject/Scenario4/' + j // 200 * str(1) + '.png')
            if (j != days):  # 保留最运行结束时的数据
                plt.close()
        # plt.show()
    gif_name = 'Scenario4.gif'
    pic_path = '../pythonProject/Scenario4/'  # 指定文件路径
    duration = 0.1
    create_gif(gif_name, pic_path, duration)
    return wealth, order

#S5更努力有用吗？
def generate5(days):
    wealth = []
    order = []
    for i in range(100):
        order.append(i+1)
    for i in range(10):
        wealth.append(50)
    for i in range(10,90):
        wealth.append(100)
    for i in range(90,100):
        wealth.append(200)      #后10名为初始携带200元的富二代
    for j in range(days):
        for i in range(100):
            wealth[i]=wealth[i]-1
            rand_guylist1=[x for x in range(0,10) if x!=i]#定义0到9号玩家序号
            rand_guylist2 = [x for x in range(10, 100) if x != i]#定义10到99号玩家序号
            ind=random.randint(0,1000)
            # 前10位玩家得到钱的概率要微大于后90位玩家，几率大了将近万分之一，以此模拟10位更努力的玩家
            if ind<=100:#在0到9号玩家中取一个人的概率是11/1000
                rand_guy = random.choice(rand_guylist1)
            else:#在10到99号玩家中取一个人的概率是（1000-11）/1000
                rand_guy = random.choice(rand_guylist2)
            wealth[rand_guy]=wealth[rand_guy]+1
    #wealth.sort()#为展示前10位玩家财富变化，不进行排序
        # 排序
        wealth_transi = wealth[:]
        wealth_sort = []
        order_rich = []
        wealth_rich = []
        order_hard = []
        wealth_hard = []
        order_normal = []
        wealth_normal = []
        for i in range(100):
            ind_min = wealth_transi.index(min(wealth_transi))
            wealth_sort.append(min(wealth_transi))
            wealth_transi[ind_min] = float('inf')
            if ind_min in [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]:
                order_hard.append(len(wealth_sort) - 1)
                wealth_hard.append(wealth_sort[-1])
            elif ind_min in [90, 91, 92, 93, 94, 95, 96, 97, 98, 99]:
                order_rich.append(len(wealth_sort) - 1)
                wealth_rich.append(wealth_sort[-1])
            else:
                order_normal.append(len(wealth_sort) - 1)
                wealth_normal.append(wealth_sort[-1])
            # print(len(order_hard)==len(order_hard),len(order_rich)==len(wealth_rich),len(order_normal)==len(wealth_normal))
        gini_index = gini(100, 100, wealth_sort)  # 计算基尼系数
        # barchart
        # fig1 = plt.figure()
        if (j%200==0):
            plt.bar(order_rich, wealth_rich, 0.5, color="red")
            plt.bar(order_hard, wealth_hard, 0.5, color="green")
            plt.bar(order_normal, wealth_normal, 0.5, color="blue")
            plt.title('Scenario5:Hard-working by 0.01% VS Born Rich 200%\nDays:' + str(j) + '  Gini Index:' + str(gini_index))
            plt.xlabel('Order of participants')
            plt.ylabel('Wealth')
            plt.legend()
            plt.draw()
            plt.pause(0.00001)
            plt.savefig('../pythonProject/Scenario5/' + j // 200 * str(1) + '.png')
            if (j != days):  # 保留最运行结束时的数据
                plt.close()
            # plt.show()
    gif_name = 'Scenario5.gif'
    pic_path = '../pythonProject/Scenario5/'  # 指定文件路径
    duration = 0.1
    create_gif(gif_name, pic_path, duration)
    return wealth, order



def main():
    day=15695
    generate1(day)

if  __name__ == '__main__':
    main()