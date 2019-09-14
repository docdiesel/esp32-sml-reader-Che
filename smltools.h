
#include <stdio.h>
#include <string.h>

/* ======================
 * config for sml keys 
 */
typedef struct {
  char name[8];
  char hextxt[18];
  int  offset;
  int  length;
  int  divisor;
} SMLKEY;

/*  #0 -> 1.8.0    
 *  #1 -> 2.8.0   
 *  #2 -> 16.7.0
 */
const SMLKEY smlkeys[numOfValues] = {
  {  "1.8.0\0", "77070100010800FF\0", 20, 10, 10 },
  {  "2.8.0\0", "77070100020800FF\0", 20, 10, 10 },
  { "16.7.0\0", "77070100100700FF\0", 14,  8, 10 }
};

const char* START = "1B1B1B1B01010101";
const char* STOP  = "1B1B1B1B1A";

/*==============================
 * convert byte to hex chars
 */
String byte2hex(int convertible)
{
  char hexdigit[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  char result[3];
  result[0] = hexdigit[ convertible >> 4   ];
  result[1] = hexdigit[ convertible &  0xF ];
  result[2] = '\0';
  return result;
}

/*=====================================================
 * check if buffer contains a full valid sml hex string
 */

char* contains_valid_sml(char* smldata)
{
  char* p1 = strstr( (char *) smldata, (char *) START );
  char* p2 = strstr( (char *) smldata, (char *) STOP  );
  if ( (p1 != NULL) & (p2 != NULL) & (p2>p1)) {
    char* p3 = p2 + strlen(STOP);
    *p3 = '\0';
    return p1;
  }
  return NULL;
}

/*================================
 * convert hex string to int value
 */

signed long long twos_complement( char * hexstr, int bits){
  char* endptr;
  signed long long int value;

  value = strtoll(hexstr, &endptr, 16);
  if (value >= (1LL << (bits - 1)) ){
      value -= (1LL << bits);
  }
  return value;
}

/*========================================================
 * find sml value in hex string and convert it to a number
 */

long long ValFromSmlHex(char* smlhex, char* key, int offset, int numchars)
{
    #define BUF32CHRSIZ 32
    char   buf32chr[BUF32CHRSIZ]; // temp hex char buffer
    char*  keypos;
    int    keylen;

    //-- validate input
    if ( key == NULL )                   { Serial.println("*key is NULL\n");               return -1.0;}
    keylen = strlen(key);
    if ( keylen == 0 )                   { Serial.println("key is empty string\n");        return -1.0;}
    if ( numchars >= BUF32CHRSIZ-3 )     { Serial.println("numchars is too big\n");        return -1.0;}
    if ( (offset < 0) || (numchars<=0) ) { Serial.println("invalid offset or numchars\n"); return -1.0;} 

    //-- find sml key text
    keypos = strstr( smlhex, key );

    if (keypos != NULL) {
      //-- copy to buffer & add zero byte for end of string 
      buf32chr[0] = '0';
      buf32chr[1] = 'x';
      strncpy( &buf32chr[2], keypos + keylen + offset, numchars );
      buf32chr[numchars+2] = '\0';

      //-- convert to float and return
      return twos_complement(buf32chr,32) ;
    } else {
      Serial.print("key not found: "); Serial.println(key);
      return NULL;
    }
}

/*==============================
 * fill read-buffer with zeroes
 */
void resetReadBuffer(void) {
  bufix = 0;
  for (int n=0; n<bufsz; n++){ myBuffer[n] = '\0';}
}

/*==============================================================
 * read from serial line into buffer, both being global objects
 * read and if necc. shift data through buffer until it contains
 * one full sml message
 */
void readIntoBuffer(void){
  int num_bytes_avail = SerialX.available();
  if (num_bytes_avail*2 > bufsz) {
    Serial.println("number of bytes on Serial2 to big to save:" + String(num_bytes_avail) );
    while (SerialX.available()) {
      hx = byte2hex(SerialX.read()); // read and discard
    }
  } else {
    // if not enough space, then move some bytes
    if ( (bufsz - 1 - bufix) < (2*num_bytes_avail) ) {
      Serial.println("Not enough space; resetting buffer...");
      resetReadBuffer();
    } else {
      // read and save as hex chars
      for(int i=0; i<num_bytes_avail; i++){
        String hx = byte2hex(SerialX.read());
        myBuffer[bufix+0] = hx[0];
        myBuffer[bufix+1] = hx[1];
        bufix +=2;
        myBuffer[bufix] = '\0';
      }
      Serial.println("read " + String(num_bytes_avail) + " bytes, bufix is " +  String(bufix));
    }
  }
}


/*==============================
 * extract data from sml string
 */
signed  long long extractDataFrom( char* hexstring, int key){
  signed long long raw = ValFromSmlHex( hexstring, (char *) smlkeys[key].hextxt, smlkeys[key].offset, smlkeys[key].length );
  signed long  val = div( raw, 10 ).quot;
  if ( val == NULL ) {
    Serial.println("Error getting value from hex string\n");
  }
  return val;
}
