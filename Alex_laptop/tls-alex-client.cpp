
// Routines to create a TLS client
#include "make_tls_client.h"

// Network packet types
#include "netconstants.h"

// Packet types, error codes, etc.
#include "constants.h"
#include "tls_common_lib.h"

// add in libraries to check keypress
#include <unistd.h>
#include <termio.h>

// Tells us that the network is running.
static volatile int networkActive=0;

// variables to keep track of the mode
static int mode = 1; // 0 = original command mode, 1 = WASD mode

static volatile int actionInProgress = 0; // 0 = no action, 1 = action in progress

// constants for WASD mode
int speed = 70;
int distance = 5;
int turn_speed = 70;
int angle = 20;


void handleError(const char *buffer)
{
	switch(buffer[1])
	{
		case RESP_OK:
			printf("Command / Status OK\n");
			break;

		case RESP_BAD_PACKET:
			printf("BAD MAGIC NUMBER FROM ARDUINO\n");
			break;

		case RESP_BAD_CHECKSUM:
			printf("BAD CHECKSUM FROM ARDUINO\n");
			break;

		case RESP_BAD_COMMAND:
			printf("PI SENT BAD COMMAND TO ARDUINO\n");
			break;

		case RESP_BAD_RESPONSE:
			printf("PI GOT BAD RESPONSE FROM ARDUINO\n");
			break;

		default:
			printf("PI IS CONFUSED!\n");
	}
}

void handleStatus(const char *buffer)
{
	int32_t data[16];
	memcpy(data, &buffer[1], sizeof(data));

	printf("\n ------- ALEX STATUS REPORT ------- \n\n");
	printf("Left Forward Ticks:\t\t%d\n", data[0]);
	printf("Right Forward Ticks:\t\t%d\n", data[1]);
	printf("Left Reverse Ticks:\t\t%d\n", data[2]);
	printf("Right Reverse Ticks:\t\t%d\n", data[3]);
	printf("Left Forward Ticks Turns:\t%d\n", data[4]);
	printf("Right Forward Ticks Turns:\t%d\n", data[5]);
	printf("Left Reverse Ticks Turns:\t%d\n", data[6]);
	printf("Right Reverse Ticks Turns:\t%d\n", data[7]);
	printf("Forward Distance:\t\t%d\n", data[8]);
	printf("Reverse Distance:\t\t%d\n", data[9]);
	printf("\n---------------------------------------\n\n");
}

void handleMessage(const char *buffer)
{
	printf("MESSAGE FROM ALEX: %s\n", &buffer[1]);
}
 
void handleUltrasonic(const char *buffer)
{
	int32_t data[16];
	memcpy(data, &buffer[1], sizeof(data));

	printf("Ultrasonic Reading: %d cm\n", data[0]);
}
void handleColor(const char *buffer)
{
	int32_t data[16];
	memcpy(data, &buffer[1], sizeof(data));
	printf("Color Sensor Reading: %d %d %d\n", data[0], data[1], data[2]);
	if (data[3] == 0)
	{
		printf("Red is detected\n");
	}
	else if (data[3] == 1)
	{
		printf("green is detected\n");
	}
	printf("distance from Red : %d, distance from Green: %d\n", data[4], data[5]);
	
}

void handleCommand(const char *buffer)
{
	// We don't do anything because we issue commands
	// but we don't get them. Put this here
	// for future expansion
}

void handleNetwork(const char *buffer, int len)
{
	// The first byte is the packet type
	int type = buffer[0];

	switch(type)
	{
		case NET_ERROR_PACKET:
		handleError(buffer);
		break;

		case NET_STATUS_PACKET:
		handleStatus(buffer);
		break;
		
		case NET_ULTRASONIC_PACKET:
		handleUltrasonic(buffer);
		break;
		
		case NET_COLOR_PACKET:
		handleColor(buffer);
		break;
		case NET_MESSAGE_PACKET:
		handleMessage(buffer);
		break;

		case NET_COMMAND_PACKET:
		handleCommand(buffer);
		break;
	}
	// Reset the actionInProgress flag after processing the response
    actionInProgress = 0;
}

void sendData(void *conn, const char *buffer, int len)
{
    int c;
    printf("\nSENDING %d BYTES DATA\n\n", len);
    if (networkActive && !actionInProgress) // Only send if no action is in progress
    {
        actionInProgress = 1; // Set the flag to indicate action in progress
        c = sslWrite(conn, buffer, len);

        if (c < 0)
        {
            perror("Error writing to server: ");
        }
        networkActive = (c > 0);
    }
    else if (actionInProgress)
    {
        printf("Action in progress. Ignoring new command.\n");
    }
}

void *readerThread(void *conn)
{
	char buffer[128];
	int len;

	while(networkActive)
	{
		/* TODO: Insert SSL read here into buffer */
	len = sslRead(conn, buffer, sizeof(buffer));

    if (len < 0)
    {
      perror("Error reading socket: ");
    }

    if (len > 0)
    {
      printf("read %d bytes from server.\n", len);
    }
		
		/* END TODO */

		networkActive = (len > 0);

		if(networkActive)
			handleNetwork(buffer, len);
	}

	printf("Exiting network listener thread\n");
    
    /* TODO: Stop the client loop and call EXIT_THREAD */
	stopClient();
    EXIT_THREAD(conn);
    /* END TODO */

    return NULL;
}

char getKeypress() {
	char key = 0;
	struct termios term_state = {0};
	if (tcgetattr(0, &term_state) < 0)
	  perror("tcsetattr()");
	term_state.c_lflag &= ~ICANON; //temporarily disable the canonical state
	term_state.c_lflag &= ~ECHO; //temporarily disable echo
	term_state.c_cc[VMIN] = 1;
	term_state.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &term_state) < 0)
	  perror("tcsetattr ICANNON");
	if (read(0, &key, 1) < 0)
	  perror("read()");
	term_state.c_lflag |= ICANON; //re-enable canonical state
	term_state.c_lflag |= ECHO; //re-enable echo
	if (tcsetattr(0, TCSADRAIN, &term_state) < 0)
	  perror("tcsetattr ~ICANON");
	return (key);
}  

void flushInput()
{
	char c;

	while((c = getchar()) != '\n' && c != EOF);
} 

void getParams(int32_t *params)
{
	printf("Enter distance/angle in cm/degrees (e.g. 50) and power in %% (e.g. 75) separated by space.\n");
	printf("E.g. 50 75 means go at 50 cm at 75%% power for forward/backward, or 50 degrees left or right turn at 75%%  power\n");
	scanf("%d %d", &params[0], &params[1]);
	flushInput();
}

void WASD_Command(void *conn, int *quit)
{
	printf("Enter WASD command (W=forward, A=left, S=stop, D=right), C=get color sensor reading, V=get ultrasonic reading, X=get stats, Z=clear	stats, J=open front claw, K=close front claw, P=dispense med kit \n");
	char ch = getKeypress();
	char buffer[10];
	int32_t params[2];
	buffer[0] = NET_COMMAND_PACKET;

	switch(ch)
	{
		case 'w': //forward
		case 'W':
			buffer[1] = 'f';
			params[0] = distance;
			params[1] = speed;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;
		case 's': //reverse
		case 'S':
			buffer[1] = 'b';
			params[0] = distance;
			params[1] = speed;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;
		case 'a': //left
		case 'A':
			buffer[1] = 'l';
			params[0] = angle;
			params[1] = turn_speed;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;
		case 'd': //right
		case 'D':
			buffer[1] = 'r';
			params[0] = angle;
			params[1] = turn_speed;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;
		
		case 'c': //get color sensor reading
		case 'C':
			buffer[1] = 'u';
			params[0] = 0;
			params[1] = 0;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;
		
		case 'v': //get ultrasonic reading
		case 'V':
			buffer[1] = 'v';
			params[0] = 0;
			params[1] = 0;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;
		
		case 'j': //open front claw
		case 'J':
			buffer[1] = 'j';
			params[0] = 0;
			params[1] = 0;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;
		
		case 'k': //close front claw
		case 'K':
			buffer[1] = 'k';
			params[0] = 0;
			params[1] = 0;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;
		case 'p': //Dispense med kit
		case 'P':
			buffer[1] = 'p';
			params[0] = 0;
			params[1] = 0;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;
		case '1': //gear 1
			speed = 50;
			distance = 2;
			turn_speed = 50;
			printf("Gear 1\n");
			break;
		case '2': //gear 2
			speed = 70;
			distance = 5;
			turn_speed = 70;
			printf("Gear 2\n");
			break;
		case '3':
			speed = 90;
			distance = 8;
			turn_speed = 90;
			printf("Gear 3\n");
			break;

		case 'x': //get stats
		case 'X':
			buffer[1] = 'g';
			params[0] = 0;
			params[1] = 0;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;

		case 'z': //clear stats
		case 'Z':
			buffer[1] = 'c';
			params[0] = 0;
			params[1] = 0;
			memcpy(&buffer[2], params, sizeof(params));
			sendData(conn, buffer, sizeof(buffer));
			break;

		case 'm': //switch to original command mode
		case 'M':
			printf("Switching to original command mode\n");
			mode = 0;
			break;
		case 'q':
		case 'Q':
			*quit=1;
			break;
		default:
			printf("BAD COMMAND\n");
	}
}

void Original_command(void *conn, int *quit)
{
	char ch;
	printf("Command (f=forward, b=reverse, l=turn left, r=turn right, s=stop, c=get color sensor reading, v=get ultrasonic reading, k=open claw, l=close claw, c=clear stats, g=get stats, q=exit m = switch mode)\n");
	scanf("%c", &ch);

	// Purge extraneous characters from input stream
	flushInput();

	char buffer[10];
	int32_t params[2];

	buffer[0] = NET_COMMAND_PACKET;
	switch(ch)
	{
		case 'f':
		case 'F':
		case 'b':
		case 'B':
		case 'l':
		case 'L':
		case 'r':
		case 'R':
					getParams(params);
					buffer[1] = ch;
					memcpy(&buffer[2], params, sizeof(params));
					sendData(conn, buffer, sizeof(buffer));
					break;
		case 's':
		case 'S':
		case 'c':
		case 'C':
		case 'g':
		case 'G':
		case 'u':
		case 'U':
		case 'v':
		case 'V':
		case 'j':
		case 'J':
		case 'k':
		case 'K':
		case 'p':
		case 'P':
				params[0]=0;
				params[1]=0;
				memcpy(&buffer[2], params, sizeof(params));
				buffer[1] = ch;
				sendData(conn, buffer, sizeof(buffer));
				break;
		case 'm':
		case 'M':
			printf("Switching to WASD mode\n");
			mode = 1;
			break;
		case 'q':
		case 'Q':
			*quit=1;
			break;
		default:
			printf("BAD COMMAND\n");
	}
}

void *writerThread(void *conn)
{
	int quit=0;
	printf("Default mode is WASD mode. Press 'm' to switch to original command mode.\n");
	printf("Press 'q' to exit.\n");

	while(!quit)
	{
		if (mode == 0)
		{
			Original_command(conn, &quit);
		}
		else
		{
			WASD_Command(conn, &quit);
		}
	}
	printf("Exiting keyboard thread\n");

    /* TODO: Stop the client loop and call EXIT_THREAD */
		stopClient();
  		EXIT_THREAD(conn);
    /* END TODO */

    return NULL;
}
				


/* TODO: #define filenames for the client private key, certificatea,
   CA filename, etc. that you need to create a client */

#define SERVER_NAME "172.20.10.14"
#define CA_CERT_FNAME "signing.pem"
#define PORT_NUM 5001
#define CLIENT_CERT_FNAME "laptop.crt"
#define CLIENT_KEY_FNAME "laptop.key"
#define SERVER_NAME_ON_CERT "aplus.com"
/* END TODO */
void connectToServer(const char *serverName, int portNum)
{
    /* TODO: Create a new client */
	createClient(SERVER_NAME, PORT_NUM, 1, CA_CERT_FNAME, SERVER_NAME_ON_CERT, 1,
               CLIENT_CERT_FNAME, CLIENT_KEY_FNAME, readerThread, writerThread);
    /* END TODO */
}

int main(int ac, char **av)
{
	if(ac != 3)
	{
		fprintf(stderr, "\n\n%s <IP address> <Port Number>\n\n", av[0]);
		exit(-1);
	}

    networkActive = 1;
    connectToServer(av[1], atoi(av[2]));

    /* TODO: Add in while loop to prevent main from exiting while the
    client loop is running */

	while (client_is_running());

    /* END TODO */
	printf("\nMAIN exiting\n\n");
}
