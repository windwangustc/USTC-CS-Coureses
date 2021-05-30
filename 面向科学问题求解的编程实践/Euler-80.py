
print('####### https://projecteuler.net/ Euler-80 ###############')


import decimal
# import math

decimal.getcontext().prec = 140

sum = 0

for i in range(1,100):
    a = decimal.Decimal(i) ** decimal.Decimal(.5)
#    print(a)
    sum = sum + int(str(a)[0])
    for j in range(2,101):
        sum = sum + int(str(a)[j])
#    print(sum)

sum = sum - 45

print(sum)

