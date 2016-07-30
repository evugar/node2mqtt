#include <stdio.h>
#include <RF24/RF24.h>
#include <mosquitto.h>
#include <unistd.h>


void on_publish(struct mosquitto *mosq, void *userdata, int mid)
{
    //mosquitto_disconnect(mosq);
}


int main(int argc, char **argv)
{

    struct mosquitto *mosq;

	mosquitto_lib_init();
	mosq = mosquitto_new("id", true, NULL);
    mosquitto_publish_callback_set(mosq, on_publish);

    mosquitto_connect(mosq, "localhost", 1883, 0);




	RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ);	

	const uint8_t rxpipe[5] = {'N','o','d','e','0'};

	uint8_t payload[32];
//	char humidity[4];
//	char temperature[4];

	radio.begin();
	radio.enableDynamicPayloads();
	radio.setRetries(5,15);
	radio.printDetails();

	radio.openReadingPipe(1,rxpipe);
	radio.startListening();

	while (1) {

		if ( radio.available() )
		{
			// Dump the payloads until we've gotten everything
			uint8_t len;


			while (radio.available())
			{
				// Fetch the payload, and see if this was the last one.
				len = radio.getDynamicPayloadSize();
				radio.read( payload, len );

				// Put a zero at the end for easy printing
//				sprintf(humidity, "%d", payload[0]);
//				sprintf(temperature, "%d", payload[1]);

				// Spew it
				printf("Got payload size=%i value=%d,%d\n\r",len,payload[0],payload[1]);
//				mosquitto_publish(mosq, NULL, "nodes/01/humidity", strlen(humidity), humidity, 0, false);
//				mosquitto_publish(mosq, NULL, "nodes/01/temperature", strlen(temperature), temperature, 0, false);
				mosquitto_publish(mosq, NULL, "nodes/01/humidity", 1, &payload[0], 0, false);
				mosquitto_publish(mosq, NULL, "nodes/01/temperature", 1, &payload[1], 0, false);
				mosquitto_loop(mosq, -1, 1);  // this calls mosquitto_loop() in a loop, it will exit once the client disconnects cleanly
			}

		}
		sleep(1);
	}

	return 0;
}
