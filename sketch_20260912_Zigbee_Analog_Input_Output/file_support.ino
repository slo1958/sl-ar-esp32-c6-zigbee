#include <Preferences.h>

#define TRACE_FILES_INFO    Serial.print

#define PREFKEY_STORE "PREFSTORE"

#define FILEKEY_BASECOUNTER "KBASECNTR"
#define FILEKEY_CURRENTCOUNTER "KLASTCNTR"
#define FILEKEY_SAVEMARK "KSAVED"

#define FILEKEY_SAVEMARK_DEFAULT 0
#define FILEKEY_SAVEMARK_SAVED 10

Preferences preferences;

void file_put_uint32(const char * key, uint32_t value){

  TRACE_FILES_INFO ("Writing ");
  TRACE_FILES_INFO (key);
  TRACE_FILES_INFO (" : ");
  TRACE_FILES_INFO (value);
  TRACE_FILES_INFO ("\n");

  preferences.putUInt(key, value);

}

uint32_t file_get_uint32(const char * key, uint32_t default_value){
  uint32_t v = preferences.getUInt(key, default_value);

  TRACE_FILES_INFO ("Reading ");
  TRACE_FILES_INFO (key);
  TRACE_FILES_INFO (" : ");
  TRACE_FILES_INFO (v);
  TRACE_FILES_INFO (" (");
  TRACE_FILES_INFO (default_value);
  TRACE_FILES_INFO (")\n");

  return v;

}

uint32_t file_get_baseCounter() {
  return file_get_uint32(FILEKEY_BASECOUNTER, 0);
}

uint32_t file_get_saveMark() {
  return file_get_uint32(FILEKEY_SAVEMARK, 0);
}

uint32_t file_get_currentCounter() {
  return file_get_uint32(FILEKEY_CURRENTCOUNTER, 0);
}

void file_put_baseCounter(uint32_t value) {
  file_put_uint32(FILEKEY_BASECOUNTER, value);
  file_put_uint32(FILEKEY_SAVEMARK, FILEKEY_SAVEMARK_SAVED);
}

void file_put_currentCounter(uint32_t value) {
  file_put_uint32(FILEKEY_CURRENTCOUNTER, value);
  file_put_uint32(FILEKEY_SAVEMARK, FILEKEY_SAVEMARK_SAVED);
}


void files_setup(){
  
  preferences.begin(PREFKEY_STORE, false);

  size_t whatsLeft = preferences.freeEntries();  

  TRACE_FILES_INFO ("There are ");
  TRACE_FILES_INFO (whatsLeft);
  TRACE_FILES_INFO (" entries available in the namespace table.\n");
 
}

void files_loop(){
}
