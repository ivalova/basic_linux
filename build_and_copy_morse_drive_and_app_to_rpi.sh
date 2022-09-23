#!/bin/bash

ROOT_UID=0
E_NOTROOT=87
DRIVER_LOCATION=driver
APP_LOCATION=app
DRIVER_AND_APP_DESTINATION_PATH=/srv/tftp/
KDIR_PATH=/home/student/linux-kernel-labs/src/linux/

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-


check_return_code_and_exit_if_error() {
    if [ $1 -ne 0 ]
    then
        echo -e "\e[31mERROR: previous command failed. Aborting the script...\e[0m"
        exit 1
    fi
}

check_if_user_is_root() {
    if [ "$UID" -ne "$ROOT_UID" ]
    then
        echo "Must be root to run this script."
        exit $E_NOTROOT
    fi
}

clean_make_driver() {
    echo "[DRIVER] Clean and make..."
    cd $DRIVER_LOCATION
    make clean
    check_return_code_and_exit_if_error $?
    make KDIR=$KDIR_PATH
    check_return_code_and_exit_if_error $?
    echo "[DRIVER] Clean and make done."
}

clean_make_app() {
    echo "[APP] Clean and make..."
    cd ..
    cd $APP_LOCATION
    make clean
    check_return_code_and_exit_if_error $?
    make
    check_return_code_and_exit_if_error $?
    echo "[APP] Clean and make done."
}

copy_driver_and_app_to_rpi() {
    echo "Copy driver and app to rpi..."
    cd ..
    driver_path=$DRIVER_LOCATION/morse.ko
    cp $driver_path $DRIVER_AND_APP_DESTINATION_PATH
    check_return_code_and_exit_if_error $?

    app_path=$APP_LOCATION/bin/app
    cp $app_path $DRIVER_AND_APP_DESTINATION_PATH
    check_return_code_and_exit_if_error $?
    echo "Done copying."
}

check_if_user_is_root
clean_make_driver
clean_make_app
copy_driver_and_app_to_rpi

exit 0