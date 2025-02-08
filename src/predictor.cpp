//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <math.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Baimeedi Nithin Reddy";
const char *studentID = "A69029730";
const char *email = "nbaimeedi@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = {"Static", "Gshare",
                         "Tournament", "Custom"};

// define number of bits required for indexing the BHT here.
int ghistoryBits = 17; // Number of bits used for Global History
int bpType;            // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
// TODO: Add your own Branch Predictor data structures here
//
// gshare
uint8_t *bht_gshare;
uint64_t ghistory;

int tournament_ghistorybits = 12;
uint8_t *tournament_g_bht;
uint64_t tournament_ghistory;

int tournament_lhistory_bits = 10;
int tournament_pc_bits = 32;
uint8_t *tournament_l_bht;
uint8_t *tournament_l_lht;
uint64_t tournament_lhistory;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//

// gshare functions
void init_gshare()
{
  int bht_entries = 1 << ghistoryBits;
  bht_gshare = (uint8_t *)malloc(bht_entries * sizeof(uint8_t));
  int i = 0;
  for (i = 0; i < bht_entries; i++)
  {
    bht_gshare[i] = WN;
  }
  ghistory = 0;
}

uint8_t gshare_predict(uint32_t pc)
{
  // get lower ghistoryBits of pc
  uint32_t bht_entries = 1 << ghistoryBits;
  uint32_t pc_lower_bits = pc & (bht_entries - 1);
  uint32_t ghistory_lower_bits = ghistory & (bht_entries - 1);
  uint32_t index = pc_lower_bits ^ ghistory_lower_bits;
  switch (bht_gshare[index])
  {
  case WN:
    return NOTTAKEN;
  case SN:
    return NOTTAKEN;
  case WT:
    return TAKEN;
  case ST:
    return TAKEN;
  default:
    printf("Warning: Undefined state of entry in GSHARE BHT!\n");
    return NOTTAKEN;
  }
}

void train_gshare(uint32_t pc, uint8_t outcome)
{
  // get lower ghistoryBits of pc
  uint32_t bht_entries = 1 << ghistoryBits;
  uint32_t pc_lower_bits = pc & (bht_entries - 1);
  uint32_t ghistory_lower_bits = ghistory & (bht_entries - 1);
  uint32_t index = pc_lower_bits ^ ghistory_lower_bits;

  // Update state of entry in bht based on outcome
  switch (bht_gshare[index])
  {
  case WN:
    bht_gshare[index] = (outcome == TAKEN) ? WT : SN;
    break;
  case SN:
    bht_gshare[index] = (outcome == TAKEN) ? WN : SN;
    break;
  case WT:
    bht_gshare[index] = (outcome == TAKEN) ? ST : WN;
    break;
  case ST:
    bht_gshare[index] = (outcome == TAKEN) ? ST : WT;
    break;
  default:
    printf("Warning: Undefined state of entry in GSHARE BHT!\n");
    break;
  }

  // Update history register
  ghistory = ((ghistory << 1) | outcome);
}

void cleanup_gshare()
{
  free(bht_gshare);
}

void init_tournament()
{
  //initialize the global predictor of the tournament predictor

  //get the number of bht entries
  int tournament_g_bht_entries = 1 << tournament_ghistorybits;
  //assign memory for bht and use tournament_g_bht to point to the first address of bht
  tournament_g_bht = (uint8_t *)malloc(tournament_g_bht_entries * sizeof(uint8_t));
  //initialize each entry in bht to weakly not taken
  int i = 0;
  for(i = 0; i < tournament_g_bht_entries; i++)
  {
    tournament_g_bht[i] = WN;
  }
  //initialize the ghr to 0
  tournament_ghistory = 0;

  //initialize the local predictor of the tournament predictor

  //get the number of bht entries
  int tournament_l_bht_entries = 1 << tournament_lhistory_bits;
  //assign memory to bht and use tournament_l_bht to point to the first address of bht
  tournament_l_bht = (uint8_t *)malloc(tournament_l_bht_entries * sizeof(uint8_t));
  //get the number of lht entries 
  int tournament_l_lht_entries = 1 << tournament_pc_bits;
  //assign memory to lht and use tournament_l_lht to point to the first address of lht;
  tournament_l_lht = (uint8_t *)malloc(tournament_l_lht_entries * sizeof(uint8_t));
  //initialize the bht entries with weakly not taken
  int i = 0;
  for(i = 0; i < tournament_l_bht_entries; i++)
  {
    tournament_l_bht[i] = WN;
  }
  //initialize the lht entries to 0
  int j = 0;
  for(j = 0; j < tournament_l_lht_entries; j ++)
  {
    tournament_l_lht = 0;
  }
}

void init_predictor()
{
  switch (bpType)
  {
  case STATIC:
    break;
  case GSHARE:
    init_gshare();
    break;
  case TOURNAMENT:
    break;
  case CUSTOM:
    break;
  default:
    break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint32_t make_prediction(uint32_t pc, uint32_t target, uint32_t direct)
{

  // Make a prediction based on the bpType
  switch (bpType)
  {
  case STATIC:
    return TAKEN;
  case GSHARE:
    return gshare_predict(pc);
  case TOURNAMENT:
    return NOTTAKEN;
  case CUSTOM:
    return NOTTAKEN;
  default:
    break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

void train_predictor(uint32_t pc, uint32_t target, uint32_t outcome, uint32_t condition, uint32_t call, uint32_t ret, uint32_t direct)
{
  if (condition)
  {
    switch (bpType)
    {
    case STATIC:
      return;
    case GSHARE:
      return train_gshare(pc, outcome);
    case TOURNAMENT:
      return;
    case CUSTOM:
      return;
    default:
      break;
    }
  }
}
