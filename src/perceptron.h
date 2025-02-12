#include <stdio.h>
#include <string.h>
#include <math.h>


#define PERCEPTRONS 427
#define GHISTORY 60

uint32_t hash_pc(uint32_t pc)
{
  uint64_t temp = pc * GHISTORY;
  return (temp % PERCEPTRONS);
}

int16_t perceptron_weight[PERCEPTRONS][GHISTORY + 1];
int16_t perceptron_ghistory[GHISTORY];
int32_t perceptron_threshold;
uint32_t perceptron_prediction = NOTTAKEN;
uint8_t threshold_fail = 0;


void perceptron_saturation_counter(int16_t* counter_bits, uint8_t match){
  if(match)
  {
    if(*counter_bits != 255)
    {
      (*counter_bits)++;
    }
  }else
  {
    if(*counter_bits != -256)
    {
      (*counter_bits)--;
    }
  }
}

void perceptron_init(){
  perceptron_threshold = (int32_t)(2 * GHISTORY + 15);
  //printf("threshold: %d\n",perceptron_threshold);
  memset(perceptron_weight, 0, sizeof(int16_t) * PERCEPTRONS * (GHISTORY + 1));
  memset(perceptron_ghistory, 0, sizeof(uint16_t) * GHISTORY);
}



uint32_t perceptron_predict(uint32_t pc){
  uint32_t index = hash_pc(pc);
  //printf("hashed pc: %d\n",index);
  int16_t perceptron_function = perceptron_weight[index][0];
  //printf("perceptron_function: %d",perceptron_function);

  for(int i = 1 ; i <= GHISTORY ; i++)
  {
    if (perceptron_ghistory[i-1])
    {
      perceptron_function += perceptron_weight[index][i];
    }
    else
    {
      perceptron_function += -perceptron_weight[index][i];
    }
  }
  //printf("perceptron_function: %d",perceptron_function);
  perceptron_prediction = (perceptron_function >= 0) ? TAKEN : NOTTAKEN;
  //printf("prediction: %d\n",perceptron_prediction);
  if (perceptron_function < perceptron_threshold && perceptron_function > -perceptron_threshold)
  {
    threshold_fail = 1;
  }
  else
  {
    threshold_fail = 0;
  }
  //printf("threshold_fail: %d",threshold_fail);
  return perceptron_prediction;
}

void perceptron_train(uint32_t pc, uint32_t outcome){

  uint32_t index = hash_pc(pc);
  //printf("hashed pc: %d\n",index);

  if((perceptron_prediction != outcome) || threshold_fail)
  {
    perceptron_saturation_counter(&(perceptron_weight[index][0]), outcome);
    for(int i = 1 ; i <= GHISTORY ; i++){
      uint32_t predict = perceptron_ghistory[i-1];
      //printf("predict:%d\n",predict);
      perceptron_saturation_counter(&(perceptron_weight[index][i]), (outcome == predict));
    }

  }

  for(int i = GHISTORY - 1; i > 0 ; i--)
  {
    perceptron_ghistory[i] = perceptron_ghistory[i-1];
  }
  perceptron_ghistory[0] = outcome;
  //printf("perceptron ghistory: %d\n",perceptron_ghistory);

}
