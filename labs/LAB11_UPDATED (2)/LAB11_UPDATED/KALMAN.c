#include <stdint.h>
#include "..//inc//tm4c123gh6pm.h"
#include "KALMAN.h"


//timeStampNew is how muhc millis has progressed so basically millis - timer variable
void Kalman_Predict(Kalman& axisKalman , float accelerationThisAxis, float timestampNow ) {
  float  deltaT = timestampNow - axisKalman.currentStateTimestampSeconds;

  recreateControlMatrix(axisKalman , deltaT);
  recreateStateTransitionMatrix(axisKalman , deltaT);

  axisKalman.u[0] = (accelerationThisAxis);

  //REmember rows * columns so in this case A's rows * currentstates columsn
  float currentState_placeholder [2] = {0};

    currentState_placeholder[0] = (axisKalman.A[0] * (axisKalman.currentState[0]) + axisKalman.A[(1)] * (axisKalman.currentState[1]));
    currentState_placeholder[1] = (axisKalman.A[2] * (axisKalman.currentState[0]) + axisKalman.A[(3)] * (axisKalman.currentState[1]));

  axisKalman.currentState[0] =  currentState_placeholder[0];
  axisKalman.currentState[1] =  currentState_placeholder[1];
  //B is a 2*1 2 rows 1 column 
  float B_scalar_0_index = axisKalman.B[0] * axisKalman.u[0];
  float B_scalar_1_index = axisKalman.B[1] * axisKalman.u[0];
  axisKalman.currentState[0] += B_scalar_0_index;
  axisKalman.currentState[1] += B_scalar_1_index;

  //THis is a 2X2  multiplied by 2X2 
  //so 4 iteration
  float P0 = 0;
float P1 = 0;
float P2 = 0;
float P3 = 0;

int i = 0;
//A transpose[0] = 0 1 = 2, 2= 1, 3 = 3

    P0 = (axisKalman.A[1*i] * (axisKalman.P[0]) + axisKalman.A[(1*i) + 1] * (axisKalman.P[(0) + 2]));
    P1 = (axisKalman.A[1*i] * axisKalman.P[1]) + axisKalman.A[(1*i) + 1] * (axisKalman.P[(1) + 2]);
    i=2;
    
    P2 = (axisKalman.A[2] * (axisKalman.P[0]) + axisKalman.A[(i) + 1] * (axisKalman.P[(0) + 2]));
    P3 = (axisKalman.A[2] * (axisKalman.P[1]) +axisKalman.A[(i) + 1] * (axisKalman.P[(1) + 2]));
//Correct up to here-Still checking
    axisKalman.P[0] = (P0 * (axisKalman.A[0]) + P1 * axisKalman.A[1]) + axisKalman.Q[0];
    axisKalman.P[1] = (P0 * (axisKalman.A[2]) + P1 * axisKalman.A[3]) + axisKalman.Q[1];

    axisKalman.P[2] = (P2 * (axisKalman.A[0]) + P3 * axisKalman.A[1])+ axisKalman.Q[2];
    axisKalman.P[3] = (P2 * (axisKalman.A[2]) + P3 * axisKalman.A[3])+ axisKalman.Q[3];

    //Still correct

  //P = ((A.MultipliedBy(P)).MultipliedBy(A.Transpose())).Add(Q)

  axisKalman.currentStateTimestampSeconds = timestampNow;
}

int Update(Kalman& axisKalman, float position_axis, float velocity_axis){
  //Serial.print("CurrentState: "); Serial.println(axisKalman.currentState[0]);
  axisKalman.z[0] = position_axis;
  axisKalman.z[1] = velocity_axis;

  float y[2] = {0};
  y[0] = axisKalman.z[0] - axisKalman.currentState[0];

  y[1] = axisKalman.z[1] - axisKalman.currentState[1];

  float s[4] = {0};

  //R IS A @X2 matric because of how his add function works 

  s[0] = axisKalman.P[0] + axisKalman.R[0];
  s[1] = axisKalman.P[1] + axisKalman.R[1];
  s[2] = axisKalman.P[2] + axisKalman.R[2];
  s[3] = axisKalman.P[3] + axisKalman.R[3];

  float sInverse[4] = {0};

  int inversion_succ_flag = MatrixInversion(s, 2, sInverse);
  if (inversion_succ_flag == 0){
    return 0;
  }
 int i = 0;

 float K[4] = {0};
  K[0] = (axisKalman.P[1*i] * (sInverse[0]) + axisKalman.P[(1*i) + 1] * (sInverse[(0) + 2]));
    K[1] = (axisKalman.P[1*i] * (sInverse[1]) + axisKalman.P[(1*i) + 1] * (sInverse[(1) + 2]));
    i=2;
    K[2] = (axisKalman.P[(i)] * (sInverse[0]) + axisKalman.P[(i) + 1] * (sInverse[(0) + 2]));
    K[3] = (axisKalman.P[(i)] * (sInverse[1]) + axisKalman.P[(1) + 1] * (sInverse[(1) + 2]));

   float K_placeholder[2] = {0};
   i = 0;
   K_placeholder[0] = (K[i] * (y[0]) + K[(i) + 1] * (y[(0) + 1]));
    //K_placeholder[1] = (K[1*i] * (sInverse[1]) + K[(1*i) + 1] * (sInverse[(1) + 2]));
    i=2;
    
    K_placeholder[1] = (K[i] * y[0]) + K[(i) + 1] * (y[(0) + 1]);
   // K_placeholder[3] = (K[1*i + 1] * (sInverse[1]) + K[(1*i) + 1] * (sInverse[(1) + 2]));

   axisKalman.currentState[0] += K_placeholder[0];
   axisKalman.currentState[1] += K_placeholder[1];

   float NewIdent[4] = {1,0,0,1};
   NewIdent[0] -= K[0]; 
   NewIdent[1] -= K[1];
   NewIdent[2] -= K[2];
   NewIdent[3] -= K[3];

   float placeholder_New_Ident[4] = {0};
   i = 0;

   placeholder_New_Ident[0] = (NewIdent[1*i] * (axisKalman.P[0]) + NewIdent[(1*i) + 1] * (axisKalman.P[(0) + 2]));
    placeholder_New_Ident[1] = (NewIdent[1*i] * (axisKalman.P[1]) + NewIdent[(1*i) + 1] * (axisKalman.P[(1) + 2]));
    i=2;
    placeholder_New_Ident[2] = (NewIdent[(i)] * (axisKalman.P[0]) + NewIdent[(i) + 1] * (axisKalman.P[(0) + 2]));
    placeholder_New_Ident[3] = (NewIdent[(i)] * (axisKalman.P[1]) + NewIdent[(1) + 1] * (axisKalman.P[(1) + 2]));

    for(int j = 0; j< 4; j++){
      axisKalman.P[i] = placeholder_New_Ident[i];
    }

    //Serial.print("NewtState: "); Serial.println(axisKalman.currentState[0]);
    return 1;
    }

void recreateStateTransitionMatrix(Kalman& axisKalman, float deltaSeconds){
  //float .5 * pow(deltaSeconds, 2);
  axisKalman.A[0] = 1;
  axisKalman.A[1] = deltaSeconds;
  axisKalman.A[2] = 0;
  axisKalman.A[3] = 1;
}

void recreateControlMatrix(Kalman& axisKalman, float deltaSeconds) {
  float dtSquared = 0.5 * deltaSeconds * deltaSeconds;
  axisKalman.B[1] = deltaSeconds;
  axisKalman.B[0] = dtSquared;
}

float GetPredictedPosition(Kalman& axisKalman){
  return axisKalman.currentState[0];
  
  
}
float GetPredictedVelocity(Kalman& axisKalman){
  return axisKalman.currentState[1];
}
   

  //currentState[0] 

void NewKalmanFilterFusedPositionAccelerometer(Kalman& axisKalman , float initialPosition,
  float positionStandardDeviation,
  float accelerometerStandardDeviation,
  float currentTimestampSeconds) {

  for(int i =0; i< 2; i++){
    axisKalman.currentState[i] = 0;
  }
//2x1
  axisKalman.currentState[0] = initialPosition;
  axisKalman.currentState[1] =0;
//1x1
  axisKalman.u[0]  = 0;
  //2X1
  axisKalman.z[0] = 0;
  axisKalman.z[1] = 0;
  for(int i =0; i< 4; i++){
   axisKalman.H[i] = 0;
  }
   axisKalman.H[0] = 1;
  axisKalman.H[3] = 1;
  for(int i =0; i< 4; i++){
    axisKalman.P[i] = 0;
  }
  axisKalman.P[0] = 1;
  axisKalman.P[3] = 1;

  for(int i =0; i< 4; i++){
    axisKalman.Q[i] = accelerometerStandardDeviation*accelerometerStandardDeviation;
  }
  axisKalman.Q[2] = 0;
  axisKalman.Q[1] = 0;


for(int i =0; i< 4; i++){
    axisKalman.R[i] = positionStandardDeviation*positionStandardDeviation;
  }
  axisKalman.R[2] = 0;
  axisKalman.R[1] = 0;
axisKalman.B[0] = 0;axisKalman.B[1] = 0;
 for(int i =0; i< 4; i++){
   axisKalman.A[i] = 0;
  }
}

int MatrixInversion(float* A, int n, float* AInverse)
{
// A = input matrix (n x n)
// n = dimension of A 
// AInverse = inverted matrix (n x n)
// This function inverts a matrix based on the Gauss Jordan method.
// The function returns 1 on success, 0 on failure.
int i, j, iPass, imx, icol, irow;
float det, temp, pivot, factor;
float* ac = (float*)calloc(n*n, sizeof(float));
det = 1;
for (i = 0; i < n; i++)
     {
     for (j = 0; j < n; j++)
           {
           AInverse[n*i+j] = 0;
           ac[n*i+j] = A[n*i+j];
           }
     AInverse[n*i+i] = 1;
     }
// The current pivot row is iPass.  
// For each pass, first find the maximum element in the pivot column.
for (iPass = 0; iPass < n; iPass++)
     {
     imx = iPass;
     for (irow = iPass; irow < n; irow++)
           {
           if (fabs(A[n*irow+iPass]) > fabs(A[n*imx+iPass])) imx = irow;
           }
     // Interchange the elements of row iPass and row imx in both A and AInverse.
     if (imx != iPass)
           {
           for (icol = 0; icol < n; icol++)
                 {
                 temp = AInverse[n*iPass+icol];
                 AInverse[n*iPass+icol] = AInverse[n*imx+icol];
                 AInverse[n*imx+icol] = temp;
                 if (icol >= iPass)
                       {
                       temp = A[n*iPass+icol];
                       A[n*iPass+icol] = A[n*imx+icol];
                       A[n*imx+icol] = temp;
                       }
                 }
           }
     // The current pivot is now A[iPass][iPass].
     // The determinant is the product of the pivot elements.
     pivot = A[n*iPass+iPass];
     det = det * pivot;
     if (det == 0) 
           {
           free(ac);
           return 0;
           }
     for (icol = 0; icol < n; icol++)
           {
           // Normalize the pivot row by dividing by the pivot element.
           AInverse[n*iPass+icol] = AInverse[n*iPass+icol] / pivot;
           if (icol >= iPass) A[n*iPass+icol] = A[n*iPass+icol] / pivot;
           }
     for (irow = 0; irow < n; irow++)
           // Add a multiple of the pivot row to each row.  The multiple factor 
           // is chosen so that the element of A on the pivot column is 0.
           {
           if (irow != iPass) factor = A[n*irow+iPass];
           for (icol = 0; icol < n; icol++)
                 {
                 if (irow != iPass)
                       {
                       AInverse[n*irow+icol] -= factor * AInverse[n*iPass+icol];
                       A[n*irow+icol] -= factor * A[n*iPass+icol];
                       }
                 }
           }
     }
     free(ac);
     return 1;
}