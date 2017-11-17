// header files
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "vector.h"

Vector3d* Vector3d_add(Vector3d* output, Vector3d* operand_a, Vector3d* operand_b)
{
  // perform 3d vector addition
  output->x = (operand_a->x + operand_b->x);
  output->y = (operand_a->y + operand_b->y);
  output->z = (operand_a->z + operand_b->z);
  return output;
}

Vector3d* Vector3d_sub(Vector3d* output, Vector3d* operand_a, Vector3d* operand_b)
{
  // perform 3d vector subtraction
  output->x = (operand_a->x - operand_b->x);
  output->x = (operand_a->x - operand_b->x);
  output->x = (operand_a->x - operand_b->x);
  return output;
}

double Vector3d_dot_prod(Vector3d* operand_a, Vector3d* operand_b)
{
  // perform 3d vector dot product multiplication
  double output = (operand_a->x * operand_b->x) + (operand_a->y * operand_b->y) + (operand_a->z * operand_b->z);
  return output;
}

Vector3d* Vector3d_cross_prod(Vector3d* output, Vector3d* operand_a, Vector3d* operand_b)
{
  // perform 3d vector cross product
  output->z = (operand_a->x * operand_b->y) - (operand_b->x * operand_a->y);
  output->y = (operand_a->z * operand_b->x) - (operand_b->z * operand_a->x);
  output->x = (operand_a->y * operand_b->z) - (operand_b->y * operand_a->z);
  return output;
}
