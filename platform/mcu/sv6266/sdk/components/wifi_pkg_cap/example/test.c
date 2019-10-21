#include "smart_config.h"

void test_sniffer_cb(packetinfo *pkt)
{
	struct ieee80211_qos_hdr *hdr = (struct ieee80211_qos_hdr *)pkt->data;
	u16 fc = hdr->frame_control;
	printf("%s len %d\n", ieee80211_is_beacon(fc) ? "beacon" : ieee80211_is_data(fc) ? "data" : "UNKOWN", pkt->len);
}

void test_init(void)
{
	printf("%s\n", __func__);
	attach_sniffer_cb(RECV_DATA_BCN, test_sniffer_cb, 256);
	start_sniffer_mode();
	//set_channel(5);
	auto_ch_switch_start(150);
}

void test_stop(void)
{
	printf("%s\n", __func__);
	auto_ch_switch_stop();
	deattach_sniffer_cb();
	stop_sniffer_mode();
}
