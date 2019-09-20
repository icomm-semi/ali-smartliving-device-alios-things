#!/bin/sh
#gen_firmware.sh

#Firmware image path
project_example=living_platform
project_board=hf-lpt230
product_vendor=ali
product_name=smart_living
user_release_note=your_path_to_your_release_note
user_readme=your_path_to_your_readme_file
onepack_image_path_root=${product_vendor}_${product_name}_onepack_image
example_config_file=example/${project_example}/${project_example}.mk
project_out_path=./out/${project_example}@${project_board}/binary
fw_out_filename=fw-${product_vendor}_${product_name}
version_config_string="CONFIG_SYSINFO_APP_VERSION = "

#cross complier path
cross_compile_path=${HOME}/.bin/gcc-arm-none-eabi/Linux64/bin
aos_cube=aos

#set -x

#Firmware type
fw_type="release"
FW_DEBUG=0

function get_firmware_names()
{
        fw_out_org_filename=${project_example}@${project_board}
        fw_out_org_file=${project_out_path}/${fw_out_org_filename}
        if [[ $project_board == "hf-lpt230" || $project_board == "hf-lpt130" || $project_board == "hf-lpb130" || $project_board == "hf-lpb135" || $project_board == "uno-91h" ]]; then
                fw_out_combine_filename=${fw_out_org_filename}_all
        elif [ $project_board == "mk3080" ]; then
                fw_out_combine_filename=${fw_out_org_filename}.all
        else
                fw_out_combine_filename=${fw_out_org_filename}
        fi
        fw_out_combine_file=${project_out_path}/${fw_out_combine_filename}
        fw_upgrade_filename=ota-${product_vendor}_${product_name}
        if [[ $project_board == "hf-lpt230" || $project_board == "hf-lpt130" || $project_board == "hf-lpb130" || $project_board == "hf-lpb135" || $project_board == "uno-91h" ]]; then
                fw_upgrade_org_filename=${fw_out_org_filename}_ota
        elif [ $project_board == "mk3080" ]; then
                fw_upgrade_org_filename=ota_all
        else
                fw_upgrade_org_filename=${fw_out_org_filename}
        fi
        fw_upgrade_org_file=${project_out_path}/${fw_upgrade_org_filename}
}

function check_firmware_type()
{
	if [ "$1" == "debug" ]; then
		fw_type="debug"
		FW_DEBUG=1
	else
		fw_type="release"
		FW_DEBUG=0
	fi
}

function toggle_firmware_type()
{
        if [ ${fw_type} == "debug" ]; then
                fw_type="release"
                FW_DEBUG=0
        else
                fw_type="debug"
                FW_DEBUG=1
        fi
}

function update_compile_time()
{
	compile_date=`date +"%Y%m%d%H%M%S"`
}

function update_firmware_version()
{
	update_compile_time
        local version_line=`grep "$version_config_string" ${example_config_file}`
        version_line="${version_line##* = }"
        version_line="${version_line%%\$*}"
        firmware_version=${version_line}${compile_date}

}

function apply_patchs()
{
        # No SSL workaround default
        git checkout security/imbedtls/include/mbedtls/ssl.h
        if [ $project_board == "mk3080" ]; then
                patch -p1 < example/${project_example}/workaround/fix_rtl8710_fw_name.patch
        fi

        # Scan for patchs
        if [ "`ls -A ${PWD}/example/${project_example}/patchs/`" = "" ]; then
                echo "no patch found!"
                return 0
        fi
        # Apply patchs
        # git am example/${project_example}/patchs/*
        #patch -p1 < example/${project_example}/patchs/*
        for file in example/${project_example}/patchs/*; do
                if ! patch -R -p1 -s -f --dry-run < $file; then
                        echo "Patching==>$file"
                        patch -p1 < $file
                        if [ $? -ne 0 ]; then
                                echo "apply patch failed!!!"
                                exit 1
                        fi
                else
                        echo "Patch==>$file: already applied !"
                fi
        done
}

function build_clean()
{
	$aos_cube make clean
	echo 'Clean linkkit sdk'
	rm -rf framework/protocol/linkkit/sdk/iotx-sdk-c_clone/.O/
	rm -rf framework/protocol/linkkit/sdk/iotx-sdk-c_clone/.config
	rm -rf framework/protocol/linkkit/sdk/iotx-sdk-c_clone/output/
	git checkout framework/protocol/linkkit/sdk/iotx-sdk-c_clone/
}

function build_firmware()
{
	# Need workaround for SSL
	git checkout security/imbedtls/include/mbedtls/ssl.h
	if [ "$2" == "ON_PRE" ]; then
		echo "Workaround for SSL..................."
		patch -p1 < example/${project_example}/workaround/pre_ssl_workaround.patch
		if [ $? -ne 0 ]; then
			git checkout security/imbedtls/include/mbedtls/ssl.h
			echo "SSL workaround failed!!!"
			exit 1
		fi
	fi
	# build_clean
	$aos_cube make ${project_example}@${project_board} CONFIG_SERVER_REGION=$1 CONFIG_SERVER_ENV=$2 CONFIG_COMPILE_DATE=$3 CONFIG_DEBUG=${FW_DEBUG}
	git checkout security/imbedtls/include/mbedtls/ssl.h
	if [ $? -ne 0 ]; then
		exit 1
	fi
	# Copy and rename
	local server_region=`echo "$1" | tr '[:upper:]' '[:lower:]'`
	local server_env=`echo "$2" | tr '[:upper:]' '[:lower:]'`
	fw_out_filename_full=${fw_out_filename}-${server_region}-${server_env}-${firmware_version}-${fw_type}
	fw_ota_filename_full=${fw_upgrade_filename}-${server_region}-${server_env}-${firmware_version}-${fw_type}
	cp ${fw_out_combine_file}.bin ${project_out_path}/${fw_out_filename_full}.bin
	cp ${fw_out_org_file}.elf ${project_out_path}/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${project_out_path}/${fw_ota_filename_full}.bin
}

function gen_readme()
{
	echo "-------------------------Firmware info for ada project---------------------------" > $readme_file
	echo "Host: ${USER}" >> $readme_file
	BRANCH=`git rev-parse --abbrev-ref HEAD`
	COMMIT_HASH=`git rev-parse HEAD`
	echo "Branch: ${BRANCH}" >> $readme_file
	echo "Hash: ${COMMIT_HASH}" >> $readme_file
	AOS_VERSION_STRING=`grep "SYSINFO_KERNEL_VERSION" framework/protocol/linkkit/sdk/iotx-sdk-c_clone/aos_board_conf.mk`
	AOS_VERSION=${AOS_VERSION_STRING##*SYSINFO_KERNEL_VERSION=}
	AOS_VERSION=`echo $AOS_VERSION`
	AOS_VERSION=${AOS_VERSION%% *}
	echo "Kernel: ${AOS_VERSION}" >> $readme_file
	SDK_VERSION_STRING=`grep "LINKKIT_VERSION" framework/protocol/linkkit/sdk/iotx-sdk-c_clone/include/iot_export.h`
	SDK_VERSION=${SDK_VERSION_STRING##*LINKKIT_VERSION     }
	SDK_VERSION=`echo $SDK_VERSION`
	echo "LinkKit: ${SDK_VERSION}" >> $readme_file
	APP_VERSION_STRING=`grep "SYSINFO_APP_VERSION" framework/protocol/linkkit/sdk/iotx-sdk-c_clone/aos_board_conf.mk`
	APP_VERSION=${APP_VERSION_STRING##*SYSINFO_APP_VERSION=}
	APP_VERSION=${APP_VERSION%% *}
	APP_VERSION=`echo $APP_VERSION`
	echo "APP: ${APP_VERSION}" >> $readme_file
	echo "" >> $readme_file
	# echo "Bootloader: ${bootloader_filename}.bin" >> $readme_file
	echo "Combine firmware: ${fw_out_filename}-region-env-app-ver.bin" >> $readme_file
	# echo "APP firmware: ${fw_out_filename}-region-env-app-ver.bin" >> $readme_file
	echo "OTA firmware: ota/${fw_upgrade_filename}_region-env-app-ver.bin" >> $readme_file
	echo "factory firmware: fw-factory-${onepack_image_path_sub}.zip" >> $readme_file
	echo "" >> $readme_file
	# echo "Bootloader flash addr:            0x1000" >> $readme_file
	echo "Combine firmware flash addr:      0x1000" >> $readme_file
	# echo "APP firmware flash addr:          0x4000" >> $readme_file
	echo "Flash and debug uart baud rate:   921600" >> $readme_file
	echo "Firmware app uart baud rate:      115200 or 9600" >> $readme_file
	echo "" >> $readme_file
	echo "Flash device triple data:" >> $readme_file
	echo "1. Press key '+' for three times " >> $readme_file
	echo "2. When console prints'a', press key 'a', console will print '+OK'" >> $readme_file
	echo "3. Flash product secret: AT+PROSECRET=<product secret>" >> $readme_file
	echo "4. Flash device secret:  AT+PRODEVICE=<product key>,<device name>,<device secret>" >> $readme_file
}

function build_firmware_all()
{
	#onepack firmware path
	onepack_image_path_sub=fw-${product_vendor}_${product_name}_${firmware_version}_${fw_type}
	onepack_image_path=${onepack_image_path_root}/${onepack_image_path_sub}
	if [ ! -d "$onepack_image_path_root" ]; then
 		mkdir $onepack_image_path_root
	fi
	if [ -d "$onepack_image_path" ]; then
		rm -rf $onepack_image_path
	fi
	#mkdir $firmware_image_path
	mkdir $onepack_image_path
	mkdir $onepack_image_path/ota
	if [ "$1" == "factory_mode" ]; then
		mkdir $onepack_image_path/factory
	fi
	#mkdir $onepack_image_path/factory/ota
	mkdir $onepack_image_path/debug

	#build and copy firmware
	build_clean
	build_firmware  "MAINLAND"  "ONLINE" "${compile_date}"
	cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin
	if [ "$1" == "test_mode" ]; then
		cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
	else
		cp ${fw_out_combine_file}.bin ${onepack_image_path}/factory/${fw_out_filename_full}.bin
	fi
	build_clean
	build_firmware  "SINGAPORE"  "ONLINE" "${compile_date}"
	cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin
	if [ "$1" == "test_mode" ]; then
		cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
	else
		cp ${fw_out_combine_file}.bin ${onepack_image_path}/factory/${fw_out_filename_full}.bin
	fi
	if [ "$1" == "test_mode" ]; then
		build_clean
		build_firmware  "GERMANY"  "ONLINE" "${compile_date}"
		cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
		cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin
		cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
		build_clean
		build_firmware  "MAINLAND"  "ON_DAILY" "${compile_date}"
		cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
		cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin
		cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
		build_clean
		build_firmware  "MAINLAND"  "ON_PRE" "${compile_date}"
		cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
		cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin
		cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
		build_clean
		build_firmware  "SINGAPORE"  "ON_PRE" "${compile_date}"
		cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
		cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin
		cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
		build_clean
		build_firmware  "GERMANY"  "ON_PRE" "${compile_date}"
		cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
		cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin
		cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
	fi
	#Copy bootloader
	# cp ${bootloader_org_file}.bin $onepack_image_path/${bootloader_filename}.bin

	#MD5 checksum
	#md5sum $onepack_image_path/* > $onepack_image_path/checksum.md5
	md5sum $onepack_image_path/ota/* > $onepack_image_path/ota/checksum.md5
	md5sum $onepack_image_path/factory/* > $onepack_image_path/factory/checksum.md5
	#md5sum $onepack_image_path/factory/ota/* > $onepack_image_path/factory/ota/checksum.md5
	md5sum $onepack_image_path/debug/* > $onepack_image_path/debug/checksum.md5
	#Gen readme
	if [ ! -f "$user_reame" ]; then
		readme_file=$onepack_image_path/readme.md
		gen_readme
	else
		readme_file=${user_readme}
		cp $readme_file $onepack_image_path/
	fi
	if [ -d "$onepack_image_path/factory" ]; then
		cp $readme_file $onepack_image_path/factory
	fi
	#Release notes
	if [ ! -f "$user_release_note" ]; then
		release_note=example/${project_example}/ReleaseNotes.txt
	else
		release_note=${user_release_note}
	fi
	if [ -f "$user_release_note" ]; then
		cp ${release_note} $onepack_image_path/
		cp ${release_note} $onepack_image_path/factory
	fi
	#Firmware package
	cd $onepack_image_path
	# zip -r factory-${onepack_image_path_sub}.zip factory/
	# rm -rf factory/
	cd ../
	zip -r ${onepack_image_path_sub}.zip $onepack_image_path_sub/
}

#Set compiler path if exists
if [ -d "$cross_compile_path" ]; then
        export PATH=$PATH:$cross_compile_path
fi

# Frimware type
check_firmware_type $1

# Apply patchs
apply_patchs

# get firmware names
get_firmware_names

while :
do
	clear

	cat << EOF
		${product_vendor}_${product_name} firmware compile menu
*************************************************************************
@@@ Firmware type: ${fw_type}
@@@ Board name:    ${project_board}
@@@ Application:   ${project_example}
*************************************************************************

(1) gen mainland daily
(2) gen mainland pre
(3) gen mainland online
(4) gen overseas(singapore) pre
(5) gen overseas(singapore) online
(6) gen germany pre
(7) gen germany online
(a) gen all firmwares
(f) gen factory firmwares
(c) clean all
(t) toggle firmware type
(q) Quit
EOF

	#Read user input and process
	read -p "Please enter your selection: " input
	case $input in
	1) echo " Region: mainland, env: on daily"
	update_firmware_version
	build_firmware  "MAINLAND"  "ON_DAILY" "${compile_date}"
	exit;;
	2) echo " Region: mainland, env: on pre"
	update_firmware_version
	build_firmware  "MAINLAND"  "ON_PRE" "${compile_date}"
	exit;;
	3) echo " Region: mainland, env: online"
	update_firmware_version
	build_firmware  "MAINLAND"  "ONLINE" "${compile_date}"
	exit;;
	4) echo " Region: overseas(singapore), env: on pre"
	update_firmware_version
	build_firmware  "SINGAPORE"  "ON_PRE" "${compile_date}"
	exit;;
	5) echo " Region: overseas(singapore), env: online"
	update_firmware_version
	build_firmware  "SINGAPORE"  "ONLINE" "${compile_date}"
	exit;;
	6) echo " Region: germany, env: on pre"
	update_firmware_version
	build_firmware  "GERMANY"  "ON_PRE" "${compile_date}"
	exit;;
	7) echo " Region: germany, env: online"
	update_firmware_version
	build_firmware  "GERMANY"  "ONLINE" "${compile_date}"
	exit;;
	a) echo "Region: all, env: all for test"
	update_firmware_version
	build_firmware_all test_mode
	exit;;
	f) echo "Region: all, env: all for factory release"
	if [ "$fw_type" == "debug" ]; then
		toggle_firmware_type
	fi
	update_firmware_version
	build_firmware_all factory_mode
	exit;;
	c) echo "Clean last build (aos make clean)"
	build_clean
	sleep 0.5;;
	t) echo "Toggle firmware type"
	toggle_firmware_type;;
	q) echo "Exiting firmware build menu ..."
	sleep 0.5
	exit;;
	esac
done

