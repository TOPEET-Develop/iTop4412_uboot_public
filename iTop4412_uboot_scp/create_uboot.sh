./build_uboot.sh clean

ls

sleep 2

#cp include/configs/tc4_plus_pop_android.h include/configs/tc4_plus_android.h

#cp include/configs/tc4_plus_no_charge_android.h include/configs/tc4_plus_android.h
if [ -z $1 ]
then
./build_uboot.sh
else
./build_uboot.sh $1
fi

ls
