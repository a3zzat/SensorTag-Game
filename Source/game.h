

#ifndef _GAME_H_
#define  _GAME_H_
   
/*********************************************************************
 * INCLUDES
 */
#include "hal_types.h"

#define NUM_SAMPLES 10

#define TEMP_VAR_THR 2
#define XAXIS_VAR_THR 2
#define YAXIS_VAR_THR 2
#define ZAXIS_VAR_THR 2


/*********************************************************************
 * TYPEDEF
 */   
   
typedef enum myenum1{
     NoAction,
     ButtonLeftAction,
     ButtonRightAction,
     TempratureAction,
     ShakeAction,
     MoveUPAction,
     MoveDownAction,
     MoveLeftAction,
     MoveRightAction,
     TotalActionCount
   } ActionEnum;
 
typedef enum myenum2{
     SendAction,
     CheckAction,
     RcieveAction,
     SendResult,
     GameOver,
     CheckGameStart
   } StateEnum;
     
typedef enum myenum3{
     LeftKey,
     RightKey
   } KeyEnum;
     
  
     
typedef struct RingBuffer{
  
  uint16 buffer[NUM_SAMPLES];
  uint8 pointer;
 
}RingBuffer_t;

/*********************************************************************
 * FUNCTIONS
 */
void SetKeyVal(KeyEnum keyval);
void fillTempBuffer(uint8* tData);
void fillAcc(uint8* pBuf);

#endif 