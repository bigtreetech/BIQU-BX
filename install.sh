#!/bin/bash

WORK_PATH="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"

function env_init()
{
    os_bit=$(getconf LONG_BIT)bit
    kernel_ver=$(uname -r)
    model_sn=`uname -r | awk -F '-' '{print $2}'`
    case "$model_sn" in
           +) model="Pi1, Zero, Zero W, CM1 32bit" ;;
         v7+) model="Pi3 32bit" ;;
        v7l+) model="Pi4 32bit" ;;
         v8+) model="Pi3, Pi4 64bit" ;;
           *) model="Unable to automatically recognize model!"
    esac

    echo -e "\n  /~~~~~~~~ OS Info ~~~~~~~~\\"
    echo -e "  |  Current OS: $os_bit      "
    echo -e "  |  Kernel ver: $kernel_ver  "
    echo -e "  |    Model SN: $model_sn    "
    echo -e "  |       Model: $model       "
    echo -e "  \~~~~~~~~~~~~~~~~~~~~~~~~~~~/\n"
}

function env_unknow()
{
    echo -e "\n  /~~~~~~~~~~~~~~~~~~~~~~~ NOTICE ~~~~~~~~~~~~~~~~~~~~~~~~\\"
    echo -e "  |  Sorry, The current OS_version is not in the support  |"
    echo -e "  |  list, please pay attention to our next updates.      |"
    echo -e "  \~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/\n"
}

function modify_modules_dtbo()
{
  # init data
  dtbo_btt_bxv3="vc4-kms-dsi-btt-bxv3-7inch.dtbo"
  ko_touch="edt-ft5x06.ko.xz"
  ko_panel="panel-simple.ko.xz"
  path_dtbo="/boot/overlays"
  path_touch="/lib/modules/${kernel_ver}/kernel/drivers/input/touchscreen"
  path_panel="/lib/modules/${kernel_ver}/kernel/drivers/gpu/drm/panel"

  # copy dtbo & .ko modules
  echo -e "\nCopy files...\n"

  echo -e "\nCopy $dtbo_btt_bxv3 to $path_dtbo...\n"
  sudo cp $dtbo_btt_bxv3 $path_dtbo -f

  echo -e "\nCopy ${kernel_ver}/${ko_touch} to $path_touch...\n"
  sudo cp ${kernel_ver}/${ko_touch} $path_touch -f

  echo -e "\nCopy ${kernel_ver}/${ko_panel} to $path_panel...\n"
  sudo cp ${kernel_ver}/${ko_panel} $path_panel -f

  sync
}

function install_modules()
{
  # install modules
  echo -e "\nInstall modules..."
  echo -e "\ndepmoding...\n"
  sudo depmod
  echo -e "\ndepmoded!\n"
  sudo modprobe -r edt_ft5x06
  sudo modprobe -r panel_simple
  sudo modprobe edt_ft5x06
  [ $? != 0 ] && echo -e "\nInstall $ko_touch failed! \n" && exit 1
  sudo modprobe panel_simple
  [ $? != 0 ] && echo -e "\nInstall $ko_panel failed! \n" && exit 1
  sync
}

function cfg_insert()
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
    cfg_insert config.txt  "ignore_lcd=1"
    cfg_insert config.txt  "dtoverlay=vc4-kms-v3d"
    cfg_insert config.txt  "dtparam=i2c_vc=on"
    cfg_insert config.txt  "dtparam=i2c_arm=on"
    cfg_insert config.txt  "dtoverlay=vc4-kms-dsi-btt-bxv3-7inch"
    sync
}

# starting...
env_init
cd $WORK_PATH/$kernel_ver &> /dev/null
[ $? != 0 ] && env_unknow && exit 1
cd $WORK_PATH
unset WORK_PATH

modify_modules_dtbo

install_modules

modify_config

echo -e "\nInstallation completed!\nRebooting now..."

sudo reboot
