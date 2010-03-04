#ifndef reconstruct_h
#define reconstruct_h

void average_nonstreaming(int start, int end);
void fill_in_mem(OctNode n, int d = 0);
void execPipeline(vector<Process> &pipeline);
void count_and_coeffs_haar_second(int from, int to);
void count_and_coeffs_haar_first(int from, int to);
void calc_coeffs_haar_first(int from, int to);
void calc_coeffs_haar_second(int from, int to);
void calc_coeffs_daub4_first(int from, int to);
void calc_coeffs_daub4_second(int from, int to);
void count_points_first(int from, int to);
void count_points_second(int from, int to);

#endif