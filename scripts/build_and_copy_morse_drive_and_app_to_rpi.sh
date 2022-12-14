#!/bin/bash

DRIVER_LOCATION=driver
APP_LOCATION=app
SCRIPTS_LOCATION=scripts
DRIVER_AND_APP_DESTINATION_PATH=/home/student/linux-kernel-labs/modules/nfsroot/root
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

clean_make_driver() {
    echo "[DRIVER] Clean and make..."
    cd ../$DRIVER_LOCATION
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
    sudo cp $driver_path $DRIVER_AND_APP_DESTINATION_PATH
    check_return_code_and_exit_if_error $?

    app_path=$APP_LOCATION/bin/app
    sudo cp $app_path $DRIVER_AND_APP_DESTINATION_PATH
    check_return_code_and_exit_if_error $?
    echo "Done copying."
}

copy_run_script_to_rpi() {
    echo "Copy run script to rpi..."
    run_script_path=$SCRIPTS_LOCATION/run_morse.sh
    sudo cp $run_script_path $DRIVER_AND_APP_DESTINATION_PATH
    check_return_code_and_exit_if_error $?
    echo "Done copying."
}

clean_make_driver
clean_make_app
copy_driver_and_app_to_rpi
copy_run_script_to_rpi

exit 0