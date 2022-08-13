#!/bin/bash

WORK_PATH="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"

function Env_init()
{
	os_bit=$(getconf LONG_BIT)bit
    modules_path=$(uname -r)
    kernel_ver=${modules_path%-*}

    echo -e "\n  /~~~~~~~~~~~~ Pi Type ~~~~~~~~~~~~\\"
    echo -e "  |   1) Pi3       ·   2) Pi4       |"
    echo -e "  \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/\n"
    read -p " Please choose the hardware version type: " pi_type

    case "$pi_type" in
        1) pi_type=Pi3 ;;
        2) pi_type=Pi4 ;;
        *) echo "Selection error!"
        exit 1 ;;
    esac
}

function cfg_Insertion()
{
	if grep -w -q ^$2 $1
	then
	    echo "It already exists and does not need to be added"
	else
	    echo $2 >> $1
	fi
    sync
}

function modify_config()
{
    cd /boot
    sudo sed -i '/dtoverlay=vc4-fkms-v3d/d' config.txt
    sudo sed -i '/dtoverlay=vc4-kms-v3d/d' config.txt
    cfg_Insertion config.txt  "dtoverlay=rpi-ft5406"
    cfg_Insertion config.txt  "dtoverlay=vc4-kms-v3d"
    cfg_Insertion config.txt  "dtoverlay=vc4-kms-dsi-7inch"
    sync
}

if [ $# == 0 ]; then
    Env_init
    cd $WORK_PATH/$pi_type/$os_bit/$kernel_ver
    ./update.sh
else
    echo -e "\n Invalid parameters!"
    exit 0
fi

unset WORK_PATH

sudo reboot
