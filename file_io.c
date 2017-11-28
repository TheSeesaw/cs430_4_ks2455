#include "file_io.h"

#define MAX_OBJECTS 128

// Function traverses white space until a character that is not a space, newline,
// or a comment line
// Params - file_to_read: a pointer to a file handle to be read
// Returns - NOTHING
void traverse_whitespace_and_comments(FILE* file_to_read) {
	// Initialize with the first character in the file
	int current_char = fgetc(file_to_read);
	// Traverse the file, ignoring whitespace and newlines
	while (current_char == ' ' || current_char == '\n' || current_char == '#') {
		// If hash, skip the entire line
		if (current_char == '#') {
			current_char = fgetc(file_to_read); // Initialize loop with next char
			while (current_char != '\n') { // While not newline
				current_char = fgetc(file_to_read);
				// current_char now holds newline, break out of loop
			}
		}
		current_char = fgetc(file_to_read); // Get the next char
	}
	// Unget the last non-whitespace character
	ungetc(current_char, file_to_read);
}

// read single camera object from a csv file
void read_camera_data(FILE* file_to_read, Shape *camera) {
	// data should be in this format:
	// camera, width: 0.5, height: 0.5
	char *wastebasket = malloc(10*sizeof(char)); // initialize a junk data variable
  fscanf(file_to_read, "%s", wastebasket); // read past the camera identifier
	camera->type = Camera;
	fscanf(file_to_read, "%s", wastebasket); // read past the width identifier
	if (strcmp(wastebasket, "width:") != 0) { // use it for error checking
		fprintf(stderr, "Error: Invalid camera width format.\n");
    return;
	}
	fscanf(file_to_read, "%lf", &camera->width); // capture the width value
	fscanf(file_to_read, "%s", wastebasket); // read past the trailing comma
	fscanf(file_to_read, "%s", wastebasket); //read past the height identifier
	if (strcmp(wastebasket, "height:") != 0) { // use it for error checking
		fprintf(stderr, "Error: Invalid camera height format.\n");
    return;
	}
	fscanf(file_to_read, "%lf", &camera->height); // capture the height value
	/*
	printf("%d\n", camera->type);
	printf("%lf\n", camera->width);
	printf("%lf\n", camera->height);
	*/
	free(wastebasket);
}

void read_sphere_data(FILE* file_to_read, Shape* output_list, int obj_index) {
	// data should be in this format:
	// sphere, radius: 2.0, diffuse_color: [1, 0, 0], specular_color: [1,1,1], position: [0, 1, -5]
	char *wastebasket = malloc(20*sizeof(char)); // initialize a junk data variable
	output_list[obj_index].type = Sphere;
	fscanf(file_to_read, "%s", wastebasket); // read past radius identifier
	fscanf(file_to_read, "%lf", &output_list[obj_index].radius); // read in radius value
	fgetc(file_to_read); // skip over comma
	traverse_whitespace_and_comments(file_to_read); // skip over spaces
	fscanf(file_to_read, "%s", wastebasket); // read past diffuse color identifier
	traverse_whitespace_and_comments(file_to_read); // skip over spaces
	fgetc(file_to_read); // skip over left bracket
	fscanf(file_to_read, "%lf", &output_list[obj_index].d_col_r); // read in red diffuse value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].d_col_g); // read in green diffuse value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].d_col_b); // read in blue diffuse value
	fgetc(file_to_read); // skip over right bracket
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%s", wastebasket); // read past specular color identifier
	traverse_whitespace_and_comments(file_to_read); // skip over spaces
	fgetc(file_to_read); // skip over left bracket
	fscanf(file_to_read, "%lf", &output_list[obj_index].s_col_r); // read in red specular value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].s_col_g); // read in green specular value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].s_col_b); // read in blue specular value
	fgetc(file_to_read); // skip over right bracket
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%s", wastebasket); // read past position color identifier
	traverse_whitespace_and_comments(file_to_read); // skip over spaces
	fgetc(file_to_read); // skip over left bracket
	fscanf(file_to_read, "%lf", &output_list[obj_index].pos_x); // read in x position
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].pos_y); // read in y position
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].pos_z); // read in z position
	fgetc(file_to_read); // skip over right bracket
	printf("sphere\n");
	free(wastebasket); // free the junk data pointer
}

void read_plane_data(FILE* file_to_read, Shape* output_list, int obj_index)
{
	// data should be in this format
	// plane, normal: [0, 1, 0], diffuse_color: [0, 1, 0], position: [0, 2, 5]
	char *wastebasket = malloc(20*sizeof(char)); // initialize a junk data variable
	output_list[obj_index].type = Plane;
	fscanf(file_to_read, "%s", wastebasket); // read past normal identifier
	traverse_whitespace_and_comments(file_to_read); // skip over spaces
	fgetc(file_to_read); // skip over left bracket
	fscanf(file_to_read, "%lf", &output_list[obj_index].norm_x); // read in x normal
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].norm_y); // read in y normal
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].norm_z); // read in z normal
	fgetc(file_to_read); // skip over right bracket
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%s", wastebasket); // read past diffuse color identifier
	traverse_whitespace_and_comments(file_to_read); // skip over spaces
	fgetc(file_to_read); // skip over left bracket
	fscanf(file_to_read, "%lf", &output_list[obj_index].d_col_r); // read in red diffuse value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].d_col_g); // read in green diffuse value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].d_col_b); // read in blue diffuse value
	fgetc(file_to_read); // skip over right bracket
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%s", wastebasket); // read past position identifier
	traverse_whitespace_and_comments(file_to_read); // skip over spaces
	fgetc(file_to_read); // skip over left bracket
	fscanf(file_to_read, "%lf", &output_list[obj_index].pos_x); // read in x position
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].pos_y); // read in y position
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &output_list[obj_index].pos_z); // read in z position
	fgetc(file_to_read); // skip over right bracket
	printf("plane\n");
	//printf("red color: %lf, green color: %lf, blue color: %lf\n", output_list[obj_index].d_col_r, output_list[obj_index].d_col_g, output_list[obj_index].d_col_b);
	free(wastebasket); // free the junk data pointer
}

void read_light_data(FILE* file_to_read, Light* light_list, int light_index)
{
	// data should be in the following format
	// light, color: [2, 2, 2], theta: 0, radial-a2: 0.125, radial-a1: 0.125, radial-a0: 0.125, position: [1, 3, -1]
	char *wastebasket = malloc(20*sizeof(char)); // initialize a junk data variable
	fscanf(file_to_read, "%s", wastebasket); // read past color identifier
	traverse_whitespace_and_comments(file_to_read); // skip over spaces
	fgetc(file_to_read); // skip over left bracket
	fscanf(file_to_read, "%lf", &light_list[light_index].col_r); // read in red value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &light_list[light_index].col_g); // read in green value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &light_list[light_index].col_b); // read in blue value
	fgetc(file_to_read); // skip over right bracket
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%s", wastebasket); // read past theta identifier
	fscanf(file_to_read, "%lf", &light_list[light_index].theta); // read in theta value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%s", wastebasket); // read past radial-a2 identifier
	fscanf(file_to_read, "%lf", &light_list[light_index].radial_a2); // read in radial-a2 value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%s", wastebasket); // read past radial-a1 identifier
	fscanf(file_to_read, "%lf", &light_list[light_index].radial_a1); // read in radial-a1 value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%s", wastebasket); // read past radial-a0 identifier
	fscanf(file_to_read, "%lf", &light_list[light_index].radial_a0); // read in radial-a0 value
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%s", wastebasket); // read past position identifier
	traverse_whitespace_and_comments(file_to_read); // skip over spaces
	fgetc(file_to_read); // skip over left bracket
	fscanf(file_to_read, "%lf", &light_list[light_index].pos_x); // read in x position
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &light_list[light_index].pos_y); // read in y position
	fgetc(file_to_read); // skip over comma
	fscanf(file_to_read, "%lf", &light_list[light_index].pos_z); // read in z position
	fgetc(file_to_read); // skip over right bracket
	printf("light\n");
	//printf("r color: %lf, g color: %lf, b color %lf\n", light_list[light_index].col_r, light_list[light_index].col_g, light_list[light_index].col_b);
	//printf("theta: %lf\n", light_list[light_index].theta);
	//printf("radial-a2: %lf, radial-a1: %lf, radial-a0: %lf\n", light_list[light_index].radial_a2, light_list[light_index].radial_a1, light_list[light_index].radial_a0);
	//printf("x pos %lf, y pos %lf, z pos %lf\n", light_list[light_index].pos_x, light_list[light_index].pos_y, light_list[light_index].pos_z);
	free(wastebasket);
}

/* reads the camera object information, then calls read functions for each Object
   in the input file
*/
int read_object_file_director(char *in_file_name, Shape *camera, Shape *output_list, Light *light_list, int *obj_count_array)
{
  // declare variables
  char type_string[15];
  int object_index = 0;
	int light_index = 0;
  // open the file to read binary data
  FILE* file_handle_in = fopen(in_file_name, "rb");
  // read in the camera
  read_camera_data(file_handle_in, camera);
  // read objects
  while (fscanf(file_handle_in, "%s", type_string) == 1) // next object type stored in type_string
  {
    traverse_whitespace_and_comments(file_handle_in); // reset to start of next line
    type_string[strlen(type_string) - 1] = 0; // trim trailing comma
    // switch on type, pass to read function
    if (strcmp(type_string, "sphere") == 0)
    {
      read_sphere_data(file_handle_in, output_list, object_index);
			object_index++;
    }
    else if (strcmp(type_string, "plane") == 0)
    {
      read_plane_data(file_handle_in, output_list, object_index);
			object_index++;
    }
		else if (strcmp(type_string, "light") == 0)
		{
			read_light_data(file_handle_in, light_list, light_index);
			light_index++;
		}
    else
    {
      fprintf(stderr, "Error: invalid shape type.\n");
      return -1;
    }
  }
  fclose(file_handle_in);
	obj_count_array[0] = object_index;
	obj_count_array[1] = light_index;
  return 0;
}

void shade(double *r_strg, double *g_strg, double *b_strg)
{
	return;
}

// writes pixel data to a P6 .ppm file in binary
int write_pixels_to_p6(Pixel *buffer, char *out_file_name, int res_width, int res_height)
{
	// open a new file for writing binary
	FILE* file_handle_out = fopen(out_file_name, "wb");
	// initialize writing variables
	unsigned char space = ' ';
	unsigned char newline = '\n';
	unsigned char hash = '#';
	unsigned char capital_p = 'P';
	// write header data
	fwrite(&capital_p, sizeof(char), 1, file_handle_out); // write P
	fprintf(file_handle_out, "%d", 6); // write magic number
	fwrite(&newline, sizeof(char), 1, file_handle_out);
	fwrite(&hash, sizeof(char), 1, file_handle_out); // write a hash
	fwrite(&newline, sizeof(char), 1, file_handle_out);
	fprintf(file_handle_out, "%d", res_width); // write width
	fwrite(&space, sizeof(char), 1, file_handle_out);
	fprintf(file_handle_out, "%d", res_height); // write height
	fwrite(&newline, sizeof(char), 1, file_handle_out);
	fprintf(file_handle_out, "%d", 255); // write maxval
	fwrite(&newline, sizeof(char), 1, file_handle_out);
	// write pixel data from buffer
	for (int i = 0; i < res_width*res_height; i += 1) {
		fwrite(&buffer[i].r, sizeof(char), 1, file_handle_out); // write red value
		fwrite(&buffer[i].g, sizeof(char), 1, file_handle_out); // write green value
		fwrite(&buffer[i].b, sizeof(char), 1, file_handle_out); // write blue value
	}
	// close file
	fclose(file_handle_out);
  return 0;
}
