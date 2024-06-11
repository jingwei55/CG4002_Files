#include "weights_biases_2.h"

void hidden_1(const fixed_point_t (&input_layer)[FEATURES*SENSOR_NUM], fixed_point_t &val, const int idx) {
#pragma HLS INLINE
    fixed_point_t res = hidden_biases[idx];
    for (int i = 0; i < FEATURES*SENSOR_NUM; i ++) {
//#pragma HLS PIPELINE
#pragma HLS UNROLL
//#pragma HLS LOOP_FLATTEN
        res += (input_layer[i] * hidden_weights[idx][i]);
    }
    val = (res > fixed_point_t(0)) ? res : fixed_point_t(0);
}

void output(const fixed_point_t (&hidden_layer)[UNITS_PER_LAYER], fixed_point_t &val, const int idx) {
#pragma HLS INLINE // Uncomment for single DMA
	fixed_point_t res = output_biases[idx];
    for (int i = 0; i < UNITS_PER_LAYER; i ++) {
//#pragma HLS PIPELINE
#pragma HLS UNROLL
//#pragma HLS LOOP_FLATTEN
        res += (hidden_layer[i] * output_weights[idx][i]);
    }
	val = res;
}

void mlp(stream_io& s_axis, stream_io& m_axis) {
#pragma HLS INTERFACE axis port=s_axis
#pragma HLS INTERFACE axis port=m_axis
#pragma HLS INTERFACE s_axilite port=return

	fixed_point_t input[FEATURES*SENSOR_NUM];
	fixed_point_t hidden_layer_1[UNITS_PER_LAYER];
	fixed_point_t predict[NUM_CLASSES] = {0};

	AXIS_IO data_in;
	AXIS_IO data_out;

	for (int i = 0; i < FEATURES*SENSOR_NUM; i++) {
//#pragma HLS PIPELINE
#pragma HLS UNROLL
		s_axis.read(data_in);
		input[i] = data_in.data;
	}

	compute_hidden:for (int i = 0; i < UNITS_PER_LAYER; i++) {
		hidden_1(input, hidden_layer_1[i], i);
	}

	compute_output:for (int i = 0; i < NUM_CLASSES; i++) {
		output(hidden_layer_1, predict[i], i);
	}

	int pred = 0;
	fixed_point_t max_val = predict[0];

	for (int i = 0; i < NUM_CLASSES; i++) {
#pragma HLS UNROLL
//#pragma HLS PIPELINE
		if (predict[i] > max_val) {
			max_val = predict[i];
			pred = i;
		}
	}

	// Check if max prediction is lower than threshold or not
//	if (max_val < THRESHOLD) {
//		pred = 8; //NULL class
//	}

	data_out.data = pred;
	data_out.keep = 1;
	data_out.strb =1;
	data_out.last = 1;
	m_axis.write(data_out);
}
