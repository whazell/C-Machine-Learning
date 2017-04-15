#include "builder.h"

error_t build_layer (layer* l, layer_type lt, int bias, int nodes, act_f af, act_prime_f ap) {
	
	if (l == NULL)
		return E_NULL_ARG;

	/* Only set the variables we need */
	l->ltype = lt;
	l->output_nodes = lt;
	l->using_bias = bias;
	l->af = af;
	l->ap = ap;

	return E_SUCCESS;
}


error_t add_layer (net* nn, layer* l) {
	if (nn == NULL || l == NULL)
		return E_NULL_ARG;

	/* Check to make sure we aren't adding extra input/output layer */
	layer_type added_type = l->ltype;
	for (int i = 0; i < nn->layer_count; i++) {
		if (nn->layers[i]->ltype == added_type && added_type == input)
			return E_TOO_MANY_INPUT_LAYERS;
		
		if (nn->layers[i]->ltype == added_type && added_type == output)
			return E_TOO_MANY_OUTPUT_LAYERS;
	}

	/* Add room for 1 more layer, and put given argument in */
	nn->layers = realloc(nn->layers, sizeof(layer*) * ++nn->layer_count);
	nn->layers[nn->layer_count - 1] = l;
	return E_SUCCESS;
} 


data_set* data_set_from_csv(FILE* fh) {
	int lines, inputs_per_line, outputs_per_line;
	char *token, *inputs, *outputs;

	data_set* data = malloc(sizeof(data_set));
	data->data = malloc(sizeof(data_pair*));
	char* line_buffer = malloc(sizeof(char) * MAXLINELEN);
	lines = inputs_per_line = outputs_per_line = 0;

	const char delim[2] = ",";
	const char split[2] = ";";

	while( fgets(line_buffer, MAXLINELEN, fh) != NULL) {
		int item_count = 0;

		/* Allocate more space */
		data->count = lines++;
		data->data = realloc(data->data, sizeof(data_pair*) * lines);
		data->data[data->count] = malloc(sizeof(data_pair));
		data->data[data->count]->input = malloc(sizeof(matrix_t));
		data->data[data->count]->expected_output = malloc(sizeof(matrix_t));
		
		/* Get first and second half of line */
		double* value_buffer = malloc(sizeof(double));
		inputs = strtok(line_buffer, split);
		outputs = strtok(NULL, split);

		token = strtok(inputs, delim);
		while (token != NULL) {
			item_count++;
			value_buffer = realloc(value_buffer, sizeof(double) * item_count);
			value_buffer[item_count-1] = strtod(token, NULL);
			token = strtok(NULL, delim);
		}
		
		/* If this is first iteration, save the values */
		if (lines == 1) 
			inputs_per_line = item_count;

		if (inputs_per_line != item_count) {
			fprintf(stderr, "Inconsistent amount of inputs on line %d...Dying\n", lines);
			exit(1);
		}

		/* Put into data_pair* */
		init_matrix(data->data[data->count]->input, item_count, 1);
		for (int i = 0; i < item_count; i++)
			data->data[data->count]->input->matrix[i][0] = value_buffer[i];
		
		/* Reset for output */
		item_count = 0;
		double* vals = malloc(sizeof(double));
		
		token = strtok(outputs, delim);
		while (token != NULL) {
			item_count++;
			vals = realloc(vals, sizeof(double) * item_count);
			vals[item_count-1] = strtod(token, NULL);
			token = strtok(NULL, delim);
		}

		if (lines == 1)
			outputs_per_line = item_count;
		
		if (outputs_per_line != item_count) {
			fprintf(stderr, "Inconsistent amount of outputs on line %d...Dying\n", lines);
			exit(1);
		}

		init_matrix(data->data[data->count]->expected_output, item_count, 1);
		for (int i = 0; i < item_count; i++) 
			data->data[data->count]->expected_output->matrix[i][0] = vals[i];
		
		free(value_buffer);
		free(vals);
	}
	data->count++;
	free(line_buffer);	
	return data;
}

/*
int load_net (net* n, FILE* fh) {
	return parse_net(n, fh);	
}

static int parse_net (net* n, FILE* fh) {

}

static int parse_layer (layer** l, FILE* fh) {

}

static int parse_matrix (matrix_t** m, FILE* fh) {

}
*/

error_t save_net (net* n, FILE* fh) {
	if (n == NULL || fh == NULL) return E_NULL_ARG;

	fprintf(fh, "BEGIN NET\n");
	fprintf(fh, "layers=%d\n", n->layer_count);

	for (int i = 0; i < n->layer_count; i++) {
		fprintf(fh, "%d,", n->topology[i]);
	}
	fprintf(fh, "\n");

	for (int i = 0; i < n->layer_count; i++) {
		layer* clayer = n->layers[i];
		fprintf(fh, "BEGIN LAYER\n");
		fprintf(fh, "type=");
		
		if (clayer->ltype == input) fprintf(fh, "input\n");
		if (clayer->ltype == hidden) fprintf(fh, "hidden\n");
		if (clayer->ltype == output) fprintf(fh, "output\n");
		
		fprintf(fh, "input-nodes=%d\n", clayer->input_nodes);
		fprintf(fh, "output-nodes=%d\n", clayer->output_nodes);
		fprintf(fh, "bias=%lf\n", clayer->bias);
		
		fprintf(fh, "BEGIN MATRIX\n");
		
		if (clayer->weights != NULL) {
			matrix_t* buff = clayer->weights;

			for (int n = 0; n < buff->rows; n++) {
				for (int j = 0; j < buff->columns; j++) {
					fprintf(fh, "%lf,", buff->matrix[n][j]);	
				}
				fprintf(fh, "\n");
			}
		}
		fprintf(fh, "END MATRIX\nEND LAYER\n");
	}
	fprintf(fh, "END NET");
	return E_SUCCESS;
}

int free_data_set (data_set* data) {
	for (int i = 0; i < data->count; i++) {
		free_matrix(data->data[i]->input);
		free_matrix(data->data[i]->expected_output);
		free(data->data[i]);
	}
	free(data->data);
	free(data);
	return E_SUCCESS;
}
