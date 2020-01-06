/**
 * Copyright (c) 2016 iCOMM-semi. All Rights Reserved.
 *
 * NO WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#include "types.h"
#include "string.h" // for memory copy
 
#include "icmd_api.h"
#include "icmd2load.h"
#include "ble_transport_api.h"
#include "ble_transport_api_external.h"

#include "terminal.h"
#include "ble_bsp.h"
#include "ble_pub.h"
#include "breeze_hal_ble.h"

/**
 * Load configurations of BLE Register Service, including:
 *  -Device Configuration
 *  -GATT Database
 *  -Advertising Data, Parameter, and Enable
 *
 */
connected_callback_t sv_ble_connected = NULL;
disconnected_callback_t sv_disconnected = NULL;
on_char_write_t wc_handler = NULL;
on_char_write_t wwnrc_handler = NULL;
on_char_ccc_change_t ble_ic_ccc_change = NULL;
on_char_ccc_change_t ble_nc_ccc_change = NULL;

//on_char_read_t ;
uint8_t  ble_mac_addr[6];

uint8_t  icmd_buf[80];
uint8_t  uuid[16];

DECLARE_CONST uint8_t icmd_gap_add_device_name_service[] = {
/* iCMD add database: ... */
/* database 0x00: service: GAP */
/* CMD */ 0x0a, 0x15, 0x06, 0x00, 0x00, 0x01, 0x02, 0x00, 0x18,

};

DECLARE_CONST uint8_t icmd_gap_add_device_name[] = {
	/* database 0x01: characteristic: Device Name */ 
	/* CMD: GATTS_ADD_CHARACTERISTIC_CMD */
	0x0a, 0x17, 0x1d,
    0x01,       /* index */
    0x01, 0x00, /* permit */
    0x02,       /* property */
    0x00,       /* is_variable_value_len */
    0x02,       /* uuid_len */
    0x00, 0x2a, /* UUID_GAP_CHARACTERISTIC_DEVICE_NAME */
    0x14,       /* value_len */
    0x69, 0x43, 0x6f, 0x6d, 0x6d, 0x20, 0x42, 0x4c, /* value: "iComm BLE Transport" */
    0x45, 0x20, 0x54, 0x72, 0x61, 0x6e, 0x73, 0x70,
    0x6f, 0x72, 0x74, 0x00,
};

DECLARE_CONST uint8_t icmd_gap_add_service[] = {
//GATTS_ADD_SERVICE_CMD
/* CMD */ 0x0a, 0x15, 0x14, 0x00, 0x02, 0x0b, 0x10, 0x84, 0x20, 0x51, 0x49, 0x03, 0x98, 0x3a, 0x48, 0xe9, 0x6d, 0x46, 0x81, 0x01, 0x00, 0x01, 0x00,
};

DECLARE_CONST uint8_t icmd_gap_add_characteristic[] = {
/* database characteristic */
/* CMD */ 0x0a, 0x17, 0x1d, 0x03, 0x03, 0x00, 0x3e, 0x00, 0x10, 0x84, 0x20, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};

DECLARE_CONST uint8_t icmd_gap_add_characteristic_descriptor[] = {
	/* database 0x04: characteristic descriptor */
	/* CMD */ 0x0a, 0x18, 0x0b, 0x03, 0x04, 0x03, 0x00, 0x00, 0x02, 0x02, 0x29, 0x02, 0x00, 0x00,
};

DECLARE_CONST uint8_t ble_adv_data[] = {
/* CMD: GAP_SET_ADV_DATA_CMD */ 0x0a, 0x03, 0x34,
    0x00, /* data_type */
    0x32, /* length */
    /* data */
    0x02, /* AD Data length */
    0x01, /* AD Data type: Flag */
    0x06, /* AD Data: GAP_FLAGS_LE_ONLY_GENERAL_DISCOVERABLE_MODE */
    0x14, /* AD Data length */
    0x09, /* AD Data type: GAP_AD_TYPE_COMPLETE_LOCAL_NAME */
    0x69, 0x43, 0x6f, 0x6d, 0x6d, 0x20, 0x42, 0x4c, /* AD Data: iComm BLE Transport*/
    0x45, 0x20, 0x54, 0x72, 0x61, 0x6e, 0x73, 0x70,
    0x6f, 0x72, 0x74,
    0x19, /* AD Data length *///25  //12开始
    0xff, /* AD Data type: 自定厂商数据 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* AD Data: 厂商数据*/
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void convert_adv_data(ais_adv_init_t *adv_data)
{
	uint8_t len;
	uint8_t i,n;

	
	memcpy(icmd_buf, ble_adv_data, sizeof(ble_adv_data));
	icmd_buf[7] = adv_data->flag;
	if(adv_data->name.ntype == AIS_ADV_NAME_SHORT)
		icmd_buf[9] = 0x08;
	else
		icmd_buf[9] = 0x09;

	len = strlen(adv_data->name.name);
	icmd_buf[2] = len + 7 + adv_data->vdata.len + 2;
	icmd_buf[4] = len + 5 + adv_data->vdata.len + 2;
	icmd_buf[8] = len + 1;//名字总字节长度
	memcpy(icmd_buf + 10, adv_data->name.name, len);
	icmd_buf[8 + len + 2] = adv_data->vdata.len + 1;//厂商字节长度
	icmd_buf[8 + len + 3] = 0xff;//厂商类型
	memcpy(icmd_buf + 8 + len + 4 , adv_data->vdata.data, adv_data->vdata.len);
	icmd2load(icmd_buf,icmd_buf[2]+3);
}

uint8_t get_uuid_len(uint8_t len)
{
	if(len == AIS_UUID_TYPE_16)
		return 0x02;
	else if(len == AIS_UUID_TYPE_32)
		return 0x04;
	else if(len == AIS_UUID_TYPE_128)
		return 0x10;
	return 0xff;
}
	
void convert_uuid(uint8_t *buf, uint8_t *data, uint8_t uuid_len)
{
	uint8_t n;
	uint8_t i = 0;
	for(n = 0; n < uuid_len; n++)
	{
		buf[n] = data[n];
		i++;
	}
}
uint8_t ble_transport_server(uint8_t *data, uint8_t uuid_len)
{
	uint8_t i,ret;
	memcpy(icmd_buf, icmd_gap_add_service, sizeof(icmd_gap_add_service));
	memset(icmd_buf + 6, 0x00, 17);
	icmd_buf[6] = get_uuid_len(uuid_len);
	icmd_buf[2]-= (16 - icmd_buf[6]);
	convert_uuid(uuid, data, icmd_buf[6]);
	memcpy(icmd_buf + 7,uuid,icmd_buf[6]);
	ret = icmd2load(icmd_buf,(icmd_buf[2] + 3));
	return ret;
}


uint8_t ble_transport_device_name(uint8_t *name)
{
	uint8_t i,ret;
	uint8_t len;
	uint8_t  ble_name[20];
	memcpy(icmd_buf, icmd_gap_add_device_name_service, sizeof(icmd_gap_add_device_name_service));
	ret = icmd2load(icmd_buf,sizeof(icmd_gap_add_device_name_service));
	memset(ble_name,0,20);
   	memcpy(ble_name, name,strlen(name));
	len = strlen(ble_name);
	memcpy(icmd_buf, icmd_gap_add_device_name, sizeof(icmd_gap_add_device_name));
	memset(icmd_buf + 11, 0x00, 21);
	memcpy(icmd_buf + 12, ble_name, len);
	icmd_buf[11] = len;
	icmd_buf[2] -= 20 - len;
	ret = icmd2load(icmd_buf,(sizeof(icmd_gap_add_device_name) - (20 - len)));
	return ret;
}


uint8_t ble_transport_characteristic(ais_char_init_t *Characteristics, uint8_t index)
{
	ais_uuid_16_t *p;
	uint8_t i,ret;
	p = &Characteristics->uuid->type;
	memcpy(icmd_buf, icmd_gap_add_characteristic, sizeof(icmd_gap_add_characteristic));
	memset(icmd_buf + 8, 0x00, 3);
	icmd_buf[3] = index;
	icmd_buf[6] = Characteristics->prop ;//& 0x07
	icmd_buf[4] = Characteristics->perm;
	icmd_buf[8] = get_uuid_len(p->uuid.type);
	convert_uuid(uuid, &p->val, icmd_buf[8]);
	memcpy(icmd_buf + 9,uuid,icmd_buf[8]);
	icmd2load(icmd_buf,(icmd_buf[2] + 3));
	
	memcpy(icmd_buf, icmd_gap_add_characteristic_descriptor, sizeof(icmd_gap_add_characteristic_descriptor));
	icmd_buf[3] = index;
	icmd_buf[4] = index + 1;
	ret = icmd2load(icmd_buf,sizeof(icmd_gap_add_characteristic_descriptor));
	return ret;
}


uint8_t al_stack_init(ais_bt_init_t *ais_init)
{
	ais_uuid_16_t *p;
	p = &ais_init->uuid_svc->type;
	uint8_t i;
	ble_transport_server(&p->val, p->uuid.type);
	
	ble_transport_characteristic(&ais_init->rc, 3);//READ
	
	
	ble_transport_characteristic(&ais_init->wc, 5);//WRITE
	wc_handler = ais_init->wc.on_write;
	
	ble_transport_characteristic(&ais_init->ic, 7);//INDICATE
	ble_ic_ccc_change = ais_init->ic.on_ccc_change;
	
	ble_transport_characteristic(&ais_init->wwnrc, 9);//WRITE_WITHOUT_RESP
	wwnrc_handler = ais_init->wwnrc.on_write;
	
	ble_transport_characteristic(&ais_init->nc, 11);//NOTIFY
	ble_nc_ccc_change = ais_init->nc.on_ccc_change;
	
	sv_ble_connected = ais_init->on_connected;
	sv_disconnected = ais_init->on_disconnected;
	ble_get_mac(ble_mac_addr);
	return 0;
}
 
DECLARE_CONST uint8_t icmd_tbl_ble_transport_init[] = {
/* iCMD device config: */
/* CMD */ 0x0a, 0x01, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
/* mask GAP_ADV_REPORT: */
/* CMD */ 0x0a, 0x24, 0x04, 0x04, 0x00, 0x00, 0x00,
/* rf indication */
/* CMD */ 0x0a, 0x3b, 0x03, 0x01, 0x01, 0x05,
/* adv to connect... */
/* CMD: GAP_SET_ADV_PARAM_CMD */ 0x0a, 0x02, 0x0e,
    0x50, 0x00, /* adv_interval */
    0x00,       /* adv_type */
    0x00,       /* own_addr_type */
    0x00,       /* peer_addr_type */
    /* peer addr (if adv_type == ADV_DIRECT_IND) */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07,       /* adv_channel_map */
    0x00,       /* adv_filter */
    0x00,       /* reserved */
};

/**
 * XTAL CLOAD tunning
 *  - Only necessary if XTAL not fit iComm XTAL SPEC.
 *
 */
DECLARE_CONST uint8_t cload_tunning_cmd[] = {
    0x0a, 0x2f, 0x0c, 0x3c, 0x40, 0xc0, 0x00, 0xff, 0xff, 0x00, 0x00, 0x2c, 0x2c, 0x00, 0x00
};
void ble_transport_init() {

    LOAD_TBL(icmd_tbl_ble_transport_init);
    return;
}

/**
 * about connect
 *
 */
DECLARE_CONST uint8_t icmd_tbl_ble_transport_adv2connect[] = {
/* CMD */ 0x0a, 0x02, 0x0e, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00,
/* CMD */ 0x0a, 0x04, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
};
void ble_transport_adv2connect_on_disconnect() {

    LOAD_TBL(icmd_tbl_ble_transport_adv2connect);
    return;
}

DECLARE_CONST uint8_t icmd_gap_disconnect_cmd[] = {
    /* GAP_DISCONNECT_CMD */
    0x0a, 0x0f, 0x01, 0x16
};

void ble_gap_disconnect_cmd(void)
{
    LOAD_TBL(icmd_gap_disconnect_cmd);    
}


DECLARE_CONST uint8_t icmd_tbl_ble_test_transmitte[] = {
	0x0a, 0x2c, 0x05,
	0x00,		/* tx_frequency */
	0x00, 0x00, /* num_of_packets */
	0x25,		/* length */
	0x03		/* payload */
};




void ble_icmd_test_transmitter(uint8_t tx_frequency, uint16_t num_of_packets, uint8_t length, uint8_t payload) {
	memcpy(icmd_buf, icmd_tbl_ble_test_transmitte, sizeof(icmd_tbl_ble_test_transmitte));
	icmd_buf[3] = tx_frequency;
	icmd_buf[4] = num_of_packets;
	icmd_buf[5] = num_of_packets >> 8;
	icmd_buf[6] = length;
	icmd_buf[7] = payload;
    icmd2load(icmd_buf, sizeof(icmd_tbl_ble_test_transmitte));
    return;
}

  


DECLARE_CONST  uint8_t icmd_tbl_ble_test_receiver[4] = {
    0x0a, 0x2b, 0x01,
    0x00        /* rx_frequency */
};


void ble_icmd_test_receiver(uint8_t rx_frequency) {
	memcpy(icmd_buf, icmd_tbl_ble_test_receiver, sizeof(icmd_tbl_ble_test_receiver));
	icmd_buf[3] = rx_frequency;
	
    icmd2load(icmd_buf, sizeof(icmd_tbl_ble_test_receiver));
	return;
}

DECLARE_CONST uint8_t cmd_gap_adv_enable[] = {
    /* CMD: GAP_SET_ADV_ENABLE_CMD */ 
    0x0a, 0x04, 0x06,
    0x01,                   /* enable */
    0x00, 0x00, 0x00, 0x00, /* timeout */
    0x00                   /* reserved */
};

DECLARE_CONST uint8_t cmd_gap_adv_disable[] = {
    /* CMD: GAP_SET_ADV_ENABLE_CMD */ 
    0x0a, 0x04, 0x06,
    0x00,                   /* disable */
    0x00, 0x00, 0x00, 0x00, /* timeout */
    0x00                   /* reserved */
};

void ble_icmd_gap_adv(int enable)
{
    printf("\nGAP_SET_ADV_ENABLE_CMD: %d\n", enable);
    if (enable) {
        LOAD_TBL(cmd_gap_adv_enable);
    }
    else {
        LOAD_TBL(cmd_gap_adv_disable);
    }
}

DECLARE_CONST uint8_t icmd_tbl_ble_get_public_addr[] = {
    0x0a, 0x0c, 0x00
};

void ble_icmd_get_mac_addr(uint8_t *mac)
{
    
    uint8_t loop_i;
    
    icmd2load(icmd_tbl_ble_get_public_addr, sizeof(icmd_tbl_ble_get_public_addr));
    for(loop_i = 0; loop_i < 6; loop_i++)
    {
		*mac = g_status.param[STATUS_PARAM_RET_PARAM_INDEX + (2 -loop_i)];
		mac++;
    }
}


DECLARE_CONST uint8_t test_end_cmd[] = {
    0x0a, 0x2d, 0x00
};




void ble_icmd_test_end(void) {

    LOAD_TBL(test_end_cmd);
    //TERMINAL_STR("\n NUM_OF_PACKETS:"); TERMINAL_ARRAY(g_status.param +STATUS_PARAM_RET_PARAM_INDEX, 2);
    return;
}


/**
 * BLE Transport:
 *
 */
#define GATTS_SERVER_INDICATION_CMD_INDEX_INDEX         (4)
#define GATTS_SERVER_INDICATION_CMD_VALUE_SIZE_INDEX    (5)
#define GATTS_SERVER_INDICATION_CMD_VALUE_INDEX         (6)
DECLARE_CONST uint8_t gatts_server_notification_cmd[] = {
    0x0a,           /* INDICATE */
    0x1b,           /* OP */
    0x17,           /* PARAM LEN */
    0x00,           /* INDICATE_TYPE: Notify */
    0x00,           /* INDEX */
    0x14,           /* VALUE_SIZE */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* VALUE */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

DECLARE_CONST uint8_t gatts_server_indication_cmd[] = {
    0x0a,           /* INDICATE */
    0x1b,           /* OP */
    0x17,           /* PARAM LEN */
    0x01,           /* INDICATE_TYPE:  indication  */
    0x00,           /* INDEX */
    0x14,           /* VALUE_SIZE */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* VALUE */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

// command buffer: a temperally place to compose the cmd
uint8_t XDATA _cmd_buf[sizeof(gatts_server_notification_cmd)];

uint8_t* DECLARE_CONST cmd_buf = _cmd_buf;
static int8_t do_gatts_server_notification(uint8_t index, uint8_t value_len, uint8_t* value) {

    memcpy(cmd_buf, gatts_server_notification_cmd, sizeof(gatts_server_notification_cmd));
    cmd_buf[GATTS_SERVER_INDICATION_CMD_INDEX_INDEX] = index;
    
    //TERMINAL_STR("\n do server indication on index"); TERMINAL_HEX(index);
    //TERMINAL_ARRAY(value, value_len);

    // compose value
    if(value_len > 20) {
        return -1;
    }
    memcpy(cmd_buf +GATTS_SERVER_INDICATION_CMD_VALUE_INDEX, value, value_len);

    // compose length
    cmd_buf[GATTS_SERVER_INDICATION_CMD_VALUE_SIZE_INDEX] = value_len;
    cmd_buf[ICMD_PARAM_LEN_INDEX] = value_len + 
        // number of parameters, exclude value
        (GATTS_SERVER_INDICATION_CMD_VALUE_INDEX -1 -ICMD_PARAM_LEN_INDEX);

    return icmd2load(cmd_buf, GATTS_SERVER_INDICATION_CMD_VALUE_INDEX +value_len);
}

static int8_t do_gatts_server_indication(uint8_t index, uint8_t value_len, uint8_t* value) {

    memcpy(cmd_buf, gatts_server_indication_cmd, sizeof(gatts_server_indication_cmd));
    cmd_buf[GATTS_SERVER_INDICATION_CMD_INDEX_INDEX] = index;
    
    //TERMINAL_STR("\n do server indication on index"); TERMINAL_HEX(index);
    //TERMINAL_ARRAY(value, value_len);

    // compose value
    if(value_len > 20) {
        return -1;
    }
    memcpy(cmd_buf +GATTS_SERVER_INDICATION_CMD_VALUE_INDEX, value, value_len);

    // compose length
    cmd_buf[GATTS_SERVER_INDICATION_CMD_VALUE_SIZE_INDEX] = value_len;
    cmd_buf[ICMD_PARAM_LEN_INDEX] = value_len + 
        // number of parameters, exclude value
        (GATTS_SERVER_INDICATION_CMD_VALUE_INDEX -1 -ICMD_PARAM_LEN_INDEX);

    return icmd2load(cmd_buf, GATTS_SERVER_INDICATION_CMD_VALUE_INDEX +value_len);
}

void get_ble_mac_addr(uint8_t *addr)
{

	uint8_t loop_i;
	// get public address in status.
    icmd2load(icmd_tbl_ble_get_public_addr, sizeof(icmd_tbl_ble_get_public_addr));
    // add address in advertising data
    for(loop_i = 6; loop_i > 0; loop_i--) {
        *(addr + (6 - loop_i)) = g_status.param[STATUS_PARAM_RET_PARAM_INDEX + loop_i - 1];

    }    
    
}

int8_t ble_string_notification_send(uint8_t index, uint8_t value_len, uint8_t* value) {
    
    // DO NOTIFY
    return do_gatts_server_notification(index, value_len, value);
}

int8_t ble_string_indication_send(uint8_t index, uint8_t value_len, uint8_t* value) {
    
    // DO 
    return do_gatts_server_indication(index, value_len, value);
}




void ble_transport_string_received_on_write_sync(ievt_param_gatts_write_sync_st* ievt_param_gatts_write_sync) {

    uint8_t id;
    
    switch(ievt_param_gatts_write_sync->index) {
        case BLE_UUID_AIS_WC_INDEX:
        	wc_handler(ievt_param_gatts_write_sync->value, ievt_param_gatts_write_sync->value_size);
        	break;
        	
        case BLE_UUID_AIS_IC_INDEX:
        	printf("BLE_UUID_AIS_IC_INDEX %s \n", ievt_param_gatts_write_sync->value);
        	break;
        	
        case BLE_UUID_AIS_WWNRC_INDEX:
        	wwnrc_handler(ievt_param_gatts_write_sync->value, ievt_param_gatts_write_sync->value_size);

        	break;	
        	
        case BLE_UUID_AIS_NC_INDEX:

            //id = (ievt_param_gatts_write_sync->index -BLE_STRING00_INDEX) >> 1;
            break;
        case BLE_UUID_AIS_IC_DES_INDEX:
        	ble_ic_ccc_change(*ievt_param_gatts_write_sync->value);
        	
        	break;    
        case BLE_UUID_AIS_NC_DES_INDEX:
       		ble_nc_ccc_change(*ievt_param_gatts_write_sync->value);
        
        	break;
        default:
            return;
    }

    // application
    return;
}
