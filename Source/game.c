
/*********************************************************************
 * INCLUDES
 */
#include <stdlib.h> // for malloc
#include "game.h"


/*********************************************************************
 * Globals
 */
  RingBuffer_t  TempBuffer;
  RingBuffer_t  ShakeXaxisBuffer;
  RingBuffer_t  ShakeYaxisBuffer;
  RingBuffer_t  ShakeZaxisBuffer; 
  KeyEnum KeysValue;
  
  
  

/*********************************************************************
 * FUNCTIONS
 */
   /*
void* createBuffer(uint8 size){
  
  void * myBuffer;
  myBuffer = malloc(size);
  
  return myBuffer;
  
}

void RemoveBuffer(void* Buffer){
  
  free(Buffer);
  
}

RingBuffer_t * CreateRingBuffer(uint8 size){
  RingBuffer_t * myRing;
  //void * innerBuffer;
  
  myRing = (RingBuffer_t * ) createBuffer(sizeof(RingBuffer_t));
  //innerBuffer = createBuffer(size*sizeof(uint8));
  //myRing -> buffer = innerBuffer;
  myRing -> size = size;
  myRing -> pointer = 0 ;
  
  return myRing;
}

void RemoveRingBuffer(RingBuffer_t * Buffer){
  
  free(Buffer);
  
}
*/
void fillRingBuffer(RingBuffer_t * Buffer,uint8 value){
  uint8 pointer = Buffer -> pointer;
  Buffer -> buffer[pointer] = value;
  Buffer -> pointer = (Buffer -> pointer) + 1 ;
  
  if(Buffer -> pointer > NUM_SAMPLES){
    Buffer -> pointer = 0;
  }
}

uint16 CalcAverage(RingBuffer_t * Buffer){
    uint16 Toatal = 0 ;
    uint16 Average = 0 ;
    uint8 i;
    
    for(i = 0; i<NUM_SAMPLES; i++){
        Toatal += Buffer->buffer[i];
    }
    Average = Toatal / (NUM_SAMPLES - 1);
    return Average;
}


uint16 CalcVariance(RingBuffer_t * Buffer){
  
    uint16 variance = 0 ;
    uint16 average = 0;
    uint16 total = 0;
    uint8 i;
    
    average = CalcAverage(Buffer);
    
    for(i = 0; i< NUM_SAMPLES; i++){
        total = (average - Buffer->buffer[i])*(average - Buffer->buffer[i]);
    }
    
    variance = total / (NUM_SAMPLES - 1);
    return variance;
}

void checkTempAction(ActionEnum* DoneAction){
  uint16 variance =0 ;
  
  variance = CalcVariance(&TempBuffer);
  
  if( variance >= TEMP_VAR_THR ) {
    *DoneAction = TempratureAction;
  }
  
}

void CheckShakeAction(ActionEnum* DoneAction){
  uint16 varianceX =0 ;
  uint16 varianceY =0 ;
  uint16 varianceZ =0 ;
  
  varianceX = CalcVariance(&ShakeXaxisBuffer);
  varianceY = CalcVariance(&ShakeYaxisBuffer);
  varianceZ = CalcVariance(&ShakeZaxisBuffer);
  
  if( (varianceX >= XAXIS_VAR_THR ) || (varianceY >= YAXIS_VAR_THR) || (varianceZ >= ZAXIS_VAR_THR)) {
    *DoneAction = ShakeAction ; 
  }
  
}

void CheckButtonAction(ActionEnum* DoneAction){

  if(KeysValue == LeftKey) {
    *DoneAction = ButtonLeftAction ; 
  }
  else if(KeysValue == RightKey) {
    *DoneAction = ButtonRightAction ; 
  }
  
}

void SetKeyVal(KeyEnum keyval){
  KeysValue = keyval;
}

void fillTempBuffer(uint8* tData){
  uint16 DataSample;
  uint8 high = tData[2];
  uint8 low = tData[3];
  
  DataSample = high << 8 + low;
  fillRingBuffer(&TempBuffer,DataSample);
}

/*********************************************************************
 * @fn      SensorTag_ProcessEvent
 *
 * @brief   Simple BLE Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint8 CheckCurrentAction(ActionEnum* DoneAction){
  
  *DoneAction = NoAction;
   
  checkTempAction(DoneAction);
  if(*DoneAction != NoAction){
    return 0;
  }
  CheckButtonAction(DoneAction);
    if(*DoneAction != NoAction){
    return 0;
  }
  CheckShakeAction(DoneAction);
    if(*DoneAction != NoAction){
    return 0;
  }
  return 1;
  
}

/*********************************************************************
 * @fn      SensorTag_ProcessEvent
 *
 * @brief   Simple BLE Peripheral Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
void getRandAction(ActionEnum* CurrentAction){
  
  uint8 RandomAction;
  RandomAction = rand() % TotalActionCount;
  
  *CurrentAction = RandomAction;
  
}



