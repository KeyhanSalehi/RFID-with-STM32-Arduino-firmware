/**************************************************************************************
 * Library  
 *************************************************************************************/ 

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

/**************************************************************************************
 * Define  
 *************************************************************************************/ 

#define SS_PIN PA4     /**< GPIO A4   */ 
#define RST_PIN PA3   /**< GPIO A3   */ 
#define IRQ_PIN PB13 /**< Interrupt pin connected to IRQ */ 

/**************************************************************************************
 * Variable & Object  
 *************************************************************************************/ 

MFRC522 RFID(SS_PIN, RST_PIN);        /**< Creat a Object from MFRC522 library for RFID      */ 
volatile bool WriteID_Flag = false ; /**< If User Want Write Data on Card this flag is true */

/**************************************************************************************
 * Function prototype  
 *************************************************************************************/ 

bool Check_NewCard(void);               /**< Checks if a new RFID card is present                */
bool Read_UID(void);                   /**< Reads the UID (Unique Identifier) from an RFID card */
void Show_UID(void);                  /**< Display the UID of the RFID card                    */
void Show_Data(String Data);         /**< Displays data read from an RFID card                */
void WriteData_ToCard(String Data); /**< Writes the provided data to an RFID card            */
String ReadData_FromCard();        /**< Reads data from an RFID card                        */

/**************************************************************************************
 * Interrupt prototype  
 *************************************************************************************/ 

void WriteID_Interrupt(); /**< if User want ro wirte data on any card  */

/**************************************************************************************
 * Main Code    
 *************************************************************************************/ 

/**< Init STM32 & Peripheral */
void setup()
{
  Serial.begin(9600) ; /**< Init Serial1 Port */   
  SPI.begin();        /**< Init SPI          */
  RFID.PCD_Init();   /**< Init RFID Module  */
  
  pinMode(IRQ_PIN, INPUT_PULLUP); /**< Config GPIO B13 for input and PULLUP */
  attachInterrupt(digitalPinToInterrupt(IRQ_PIN), WriteID_Interrupt , FALLING); /**< Config Interrupt for GPIO B13  */
}

/**< infinity loop */
void loop() 
{
  if(!WriteID_Flag) 
  {
    if(Check_NewCard()) /**< If a card near to RFID */
    { 
      if(Read_UID()) /**< If read serial card successfully */
      {
        /**< Print UID card */
        Show_UID();
        /**< Print data from card */
        Show_Data(ReadData_FromCard());
        /**< Halt PICC */ 
        RFID.PICC_HaltA();
        /**< Stop encryption on PCD */ 
        RFID.PCD_StopCrypto1();
      }
      else /**< If read data faild */
      {
        Serial.println("\nRead serial card Faild! Try again");
      }
    }
    else /**< Check again and wait for find new card */
      return ;
  }
  else /**< If user want write data on card */
  {
    /**< Write data on card */
    Serial.println("\nPlease enter the data you want to write to the card:");
      /**< Read data from serial port */
        String Data = "";
        while (true) 
        {
          if (Serial.available()) 
          {
            char buffer = Serial.read();
            if (buffer == '\n' || buffer == '\r') 
            {
              if(Data.length() > 0) /**< if Data is fill */
              {   
                /**< End of input */ 
                break;
              }
            } 
            else 
            {
              /**< Append characters to Data */
              Data += buffer;
            }
          }
        }
        Serial.print("Data to write: ");
        Serial.println(Data);
      
      Serial.println("\nPlase near wich card you want write data on! ");
      /**< Wait for near a new card and Read UID Card */
      while(!Check_NewCard() || !Read_UID());
      
      WriteData_ToCard(Data);
      
      WriteID_Flag = false ; /**< Reset System */ 
  }   
}

/**************************************************************************************
 * Function   
 *************************************************************************************/ 

/**
 * @brief  Checks if a new RFID card is present.
 * @retval bool
 *         - true:  A new card has been detected.
 *         - false: No new card detected.
 */
bool Check_NewCard(void)
{
  /**< Look for a new card */
  if (RFID.PICC_IsNewCardPresent()) 
  {
    return true; /**< A new card has been detected. */
  }
  else
  {
    return false; /**< No new card detected. */
  }
}

/**
 * @brief  Reads the UID (Unique Identifier) from an RFID card.
 * @retval bool
 *         - true:  The UID was read successfully.
 *         - false: The UID read operation failed.
 */
bool Read_UID(void)
{
  /**< Check if a card is selected and its UID is readable. */
  if (RFID.PICC_ReadCardSerial()) 
  {
    return true; /**< Read Data Card successfully. */
  }
  else 
  {
    return false; /**< Read Data Card failed. */
  }
}

/**
  * @brief  Display the UID of the RFID card.
  * @note   This function prints the UID of the RFID card to the serial monitor. 
  *         It formats the UID as a hexadecimal string with leading zeros if necessary.
  * @retval None
  */
void Show_UID(void)
{
  /**< Print UID of the card. */
  Serial.print("\nUID tag: "); /**< Print the prefix for UID tag. */
  for (byte i = 0; i < RFID.uid.size; i++) 
  {
    Serial.print(RFID.uid.uidByte[i] < 0x10 ? " 0" : " "); /**< Print leading zero for bytes less than 0x10. */
    Serial.print(RFID.uid.uidByte[i], HEX); /**< Print each byte of the UID in hexadecimal format. */
  }
  Serial.println(); /**< Print a newline character to end the line. */
}

/**
 * @brief  Displays data read from an RFID card.
 * 
 * This function checks if the provided data string is not empty. 
 * If the string contains data, it will be displayed on the serial monitor. 
 * If the string is empty, it will notify the user that no data was found.
 * 
 * @param  Data  The string containing data read from the RFID card.
 *               This string is expected to be non-empty if data was successfully read.
 * 
 * @retval None
 */
void Show_Data(String Data)
{
  /**< Check if the Data string is not empty */ 
  if (Data[0] != '\0') 
  {
    Serial.print("Data from card : ");
    Serial.println(Data);  /**< Display the data read from the card */ 
  } 
  else /**< If the Data string is empty */
  {
    Serial.println("No data from card!"); /**< Inform the user that no data was read */
  }
}

/**
 * @brief  Writes the provided data to an RFID card.
 * 
 * This function attempts to write a string of data to a specific sector and block on an RFID card.
 * The function first authenticates with the card using the default key and then writes the data.
 * If the operation is successful, a confirmation message is printed to the serial monitor.
 * Otherwise, it displays an error message.
 * 
 * @param  Data  The string of data to be written to the RFID card.
 *               The data is expected to be less than or equal to 16 bytes.
 * 
 * @retval None
 */
void WriteData_ToCard(String Data) 
{
    Serial.println("\nCard detected. Writing data...");

    /**< Select the sector and block where you want to write data (to this project we use the same block) */ 
    byte sector = 1;  
    byte block = 4;  

    /**< Authenticate with the card using the default key */
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) 
      key.keyByte[i] = 0xFF;  /**< Default key: FFFFFFFFFFFF */

    MFRC522::StatusCode status;
    status = RFID.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, (sector * 4) + block, &key, &(RFID.uid));
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print("Authentication failed: ");
      Serial.println(RFID.GetStatusCodeName(status));
      return;
    }

    /**< Prepare data to write */ 
    byte buffer[16];
    /**< fill Buffer with NULL char */ 
    for(uint8_t i = 0 ; i < 16 ; i++)
      buffer[i] = '\0' ; 

    Data.getBytes(buffer, sizeof(buffer));

    /**< Write data to the block */ 
    status = RFID.MIFARE_Write((sector * 4) + block, buffer, 16);
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print("Write failed: ");
      Serial.println(RFID.GetStatusCodeName(status));
    } 
    else 
    {
      Serial.println("Data written successfully!");
    }

    /**< Stop crypto */ 
    RFID.PICC_HaltA();
    RFID.PCD_StopCrypto1();
}

/**
 * @brief  Reads data from an RFID card.
 * 
 * This function reads data from a specific sector and block on an RFID card.
 * The function first authenticates with the card using the default key and then reads the data.
 * The read data is converted into a string and returned by the function.
 * 
 * @param  None
 * 
 * @retval String  The data read from the RFID card as a string.
 *                 If the authentication or reading fails, the returned string may be empty.
 */
String ReadData_FromCard() 
{
    Serial.println("Card detected. Reading data...");

    /**< Select the sector and block where the data is stored (to this project we use the same block) */ 
    byte sector = 1;  
    byte block = 4;  

    /**< Authenticate with the card using the default key */ 
    MFRC522::MIFARE_Key key;
    for (byte i = 0; i < 6; i++) 
      key.keyByte[i] = 0xFF;  /**< Default key: FFFFFFFFFFFF */ 

    MFRC522::StatusCode status;
    status = RFID.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, (sector * 4) + block, &key, &(RFID.uid));
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print("Authentication failed: ");
      Serial.println(RFID.GetStatusCodeName(status));
      return ""; /**< Return an empty string if authentication fails */
    }

    /**< Read data from the block */ 
    byte buffer[18]; /**< Buffer to hold the data read from the card */ 
    byte size = sizeof(buffer);

    status = RFID.MIFARE_Read((sector * 4) + block, buffer, &size);
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print("Read failed: ");
      Serial.println(RFID.GetStatusCodeName(status));
      return ""; /**< Return an empty string if reading fails */
    } 
    
    /**< Convert the read data to a String */ 
    String Data = "";
    for (byte i = 0; i < 16; i++)
    {
      Data += (char)buffer[i];  /**< Append each byte as a character to the string */ 
    }

    /**< Stop crypto */ 
    RFID.PICC_HaltA();
    RFID.PCD_StopCrypto1();

    return Data; /**< Return the read data as a string */
}

/**************************************************************************************
 * Interrupt    
 *************************************************************************************/

/**
  * @brief  Interrupt Service Routine (ISR) for handling WriteID events.
  * @note   This ISR is triggered when a WriteID event occurs, setting the
  *         WriteID_Flag to true to indicate that the system should process
  *         WriteID operations for a nearby card.
  * @retval None
  */
void WriteID_Interrupt()
{
  WriteID_Flag = true; /**< Set the WriteID_Flag to true to signal that a WriteID event has occurred. */
}