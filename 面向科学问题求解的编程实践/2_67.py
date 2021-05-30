
a = 2 ** 67 -1
print(a)
flag = 1
i = 3

while flag:
    if a % i == 0:
        print(i)
        print(a/i)
        flag = 0
    i = i + 2

