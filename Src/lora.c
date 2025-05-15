#include "main.h"
#include "uart.h"
#include <string.h>
#include "interruption.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>



void send(const char *cmd) {
    while (*cmd) {
        UART1_Write(*cmd++);
    }
    UART1_Write('\r');
    UART1_Write('\n');
}


void send_AT_command(void) {
SYSTICK_Delay(100);

send("AT");

SYSTICK_Delay(2000);

send("AT+JOIN");

SYSTICK_Delay(10000);



}

//AT+CMSG="0123"

void send_message(const char *message) {
    char command[100];
    sprintf(command, "AT+MSG=\"%s\"\r\n", message);
    send(command);
    SYSTICK_Delay(5000);

}

void lora_sleep(void) {
    send("AT+LOWPOWER=AUTOON");  // Mode auto wake par séquence spéciale
}

void lora_wake(void) {
    // Envoi 4 octets 0xFF (wake-up pattern)
    for (int i = 0; i < 4; i++) {
        UART1_Write(0xFF);
    }

    // Petit délai pour laisser le module détecter la séquence
    SYSTICK_Delay(10);

    // Envoi de la commande qui désactive le mode AUTOON
    send("AT+LOWPOWER=AUTOOFF");
}
