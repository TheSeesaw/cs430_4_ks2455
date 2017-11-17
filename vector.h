
// struct for Vector3ds and points
typedef struct Vector3d {
  double x, y, z;
} Vector3d;

Vector3d* Vector3d_add(Vector3d* output, Vector3d* operand_a, Vector3d* operand_b);

Vector3d* Vector3d_sub(Vector3d* output, Vector3d* operand_a, Vector3d* operand_b);

double Vector3d_dot_prod(Vector3d* operand_a, Vector3d* operand_b);

Vector3d* Vector3d_cross_prod(Vector3d* output, Vector3d* operand_a, Vector3d* operand_b);
