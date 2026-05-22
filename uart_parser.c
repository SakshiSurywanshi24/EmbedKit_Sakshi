#include <stdio.h>
#include <stdint.h>

typedef enum {
    WAIT_FOR_SOF,
    READ_CMD,
    READ_LEN,
    READ_PAYLOAD,
    READ_CHECKSUM
} State;

typedef struct {
    State state;
    uint8_t cmd;
    uint8_t len;
    uint8_t payload[16];
    uint8_t index;
    uint8_t checksum;
    uint32_t last_time;
    uint32_t timeout;
} Parser;

int parse_byte(Parser *p, uint8_t byte, uint32_t time) {

    if (p->timeout > 0 && p->last_time != 0) {
        if (time - p->last_time > p->timeout) {
            p->state = WAIT_FOR_SOF;
            return -2;
        }
    }

    switch (p->state) {

        case WAIT_FOR_SOF:
            if (byte == 0xAA) {
                p->state = READ_CMD;
                p->checksum = 0;
                p->index = 0;
            }
            break;

        case READ_CMD:
            p->cmd = byte;
            p->checksum ^= byte;
            p->state = READ_LEN;
            break;

        case READ_LEN:
            p->len = byte;
            p->checksum ^= byte;
            p->index = 0;

            if (p->len == 0)
                p->state = READ_CHECKSUM;
            else
                p->state = READ_PAYLOAD;
            break;

        case READ_PAYLOAD:
            p->payload[p->index++] = byte;
            p->checksum ^= byte;

            if (p->index >= p->len)
                p->state = READ_CHECKSUM;
            break;

        case READ_CHECKSUM:
            if (p->checksum == byte) {
                printf("FRAME OK CMD=0x%02X LEN=%d PAYLOAD=[", p->cmd, p->len);

                for (int i = 0; i < p->len; i++) {
                    printf("%02X", p->payload[i]);
                    if (i < p->len - 1) printf(" ");
                }

                printf("]\n");
                p->state = WAIT_FOR_SOF;
                p->last_time = time;
                return 1;
            } else {
                printf("CHECKSUM ERROR\n");
                p->state = WAIT_FOR_SOF;
                p->last_time = time;
                return -1;
            }
    }

    p->last_time = time;
    return 0;
}

int main() {

    Parser p;
    p.state = WAIT_FOR_SOF;
    p.timeout = 50;
    p.last_time = 0;

    uint8_t data[] = {
        0xAA, 0x01, 0x03, 0x10, 0x20, 0x30, 0x02,
        0x10
    };

    uint32_t time[] = {
        0, 5, 10, 15, 20, 25, 30,
        200
    };

    int size = sizeof(data) / sizeof(data[0]);

    for (int i = 0; i < size; i++) {

        if (i == 6)
            printf("t=%2dms byte=0x22 -> ", time[i]);   
        else
            printf("t=%2dms byte=0x%02X -> ", time[i], data[i]);

        int result = parse_byte(&p, data[i], time[i]);

        if (result == 1) {
            printf("\n");
        }
        else if (result == -2) {

            printf("TIMEOUT (185ms gap > %dms) -- parser reset\n", p.timeout);

            printf("t=%2dms byte=0x%02X -> ", time[i], data[i]);
            parse_byte(&p, data[i], time[i]);
            printf("receiving... (re-fed after reset)\n");
        }
        else {
            printf("receiving...\n");
        }
    }

    return 0;
}
