#include <bluefruit.h>

BLEClientUart clientUart; // bleuart client

void setup()
{
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("Bluefruit52 Central BLEUART Example");
  Serial.println("-----------------------------------\n");
  
  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
  Bluefruit.begin(0, 1);
  
  Bluefruit.setName("LsCentral");


  // Init BLE Central Uart Serivce
  clientUart.begin();

  // Increase Blink rate to different from PrPh advertising mode
  Bluefruit.setConnLedInterval(250);

  /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Interval = 100 ms, window = 80 ms
   * - Don't use active scan
   * - Start(timeout) with timeout = 0 will scan forever (until connected)
   */
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(1000);                   // // 0 = Don't stop scanning after n seconds

  while (!Bluefruit.Central.connected()) delay(500);
  while (!clientUart.discovered()) delay(500);
  Serial.println("Connected");

}


void loop()
{
  /*
  if ( Serial.available() )
  {
    delay(2); // delay a bit for all characters to arrive
    
    char str[20+1] = { 0 };
    Serial.readBytes(str, 20);
    Serial.println(str);
    //clientUart.print( str );
  }
  */
}

/**
 * Callback invoked when scanner pick up an advertising data
 * @param report Structural advertising data
 */
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  // Check if advertising contain BleUart service
  if ( Bluefruit.Scanner.checkReportForService(report, clientUart) )
  {
    Serial.print("BLE UART service detected. Connecting ... ");

    // Connect to device with bleuart service in advertising
    Bluefruit.Central.connect(report);
  }else
  {      
    // For Softdevice v6: after received a report, scanner will be paused
    // We need to call Scanner resume() to continue scanning
    Bluefruit.Scanner.resume();
  }
}

/**
 * Callback invoked when an connection is established
 * @param conn_handle
 */
void connect_callback(uint16_t conn_handle)
{
  Serial.println("Connected");

  Serial.print("Discovering BLE Uart Service ... ");
  if ( clientUart.discover(conn_handle) )
  {
    Serial.println("Found it");

    Serial.println("Enable TXD's notify");
    clientUart.enableTXD();

    Serial.println("Ready to receive from peripheral");
  }else
  {
    Serial.println("Found NONE");
    
    // disconnect since we couldn't find bleuart service
    Bluefruit.disconnect(conn_handle);
  }  
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
  
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}

/**
 * Callback invoked when uart received data
 * @param uart_svc Reference object to the service where the data 
 * arrived. In this example it is clientUart
 */
void bleuart_rx_callback(BLEClientUart& uart_svc)
{
  Serial.print("[RX]: ");
  
  while ( uart_svc.available() )
  {
    Serial.print( (char) uart_svc.read() );
  }

  Serial.println();
}
