#ifndef APP_H
#define APP_H

/*
 * -user_config:
 * this pin will be high while delay in setup function and in case of 
 * error after setup.
 */
#define INDICATOR_PIN 13

/*
 * -user_config:
 * if digitalRead(DEFAULT_CONTROL_PIN) returns BUTTON_DOWN on system startup,
 * all application parameters will be set to default values.
 * You can find default values in ./app/param.c, ./app/serials.c, ./model/channels.c
 * files.
 */
#define DEFAULT_CONTROL_PIN 2

/*
 * -user_config:
 * application will wait APP_UPLOAD_DELAY_MS before running main program.
 * This prevents from new sketch upload failure in case of some errors in
 * old sketch. You can always upload new sketch during APP_UPLOAD_DELAY_MS. 
 */
#define APP_UPLOAD_DELAY_MS 5000

/*
 * -user_config:
 * uncomment the following line if you want to enable serial server,
 * this allows you to communicate with device over serial interface
 */
#define SERIAL_SERVER

/*
 * -user_config:
 * uncomment the following line if you want to enable Application Object IDs,
 * this allows you to configure this application over serial server
 */
#define USE_AOIDS

/*
 * -user_config:
 * uncomment the following line if you want to enable Network Object IDs,
 * this makes your application objects accessible over serial network
 */
#define USE_NOIDS

/*
 * -user_config:
 * uncomment serial interfaces you want to use:
 */
#define WANT_SERIAL0
#define WANT_SERIAL1
#define WANT_SERIAL2
#define WANT_SERIAL3

/*
 * -user_config:
 * uncomment the following line if you need debug messages:
 */
#define MODE_DEBUG

#endif 
