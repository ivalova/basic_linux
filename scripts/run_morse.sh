#!/bin/bash

ROOT_UID=0
E_NOTROOT=87
MODULE=morse
DEVICE=morse
NAME=morse

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

insert_driver() {
    echo "Inserting driver..."
    sudo insmod $MODULE.ko
    check_return_code_and_exit_if_error $?
    echo "Inserting driver done."
}

create_char_device() {
    echo "Creating char device..."
    rm -f /dev/$DEVICE
    major=`awk "\\$2==\"$NAME\" {print \\$1}" /proc/devices`
    mknod /dev/$DEVICE c $major 0
    check_return_code_and_exit_if_error $?
    echo "Creating char device done."
}

check_if_user_is_root
insert_driver
create_char_device

exit 0