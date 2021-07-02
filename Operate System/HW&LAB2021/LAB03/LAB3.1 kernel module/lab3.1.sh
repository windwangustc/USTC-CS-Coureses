uname -r
echo "-------------------------"
sudo rmmod helloworld.ko
sudo rmmod sysfs_test.ko
sudo dmesg -C
sudo make clean
sudo make
sudo insmod helloworld.ko func=3 pid=3191
lsmod | grep helloworld
sudo dmesg
