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
const char *studentName = "Samhita Varambally";
const char *studentID = "A69036092";
const char *email = "samhitav@google.com";

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

//21264
uint8_t *local_bht;
uint64_t *local_pht;
int pcBits = 11;
int lhistoryBits = 15;
uint64_t pathhistory;
int pathhistoryBits = 16;
uint8_t *global_bht;
uint8_t *choice_bht;
int choicehistoryBits = 12;

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

void init_alpha21264()
{
    int lpht_entries = 1 << pcBits;
    local_pht = (uint64_t *)malloc(lpht_entries * sizeof(uint64_t));
    int lbht_entries = 1 << lhistoryBits;
    local_bht = (uint8_t *)malloc(lbht_entries * sizeof(uint8_t));
    
    int gbht_entries = 1 << pathhistoryBits;
    global_bht = (uint8_t *)malloc(gbht_entries * sizeof(uint8_t));
    int cbht_entries = 1 << choicehistoryBits;
    choice_bht = (uint8_t *)malloc(cbht_entries * sizeof(uint8_t));

    int i;
    for (i = 0; i < lpht_entries; i++) 
    {
        local_pht[i] = 0;
    }
    for (i = 0; i < lbht_entries; i++) 
    {
        local_bht[i] = WN;
    }
    for (i = 0; i < gbht_entries; i++) 
    {
        global_bht[i] = WN;
    }
    for (i = 0; i < cbht_entries; i++) 
    {
        choice_bht[i] = WT;
    }
    pathhistory = 0;

}

void init_custom()
{
    int lpht_entries = 1 << pcBits;
    local_pht = (uint64_t *)malloc(lpht_entries * sizeof(uint64_t));
    int lbht_entries = 1 << lhistoryBits;
    local_bht = (uint8_t *)malloc(lbht_entries * sizeof(uint8_t));
    
    int gbht_entries = 1 << pathhistoryBits;
    global_bht = (uint8_t *)malloc(gbht_entries * sizeof(uint8_t));
    int cbht_entries = 1 << choicehistoryBits;
    choice_bht = (uint8_t *)malloc(cbht_entries * sizeof(uint8_t));

    int i;
    for (i = 0; i < lpht_entries; i++) 
    {
        local_pht[i] = 0;
    }
    for (i = 0; i < lbht_entries; i++) 
    {
        local_bht[i] = WN;
    }
    for (i = 0; i < gbht_entries; i++) 
    {
        global_bht[i] = WN;
    }
    for (i = 0; i < cbht_entries; i++) 
    {
        choice_bht[i] = WT;
    }
    pathhistory = 0;

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

uint8_t alpha21264_local_predict(uint8_t bht)
{
  switch (bht)
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
    printf("Warning: Undefined state of entry in Alpha 21264 Local BHT!\n");
    return NOTTAKEN;
  }
}

uint8_t alpha21264_global_predict(uint8_t bht)
{
  switch (bht)
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
    printf("Warning: Undefined state of entry in Alpha 21264 Global BHT!\n");
    return NOTTAKEN;
  }
}

uint8_t alpha21264_predict(uint32_t pc)
{
    uint32_t pc_lower_bits = pc & ((1 << pcBits) - 1);
    uint32_t lpht_lower_bits = local_pht[pc_lower_bits] & ((1 << lhistoryBits) - 1);

    uint32_t phistory_lower_bits = (pathhistory) & ((1 << pathhistoryBits) - 1);
    uint32_t chistory_lower_bits = (pathhistory) & ((1 << choicehistoryBits) - 1);
    switch (choice_bht[chistory_lower_bits])
    {
    case WN:
        return alpha21264_local_predict(local_bht[lpht_lower_bits]);        
    case SN:
        return alpha21264_local_predict(local_bht[lpht_lower_bits]);        
    case WT:
        return alpha21264_global_predict(global_bht[phistory_lower_bits]);        
    case ST:
        return alpha21264_global_predict(global_bht[phistory_lower_bits]);        
    default:
      printf("Warning: Undefined state of entry in Alpha 21264 Choice BHT!\n");
      return alpha21264_local_predict(local_bht[lpht_lower_bits]);        
    }
}

uint8_t custom_predict(uint32_t pc)
{
    uint32_t pc_lower_bits = pc & ((1 << pcBits) - 1);
    uint32_t lpht_lower_bits = local_pht[pc_lower_bits] & ((1 << lhistoryBits) - 1);

    uint32_t phistory_lower_bits = (pathhistory^pc) & ((1 << pathhistoryBits) - 1);
    uint32_t chistory_lower_bits = (pathhistory^pc) & ((1 << choicehistoryBits) - 1);
    switch (choice_bht[chistory_lower_bits])
    {
    case WN:
        return alpha21264_local_predict(local_bht[lpht_lower_bits]);        
    case SN:
        return alpha21264_local_predict(local_bht[lpht_lower_bits]);        
    case WT:
        return alpha21264_global_predict(global_bht[phistory_lower_bits]);        
    case ST:
        return alpha21264_global_predict(global_bht[phistory_lower_bits]);        
    default:
      printf("Warning: Undefined state of entry in Custom Choice BHT!\n");
      return alpha21264_local_predict(local_bht[lpht_lower_bits]);        
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

void train_alpha21264(uint32_t pc, uint8_t outcome)
{
  uint32_t pc_lower_bits = pc & ((1 << pcBits) - 1);
  uint32_t lpht_lower_bits = local_pht[pc_lower_bits] & ((1 << lhistoryBits) - 1);

  uint32_t phistory_lower_bits = (pathhistory) & ((1 << pathhistoryBits) - 1);
  uint32_t chistory_lower_bits = (pathhistory) & ((1 << choicehistoryBits) - 1);

  //Update choice history based on outcome
  if(alpha21264_global_predict(global_bht[phistory_lower_bits]) != alpha21264_local_predict(local_bht[lpht_lower_bits])) {
    switch (choice_bht[chistory_lower_bits])
    {
    case WN:
        choice_bht[chistory_lower_bits] = (alpha21264_predict(pc) == outcome) ? SN : WT;
        break;
    case SN:
        choice_bht[chistory_lower_bits] = (alpha21264_predict(pc) == outcome) ? SN : WN;
        break;
    case WT:
        choice_bht[chistory_lower_bits] = (alpha21264_predict(pc) == outcome) ? ST : WN;
        break;
    case ST:
        choice_bht[chistory_lower_bits] = (alpha21264_predict(pc) == outcome) ? ST : WT;
        break;
    default:
      printf("Warning: Undefined state of entry in Alpha 21264 Choice BHT!\n");
      break;
    }
  }

  // Update global history based on outcome
  switch (global_bht[phistory_lower_bits])
  {
    case WN:
        global_bht[phistory_lower_bits] = (outcome == TAKEN) ? WT : SN;
        break;
    case SN:
        global_bht[phistory_lower_bits] = (outcome == TAKEN) ? WN : SN;
        break;
    case WT:
        global_bht[phistory_lower_bits] = (outcome == TAKEN) ? ST : WN;
        break;
    case ST:
        global_bht[phistory_lower_bits] = (outcome == TAKEN) ? ST : WT;
        break;
    default:
      printf("Warning: Undefined state of entry in Alpha 21264 Global BHT!\n");
      return;
  }

  // Update local history based on outcome
  switch (local_bht[lpht_lower_bits])
  {
    case WN:
        local_bht[lpht_lower_bits] = (outcome == TAKEN) ? WT : SN;
        break;
    case SN:
        local_bht[lpht_lower_bits] = (outcome == TAKEN) ? WN : SN;
        break;
    case WT:
        local_bht[lpht_lower_bits] = (outcome == TAKEN) ? ST : WN;
        break;
    case ST:
        local_bht[lpht_lower_bits] = (outcome == TAKEN) ? ST : WT;
        break;
    default:
      printf("Warning: Undefined state of entry in Alpha 21264 Local BHT!\n");
      return;
  }

  pathhistory = ((pathhistory << 1) | outcome);
  local_pht[pc_lower_bits] = ((local_pht[pc_lower_bits] << 1) | outcome);
}

void train_custom(uint32_t pc, uint8_t outcome)
{
  uint32_t pc_lower_bits = pc & ((1 << pcBits) - 1);
  uint32_t lpht_lower_bits = local_pht[pc_lower_bits] & ((1 << lhistoryBits) - 1);

  uint32_t phistory_lower_bits = (pathhistory ^ pc) & ((1 << pathhistoryBits) - 1);
  uint32_t chistory_lower_bits = (pathhistory ^ pc) & ((1 << choicehistoryBits) - 1);

  //Update choice history based on outcome
  if(alpha21264_global_predict(global_bht[phistory_lower_bits]) != alpha21264_local_predict(local_bht[lpht_lower_bits])) {
    switch (choice_bht[chistory_lower_bits])
    {
    case WN:
        choice_bht[chistory_lower_bits] = (custom_predict(pc) == outcome) ? SN : WT;
        break;
    case SN:
        choice_bht[chistory_lower_bits] = (custom_predict(pc) == outcome) ? SN : WN;
        break;
    case WT:
        choice_bht[chistory_lower_bits] = (custom_predict(pc) == outcome) ? ST : WN;
        break;
    case ST:
        choice_bht[chistory_lower_bits] = (custom_predict(pc) == outcome) ? ST : WT;
        break;
    default:
      printf("Warning: Undefined state of entry in Custom Choice BHT!\n");
      break;
    }
  }

  // Update global history based on outcome
  switch (global_bht[phistory_lower_bits])
  {
    case WN:
        global_bht[phistory_lower_bits] = (outcome == TAKEN) ? WT : SN;
        break;
    case SN:
        global_bht[phistory_lower_bits] = (outcome == TAKEN) ? WN : SN;
        break;
    case WT:
        global_bht[phistory_lower_bits] = (outcome == TAKEN) ? ST : WN;
        break;
    case ST:
        global_bht[phistory_lower_bits] = (outcome == TAKEN) ? ST : WT;
        break;
    default:
      printf("Warning: Undefined state of entry in Alpha 21264 Global BHT!\n");
      return;
  }

  // Update local history based on outcome
  switch (local_bht[lpht_lower_bits])
  {
    case WN:
        local_bht[lpht_lower_bits] = (outcome == TAKEN) ? WT : SN;
        break;
    case SN:
        local_bht[lpht_lower_bits] = (outcome == TAKEN) ? WN : SN;
        break;
    case WT:
        local_bht[lpht_lower_bits] = (outcome == TAKEN) ? ST : WN;
        break;
    case ST:
        local_bht[lpht_lower_bits] = (outcome == TAKEN) ? ST : WT;
        break;
    default:
      printf("Warning: Undefined state of entry in Alpha 21264 Local BHT!\n");
      return;
  }

  pathhistory = ((pathhistory << 1) | outcome);
  local_pht[pc_lower_bits] = ((local_pht[pc_lower_bits] << 1) | outcome);
}

void cleanup_gshare()
{
  free(bht_gshare);
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
    init_alpha21264();
    break;
  case CUSTOM:
    init_custom();
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
    return alpha21264_predict(pc);
  case CUSTOM:
    return custom_predict(pc);
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
      return train_alpha21264(pc, outcome);
    case CUSTOM:
      return train_custom(pc, outcome);;
    default:
      break;
    }
  }
}
