#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xtimer.h"
#include "timex.h"
#include "periph/adc.h"
#include "saul_reg.h"
#include "saul.h"
#include "phydat.h"

#include "net/gcoap.h"
#include "od.h"
#include "fmt.h"
#include "net/gnrc/netif.h"

#define RES             ADC_RES_10BIT
#define SOIL_HUM        2
#define IOP_ADDRESS "2001:470::242:ac11:4"
#define IOP_PORT "5683"

static void _resp_handler(unsigned req_state, coap_pkt_t* pdu)
{
    if (req_state == GCOAP_MEMO_TIMEOUT) {
        printf("gcoap: timeout for msg ID %02u\n", coap_get_id(pdu));
        return;
    }
    else if (req_state == GCOAP_MEMO_ERR) {
        printf("gcoap: error in response\n");
        return;
    }

    char *class_str = (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS)
                            ? "Success" : "Error";
    printf("gcoap: response %s, code %1u.%02u", class_str,
                                                coap_get_code_class(pdu),
                                                coap_get_code_detail(pdu));
    if (pdu->payload_len) {
        if (pdu->content_type == COAP_FORMAT_TEXT
                || pdu->content_type == COAP_FORMAT_LINK
                || coap_get_code_class(pdu) == COAP_CLASS_CLIENT_FAILURE
                || coap_get_code_class(pdu) == COAP_CLASS_SERVER_FAILURE) {
            /* Expecting diagnostic payload in failure cases */
            printf(", %u bytes\n%.*s\n", pdu->payload_len, pdu->payload_len,
                                                          (char *)pdu->payload);
        }
        else {
            printf(", %u bytes\n", pdu->payload_len);
        }
    }
    else {
        printf(", empty payload\n");
    }
}

static size_t _send(uint8_t *buf, size_t len, char *addr_str, char *port_str)
{
    ipv6_addr_t addr;
    size_t bytes_sent;
    sock_udp_ep_t remote;

    remote.family = AF_INET6;
    remote.netif  = SOCK_ADDR_ANY_NETIF;

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        puts("gcoap_cli: unable to parse destination address");
        return 0;
    }
    memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

    /* parse port */
    remote.port = atoi(port_str);
    if (remote.port == 0) {
        puts("gcoap_cli: unable to parse destination port");
        return 0;
    }

    bytes_sent = gcoap_req_send2(buf, len, &remote, _resp_handler);

    return bytes_sent;
}

int sample_sensor(int type, int *measure){
    saul_reg_t *sensor = saul_reg_find_type(type);
    phydat_t res;
    int ret = saul_reg_read(sensor, &res);

    if(type == SAUL_SENSE_COLOR){
        int total = 0;
        for(int i = 0; i < 3; i++){
            total += res.val[i];
        }
        total = total / 3;
        *measure = total;
        return 0;

    } else if(ret == 1) {
        *measure = res.val[0];
        return 0;

    } else {
        return ret;
    }
}

int sample_adc(int line, int *measure){
    int sample = adc_sample(ADC_LINE(line), RES);
    if(sample >= 0){
         *measure = sample;
         return 0;
     }
     return -1;
}

#define MAX_ADDR_LEN (8U)
int main(void)
{
    adc_init(ADC_LINE(2));
    adc_init(ADC_LINE(3));
    int val;
    //network_uint32_t temp_data, air_hum_data, light_data, soil_hum_data;
    char data_buf[12];
    //char *entry_points[] = {"/temp", "/air-hum", "/light", "/soil-hum"};
    int sensors[] = {SAUL_SENSE_TEMP, SAUL_SENSE_HUM, SAUL_SENSE_COLOR, SOIL_HUM};
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;
    size_t len;
	kernel_pid_t ifs[GNRC_NETIF_NUMOF];
	gnrc_netif_get(ifs);

	uint8_t hwaddr[MAX_ADDR_LEN];
	gnrc_netapi_get(ifs[0], NETOPT_ADDRESS, 0, hwaddr, sizeof(hwaddr));

	memcpy(data_buf, &hwaddr[4], 4);

	network_uint16_t current_data;

    while(1){
        for(int i = 0; i < 4; i++){
            if(i == 3){
                sample_adc(sensors[i], &val);
            } else {
                sample_sensor(sensors[i], &val);
            }
            printf("%d\n", val);
			current_data = byteorder_htons(val);
			memcpy(data_buf+4+2*i, &current_data, sizeof(network_uint16_t)); 
        }

        gcoap_req_init(&pdu, &buf[0], GCOAP_PDU_BUF_SIZE, 2, "/data");
        memcpy(pdu.payload, (char *)data_buf, sizeof(data_buf));
        len = gcoap_finish(&pdu, sizeof(data_buf), COAP_FORMAT_TEXT);
        _send(&buf[0], len, IOP_ADDRESS, IOP_PORT);
		printf("Data: ");
		for(int i=0;i<12;i++)
		{
			printf("%02x ", data_buf[i]);
		}
		puts(" ");
        xtimer_sleep(2);
    }
    return 0;
}

