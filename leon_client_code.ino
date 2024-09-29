#ifdef ESP8266
#include <ESP8266WiFi.h>  // esp8266
#else
#include <WiFi.h>  // esp32
#endif

const char* ssid = "Hello_Elecrow";
const char* password = "12345678";

const char* host = "192.168.4.1";
const uint16_t port = 22333;

const int simulatedLoadCellFlourValue = 500;  // Simulated load cell value for flour
const int simulatedLoadCellSugarValue = 300;  // Simulated load cell value for sugar
const int simulatedLoadCellBPValue = 200;
const int simulatedLoadCellCPValue = 300;
int flourValue = 0;
int sugarValue = 0;
int BPValue = 0;
int CPValue = 0;

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi network
  Serial.println();
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);  // Set to Station mode
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Attempt to connect to the server
  if (!connectToServer()) {
    Serial.println("Failed to connect to server");
    // You can add retry logic here
  }
}

int ingredientValues[4];

void loop() {
  if (client.connected()) {
    while (client.available()) {
      String receivedString = client.readString();  // Read the available data as a string

      // Check for the buffer message
      if (receivedString == "get ready") {
        Serial.println("Buffer message received. Preparing for next data.");
        continue;  // Skip processing the buffer message itself
      }

      Serial.println("Received: " + receivedString);  // Print the received string

      // Split the received string by commas


      int commaIndex;
      int flourIndex = receivedString.indexOf("flour:") + 6;  // Start of the flour value
      int sugarIndex = receivedString.indexOf(",sugar:");     // End of the flour value
      String flourStr = receivedString.substring(flourIndex, sugarIndex);
      ingredientValues[0] = flourStr.toInt();  // Convert to integer and store in the array

      if(ingredientValues[0] == 600)
      {
        Serial.println("HELP");
      }
      // Find and extract the sugar value
      int BPIndex = receivedString.indexOf(",BP:");  // End of the sugar value
      String sugarStr = receivedString.substring(sugarIndex + 7, BPIndex);
      ingredientValues[1] = sugarStr.toInt();  // Convert to integer and store in the array

      // Find and extract the BP value
      int CPIndex = receivedString.indexOf(",CP:");  // End of the BP value
      String BPStr = receivedString.substring(BPIndex + 4, CPIndex);
      ingredientValues[2] = BPStr.toInt();  // Convert to integer and store in the array

      // Find and extract the CP value
      String CPStr = receivedString.substring(CPIndex + 4);  // From CP start to the end
      ingredientValues[3] = CPStr.toInt();  // Convert to integer and store in the array


      processPair(ingredientValues);


      // Send acknowledgment to the server
      // client.write("ACK");
      // Serial.println("ACK sent to server");
    }
  } else {
    delay(500);
    Serial.println("Disconnected. Reconnecting...");
    connectToServer();
  }
}

bool connectToServer() {
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return false;
  }
  Serial.println("connected to server");
  return true;
}

void processPair(int* pair) {
  // int colonIndex = pair.indexOf(':');
  // if (colonIndex != -1) {
  //   String identifier = pair.substring(0, colonIndex);
  //   String valueString = pair.substring(colonIndex + 1);
  //   Serial.print("Identifier: ");
  //   Serial.println(identifier);
  //   Serial.print("Value: ");
  //   Serial.println(valueString);

    // Handle each identifier differently
    // if (identifier == "cakename") {
    // if (pair[0] == "flour") {
    //   flourValue = valueString.toInt();
    //   Serial.print("Received flour amount: ");
    //   Serial.println(flourValue);

    // } else if (identifier == "sugar") {
    //   sugarValue = valueString.toInt();
    //   Serial.print("Received sugar amount: ");
    //   Serial.println(sugarValue);

    // } else if (identifier == "BP") {
    //   BPValue = valueString.toInt();
    //   Serial.print("Received sugar amount: ");
    //   Serial.println(BPValue);

    // } else if (identifier == "CP") {
    //   CPValue = valueString.toInt();
    //   Serial.print("Received sugar amount: ");
    //   Serial.println(CPValue);
    // }

    if (pair[0] >= 0 && pair[1] >= 0 && pair[2] >= 0 && pair[3] >= 0) {
      // Process the logic for comparing requested amounts with simulated values

      String FlourBad = "and flour";
      String SugarBad = "and Sugar";
      String BP_Bad = "and BP";
      String CP_Bad = "and CP";

      String result = "NO GO ";

      // Check for all ingredients
      if (pair[0]  > simulatedLoadCellFlourValue && pair[1] > simulatedLoadCellSugarValue && pair[2] > simulatedLoadCellBPValue && pair[3] > simulatedLoadCellCPValue) {
        result = result + "all";
      } else {
        if (pair[0]  > simulatedLoadCellFlourValue) {
          result = result + FlourBad;
        }
        if (pair[1] > simulatedLoadCellSugarValue) {
          result = result + SugarBad;
        }
        if (pair[2] > simulatedLoadCellBPValue) {
          result = result + BP_Bad;
        }
        if (pair[3] > simulatedLoadCellCPValue) {
          result = result + CP_Bad;
        }
        if (pair[0]  < simulatedLoadCellFlourValue && pair[1] < simulatedLoadCellSugarValue && pair[2] < simulatedLoadCellBPValue && pair[3] < simulatedLoadCellCPValue) {
          result = "GO";
        }
      }



      client.write(result.c_str(), result.length());  // Condition: All required ingredients have enough supply
      Serial.println(result);


      // // Reset all values after processing to avoid duplicate messages
      // flourValue = 0;
      // sugarValue = 0;
      // BPValue = 0;
      // CPValue = 0;
    }


    // if (flourValue >= 0 && sugarValue >= 0) {
    //   // Now process the logic for comparing requested amounts with simulated values
    //   if (flourValue > simulatedLoadCellFlourValue && sugarValue > simulatedLoadCellSugarValue) {
    //     client.write("NO GO all");  // Condition 1: Not enough flour or sugar
    //     Serial.println("NO GO all");

    //   } else if (flourValue > simulatedLoadCellFlourValue && sugarValue <= simulatedLoadCellSugarValue) {
    //     client.write("NO GO flour");  // Condition 2: Not enough flour
    //     Serial.println("NO GO flour");

    //   } else if (flourValue <= simulatedLoadCellFlourValue && sugarValue > simulatedLoadCellSugarValue) {
    //     client.write("NO GO sugar");  // Condition 3: Not enough sugar
    //     Serial.println("NO GO sugar");

    //   } else {
    //     client.write("GO");  // Condition 4: Both amounts are okay
    //     Serial.println("GO");
    //   }

    //   // Reset flour and sugar values after processing to avoid duplicate messages
    //   flourValue = 0;
    //   sugarValue = 0;
    //   BPValue = 0;
    //   CPValue = 0;
    // }
    // }

    // if (flourValue <= simulatedLoadCellFlourValue && sugarValue <= simulatedLoadCellSugarValue) {
    //   client.write("GO");
    // } else if (flourValue > simulatedLoadCellFlourValue && sugarValue <= simulatedLoadCellSugarValue) {
    //   client.write("NO GO flour");
    // } else if (flourValue <= simulatedLoadCellFlourValue && sugarValue > simulatedLoadCellSugarValue) {
    //   client.write("NO GO sugar");
    // } else if (flourValue > simulatedLoadCellFlourValue && sugarValue > simulatedLoadCellSugarValue) {
    //   client.write("NO GO ALL");
    // }

    // if (sugarValue >= simulatedLoadCellSugarValue || flourValue >= simulatedLoadCellFlourValue) {
    //   client.write("NO GO all");  // Send "NO GO" to server
    //   Serial.println("NO GO all");
    // } else if (sugarValue < simulatedLoadCellSugarValue || flourValue >= simulatedLoadCellFlourValue) {
    //   client.write("NO GO flour");  // Send ACK to server
    //   Serial.println("NO GO flour");
    // } else if (flourValue < simulatedLoadCellFlourValue || sugarValue >= simulatedLoadCellSugarValue) {
    //   client.write("NO GO sugar");  // Send ACK to server
    //   Serial.println("NO GO sugar");
    // } else{
    //   client.write("GO");
    // }

    // Now process the logic for comparing requested amounts with simulated values
  
}