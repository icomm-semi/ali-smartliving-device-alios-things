#!/bin/sh
#gen_firmware.sh

#Firmware image path
project_example=smart_outlet
project_board=hf-lpt230

product_vendor=hf-lpt230
product_name=smart_outlet
user_release_note=your_path_to_your_release_note
user_readme=your_path_to_your_readme_file
onepack_image_path_root=${product_vendor}_${product_name}_onepack_image
version_config_file=framework/protocol/linkkit/sdk/iotx-sdk-c_clone/aos_board_conf.mk
project_out_path=./out/${project_example}@${project_board}/binary
fw_out_filename=fw-${product_vendor}_${product_name}
bootloader_version="v1.0"
bootloader_org_file=./tools/pack/bootloader_lzma
bootloader_filename=bootloader-${product_vendor}_${product_name}-${bootloader_version}
version_config_string="SYSINFO_APP_VERSION"

#cross complier path
cross_compile_path=${HOME}/.bin/gcc-arm-none-eabi/Linux64/bin
aos_cube_file=${HOME}/.bin/aos
aos_cube=aos

#set -x

#Firmware type
DEBUG=0

function apply_patchs()
{
	# No SSL workaround default
	git checkout security/imbedtls/include/mbedtls/ssl.h
	# Apply patchs
	# git am example/lv/${project_example}/patchs/*
	#patch -p1 < example/lv/${project_example}/patchs/*
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

function get_firmware_names()
{
	fw_out_org_filename=${project_example}@${project_board}
	fw_out_org_file=${project_out_path}/${fw_out_org_filename}
	if [ $project_board == "hf-lpt230" ]; then
		fw_out_combine_filename=${fw_out_org_filename}_all
        elif [ $project_board == "uno-91h" ]; then
                fw_out_combine_filename=${fw_out_org_filename}_all
	elif [ $project_board == "mk3080" ]; then
		fw_out_combine_filename=${fw_out_org_filename}.all
        elif [ $project_board == "mk5080" ]; then
                fw_out_combine_filename=${fw_out_org_filename}.all
	else
		fw_out_combine_filename=${fw_out_org_filename}
	fi
	fw_out_combine_file=${project_out_path}/${fw_out_combine_filename}
	fw_upgrade_filename=ota-${product_vendor}_${product_name}
	if [ $project_board == "hf-lpt230" ]; then
		fw_upgrade_org_filename=${fw_out_org_filename}_ota
        elif [ $project_board == "uno-91h" ]; then
                fw_upgrade_org_filename=${fw_out_org_filename}_ota
        elif [ $project_board == "mk3080" ]; then
                fw_upgrade_org_filename=ota_all
	elif [ $project_board == "mk5080" ]; then
		fw_upgrade_org_filename=ota_all
	else
		fw_upgrade_org_filename=${fw_out_org_filename}
	fi
	fw_upgrade_org_file=${project_out_path}/${fw_upgrade_org_filename}
}

function firmware_type()
{
	if [ "$1" == "debug" ]; then
		DEBUG=1
	else
		DEBUG=0
	fi
}

function auto_deploy_all()
{
	if [ "$1" == "all" ]; then
		echo "Region: all, env: all"
		build_firmware_all
		exit 0
	fi
}

function build_firmware()
{
	# Need workaround for SSL
	git checkout security/imbedtls/include/mbedtls/ssl.h
	if [ "$1-$2" == "GERMANY-ON_PRE" ]; then
		echo "Workaround for SSL..................."
		patch -p1 < example/${project_example}/workaround/germany_pre_ssl_workaround.patch
		if [ $? -ne 0 ]; then
			git checkout security/imbedtls/include/mbedtls/ssl.h
			echo "SSL workaround failed!!!"
			exit 1
		fi
	fi
	$aos_cube make clean
	rm -rf framework/protocol/linkkit/sdk/iotx-sdk-c_clone/.O
	$aos_cube make ${project_example}@${project_board} CONFIG_SERVER_REGION=$1 CONFIG_SERVER_ENV=$2 CONFIG_COMPILE_DATE=$3 CONFIG_DEBUG=${DEBUG}
	git checkout security/imbedtls/include/mbedtls/ssl.h
	if [ $? -ne 0 ]; then
		exit 1
	fi
	firmware_version=$(get_firmware_version)
	if [ $project_board == "hf-lpt230" ] || [ $project_board == "hf-lpb135" ]; then
		#create upgrade bin
		#./tools/pack/imgpkt e ${fw_out_org_file}.bin ${fw_out_org_file}_lzma.bin
		#xz --format=lzma -z -k -f -v ${fw_out_org_file}.bin
		#cp ${fw_out_org_file}.bin.lzma ${fw_out_org_file}_lzma.bin
		#python ./tools/pack/image_pack_ota_lzma.py ${fw_out_org_file}.bin 1.00.00
		#create image bin
		#python ./tools/pack/image_pack_image_lzma.py ${fw_out_org_file}.bin 1.00.00
		# Gen md5 magic
		#python ./build/scripts/ota_gen_md5_bin.py ${fw_out_org_file}_upgrade.bin
		echo 'hello test'
	fi
	#Copy and rename
	local server_region=`echo "$1" | tr '[:upper:]' '[:lower:]'`
	local server_env=`echo "$2" | tr '[:upper:]' '[:lower:]'`
	if [ ${DEBUG} -eq 1 ]; then
		fw_out_filename_full=${fw_out_filename}-${server_region}-${server_env}-${firmware_version}-$3-debug
		fw_ota_filename_full=${fw_upgrade_filename}-${server_region}-${server_env}-${firmware_version}-$3-debug
	else
		fw_out_filename_full=${fw_out_filename}-${server_region}-${server_env}-${firmware_version}-$3-release
		fw_ota_filename_full=${fw_upgrade_filename}-${server_region}-${server_env}-${firmware_version}-$3-release
	fi
	cp ${fw_out_combine_file}.bin ${project_out_path}/${fw_out_filename_full}.bin
	cp ${fw_out_org_file}.elf ${project_out_path}/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${project_out_path}/${fw_ota_filename_full}.bin
}

function get_firmware_version()
{
	local version_line=`grep "$version_config_string" ${version_config_file}`
	version_line=${version_line##*SYSINFO_APP_VERSION=}
	version_line=${version_line##* = }
	local version=${version_line%% *}
	echo $version | sed 's/.\(.*\)/\1/' | sed 's/\(.*\)./\1/'
}

function clean_linkkit_sdk_update()
{
#	rm -rf framework/protocol/linkkit/sdk/iotx-sdk-c_clone/
#	bash build/checkout_iotx_sdk.sh
	rm -rf framework/protocol/linkkit/sdk/iotx-sdk-c_clone/.O/
	rm -rf framework/protocol/linkkit/sdk/iotx-sdk-c_clone/.config
}

function gen_readme()
{
	echo "-------------------------Firmware info for ada project---------------------------" > $readme_file
	echo "Host: ${USER}" >> $readme_file
	BRANCH=`git rev-parse --abbrev-ref HEAD`
	COMMIT_HASH=`git rev-parse HEAD`
	SDK_BRANCH=`(cd framework/protocol/linkkit/sdk/iotx-sdk-c_clone/ && git rev-parse --abbrev-ref HEAD)`
	SDK_HASH=`(cd framework/protocol/linkkit/sdk/iotx-sdk-c_clone/ && git rev-parse HEAD)`
	echo "Branch: ${BRANCH}" >> $readme_file
	echo "Hash: ${COMMIT_HASH}" >> $readme_file
	AOS_VERSION_STRING=`grep "SYSINFO_KERNEL_VERSION" framework/protocol/linkkit/sdk/iotx-sdk-c_clone/aos_board_conf.mk`
	AOS_VERSION=${AOS_VERSION_STRING##*SYSINFO_KERNEL_VERSION=}
	AOS_VERSION=${AOS_VERSION%% *}
	AOS_VERSION=`echo $AOS_VERSION | sed 's/.\(.*\)/\1/' | sed 's/\(.*\)./\1/'`
	echo "Kernel: ${AOS_VERSION}" >> $readme_file
	SDK_VERSION_STRING=`grep "LINKKIT_VERSION" framework/protocol/linkkit/sdk/iotx-sdk-c_clone/include/iot_export.h`
	SDK_VERSION=${SDK_VERSION_STRING##*LINKKIT_VERSION     }
	SDK_VERSION=`echo $SDK_VERSION | sed 's/.\(.*\)/\1/' | sed 's/\(.*\)./\1/'`
	echo "LinkKit: ${SDK_VERSION}" >> $readme_file
	echo "APP: ${firmware_version}" >> $readme_file
	echo "" >> $readme_file
	# echo "Bootloader: ${bootloader_filename}.bin" >> $readme_file
	echo "APP firmware: ${fw_out_filename}-region-env-app-ver.bin" >> $readme_file
	echo "OTA firmware: ota/${fw_upgrade_filename}_region-env-app-ver.bin" >> $readme_file
	echo "factory firmware: fw-factory-${onepack_image_path_sub}.zip" >> $readme_file
	echo "" >> $readme_file
	if [ $project_board == "mk3060" ]; then
		echo "Flash and debug uart baud rate:   921600" >> $readme_file
		echo "" >> $readme_file
		echo "Flash device firmware:" >> $readme_file
		echo "1. Hold boot key then press reset key " >> $readme_file
		echo "2. When console prints'MOC108 BOOTLOADER MENU', input 'write' or 'write  0x13200' then press ENTER" >> $readme_file
		echo "3. Send firmware file by Ymodem protocol" >> $readme_file
		echo "" >> $readme_file
		echo "Write meta data:" >> $readme_file
		echo "linkkey <Product Key> <Device Name> <Device Secret> <Product Secret>" >> $readme_file
		echo "Note:" >> $readme_file
		echo "If meta data wirte failed, execute 'erase 0x11000 0x2000' command in bootloader and try again" >> $readme_file
	fi
	if [ $project_board == "uno-91h" ]; then
		# echo "Bootloader flash addr:            0x1000" >> $readme_file
		echo "Combine firmware flash addr:      0x1000" >> $readme_file
		# echo "APP firmware flash addr:          0x4000" >> $readme_file
		echo "Flash and debug uart baud rate:   921600" >> $readme_file
		echo "Firmware app uart baud rate:      115200 or 9600" >> $readme_file
		echo "" >> $readme_file
		echo "Flash device meta data:" >> $readme_file
		echo "1. Press key '+' for three times " >> $readme_file
		echo "2. When console prints'a', press key 'a', console will print '+OK'" >> $readme_file
		echo "3. Flash product secret: AT+PROSECRET=<product secret>" >> $readme_file
		echo "4. Flash device secret:  AT+PRODEVICE=<product key>,<device name>,<device secret>" >> $readme_file
	fi
}

function build_firmware_all()
{
	#onepack firmware path
	if [ ${DEBUG} -eq 1 ]; then
		onepack_image_path_sub=fw-${product_vendor}_${product_name}_${compile_date}_debug
	else
		onepack_image_path_sub=fw-${product_vendor}_${product_name}_${compile_date}_release
	fi
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
	mkdir $onepack_image_path/factory
	mkdir $onepack_image_path/factory/ota
	mkdir $onepack_image_path/debug

	# Clean update SDK
	clean_linkkit_sdk_update
	#build and copy firmware
	build_firmware  "MAINLAND"  "ON_DAILY" "${compile_date}"
	cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
	cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin

	build_firmware  "MAINLAND"  "ON_PRE" "${compile_date}"
	cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
	cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin

	build_firmware  "MAINLAND"  "ONLINE" "${compile_date}"
	cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
	cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin
	cp ${fw_out_combine_file}.bin ${onepack_image_path}/factory/${fw_out_filename_full}.bin
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/factory/ota/${fw_ota_filename_full}.bin

	build_firmware  "SINGAPORE"  "ON_PRE" "${compile_date}"
	cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
	cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin

	build_firmware  "SINGAPORE"  "ONLINE" "${compile_date}"
	cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
	cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin
	cp ${fw_out_combine_file}.bin ${onepack_image_path}/factory/${fw_out_filename_full}.bin
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/factory/ota/${fw_ota_filename_full}.bin

	build_firmware  "GERMANY"  "ON_PRE" "${compile_date}"
	cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
	cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin

	build_firmware  "GERMANY"  "ONLINE" "${compile_date}"
	cp ${fw_out_combine_file}.bin ${onepack_image_path}/${fw_out_filename_full}.bin
	cp ${fw_out_org_file}.elf ${onepack_image_path}/debug/${fw_out_filename_full}.elf
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/ota/${fw_ota_filename_full}.bin
	cp ${fw_out_combine_file}.bin ${onepack_image_path}/factory/${fw_out_filename_full}.bin
	cp ${fw_upgrade_org_file}.bin ${onepack_image_path}/factory/ota/${fw_ota_filename_full}.bin
	#Copy bootloader
	# cp ${bootloader_org_file}.bin $onepack_image_path/${bootloader_filename}.bin

	#MD5 checksum
	md5sum $onepack_image_path/* > $onepack_image_path/checksum.md5
	#md5sum $onepack_image_path/ota/* > $onepack_image_path/ota/checksum.md5
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
	cp $readme_file $onepack_image_path/factory
	#Release notes
	if [ ! -f "$user_release_note" ]; then
		release_note=example/${project_example}/ReleaseNotes.txt
	else
		release_note=${user_release_note}
	fi
	cp ${release_note} $onepack_image_path/
	cp ${release_note} $onepack_image_path/factory
	#Firmware package
	cd $onepack_image_path
	zip -r factory-${onepack_image_path_sub}.zip factory/
	rm -rf factory/
	cd ../
	zip -r ${onepack_image_path_sub}.zip $onepack_image_path_sub/
}
 
cat << EOF
********${project_example}@${project_board} firmware compile menu**********
(1) gen mainland daily
(2) gen mainland pre
(3) gen mainland online
(4) gen singapore pre
(5) gen singapore online
(6) gen germany pre
(7) gen geramny online
(a) gen all firmwares
(u) Update Linkkit SDK
(q) Quit
EOF

# Set compiler path if exists
if [ -d "$cross_compile_path" ]; then
	export PATH=$PATH:$cross_compile_path
fi

# Set aos cube
if [ -f "$aos_cube_file" ]; then
aos_cube=$aos_cube_file
fi

echo "aos cube command: $aos_cube"

# get compile date
compile_date=`date +"%Y%m%d%H%M%S"`
# get firmware version
#firmware_version=$(get_firmware_version)

# Apply patchs
# apply_patchs

# Get firmware names
get_firmware_names

# Frimware type
firmware_type $1

# auto deploy
auto_deploy_all $2

#Read user input and process
read -p "Please enter your selection: " input
case $input in
1) echo " Region: mainland, env: on daily"
build_firmware  "MAINLAND"  "ON_DAILY" "${compile_date}";;
2) echo " Region: mainland, env: on pre"
build_firmware  "MAINLAND"  "ON_PRE" "${compile_date}";;
3) echo " Region: mainland, env: online"
build_firmware  "MAINLAND"  "ONLINE" "${compile_date}";;
4) echo " Region: singapore, env: on pre"
build_firmware  "SINGAPORE"  "ON_PRE" "${compile_date}";;
5) echo " Region: singapore, env: online"
build_firmware  "SINGAPORE"  "ONLINE" "${compile_date}";;
6) echo " Region: germany, env: on pre"
build_firmware  "GERMANY"  "ON_PRE" "${compile_date}";;
7) echo " Region: germany, env: online"
build_firmware  "GERMANY"  "ONLINE" "${compile_date}";;
a) echo "Region: all, env: all"
build_firmware_all;;
u) echo "Update Linkkit SDK"
clean_linkkit_sdk_update;;
esac
