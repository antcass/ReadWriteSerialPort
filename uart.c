#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main()
{
    //con open() apro la porta UART, restituisce un file descriptor
    int uart0_file = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart0_file == -1)
    {
        perror("error: open UART port");
        return 1;
    }

    //termios= struttura dati per configurare i parametri per la comunicazione
    struct termios options;
    //prendo i parametri del terminale a cui faccio riferimento e inserisco tutto nella struttura dati termios
    int res=0;
    res=tcgetattr(uart0_file, &options);
    
    if (res < 0) { 
    	perror("failed to get attr"); 
    	return 1;
    }
    //cflag=control local modes
    //CREAD=enable receiver, CLOCAL=Ignore modem control lines, CS8 set 8 data bits
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    //IGNPAR Ignore framing errors and parity errors
    options.c_iflag = IGNPAR;
    //no options flag in output
    options.c_oflag = 0;
    //no options flag in local
    options.c_lflag = 0;
    tcflush(uart0_file, TCIFLUSH);
    tcsetattr(uart0_file, TCSANOW, &options);

    char tx_buffer[20] = "Hello!\n";

    //scrivo il messaggio sulla porta UART con write()
    if (write(uart0_file, tx_buffer, sizeof(tx_buffer)) < 0)
    {
        perror("error: write UART port");
        return 1;
    }

    char rx_buffer[256];

    //leggo il messaggio sulla porta UART con read()
    int rx_length = read(uart0_file, (void*)rx_buffer, sizeof(rx_buffer));
    if (rx_length < 0)
    {
        perror("error: read UART port");
        return 1;
    }
    else if (rx_length == 0)
    {
        printf("No data available on the UART port.\n");
    }
    else
    {
        rx_buffer[rx_length] = '\0';
        printf("Data: %s", rx_buffer);
    }

    close(uart0_file);

    return 0;
}