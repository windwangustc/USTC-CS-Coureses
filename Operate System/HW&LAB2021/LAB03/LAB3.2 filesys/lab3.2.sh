sudo sh -c "echo never > /sys/kernel/mm/transparent_hugepage/enabled"
sudo sh -c "echo never > /sys/kernel/mm/transparent_hugepage/defrag"
sudo cat /proc/kallsyms | grep follow_page |sed -n '$p'
sudo cat /proc/kallsyms | grep page_referenced|sed -n '$p'
sed -i "s/mfollow_page = (my_follow_page)0x[a-z0-9]\{16\}/mfollow_page = (my_follow_page)0x$(sudo cat /proc/kallsyms | grep follow_page |sed -n '$p'|grep -o "[a-z0-9]\{16\}")/g" sysfs_test.c
sed -i "s/mpage_referenced = (my_page_referenced)0x[a-z0-9]\{16\}/mpage_referenced = (my_page_referenced)0x$(sudo cat /proc/kallsyms | grep page_referenced|sed -n '$p'|grep -o "[a-z0-9]\{16\}")/g" sysfs_test.c
echo "-------------------------"
sudo rmmod helloworld.ko
sudo rmmod sysfs_test.ko
sudo dmesg -C
sudo make clean
make
sudo insmod sysfs_test.ko
lsmod | grep sysfs_test
sudo sh -c "echo 2 > /sys/kernel/mm/kmscan/func" 
sudo sh -c "echo 3191 > /sys/kernel/mm/kmscan/pid" 
sudo sh -c "echo 1 > /sys/kernel/mm/kmscan/run"
sudo dmesg
echo "-----------------"
echo "active_anon :$(cat /proc/kmscan/*/active_anon)";
echo "active_file :$(cat /proc/kmscan/*/active_file)";
echo "anon :$(cat /proc/kmscan/*/anon)";
echo "file :$(cat /proc/kmscan/*/file)";
echo "vma_count :$(cat /proc/kmscan/*/vma_count)";
#sudo sh -c "echo 0 > /sys/kernel/mm/kmscan/run"
#sudo rmmod sysfs_test.ko


