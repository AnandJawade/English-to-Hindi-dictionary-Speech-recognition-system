// DigitRecognition_HMM.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <fstream>

using namespace std;

#define N 5
#define M 32
#define T_MAX 150
#define P 12
#define training_utterances 20
#define testing_utterances 10
#define frame_size 320
#define number_of_words 10

int random_word_ind;

ofstream out;
ifstream in;

double M_PI = 3.141592653589793238;   // Global value of PI.
long double tokhuraWeight[12] = {1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};   // Tokhura Weights.
char words[10][10] = {"List", "Home", "Edge", "Path", "Weak", "Sell", "Left", "Stand", "Go", "Up"};
char digits[number_of_words][2] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
char meanings[number_of_words][1000] = {"Soochi", "Ghar", "Kinara", "Rasta", "Kamzor", "Bechna", "Bayen / Chhorna", "Khade Hona", "Jaana", "Upar"};
int prediction;

char *inputFolder = "./Input/214101009/";		// folder containing input files
char *outputFolder = "./Output/";	// folder containing output files
char *rollNumber = "214101009";					// roll number
char *extension = ".txt";
char *underscore = "_";
char *forwardSlash = "/";

long double pi_mean[10][N];
long double a_mean[10][N][N];
long double b_mean[10][N][M];

vector<long double> pi_bar(N);
vector<vector<long double>> a_bar(N, vector<long double>(N));
vector<vector<long double>> b_bar(N, vector<long double>(M));
vector<vector<long double>> alpha_bar(T_MAX, vector<long double>(N));

vector<long double> pi(N);
vector<vector<long double>> a(N, vector<long double>(N));
vector<vector<long double>> b(N, vector<long double>(M));
vector<vector<long double>> obs_seq(N, vector<long double>(T_MAX));
vector<int> curr_obs_seq;
vector<vector<long double>> alpha(T_MAX, vector<long double>(N));
vector<vector<long double>> beta(T_MAX, vector<long double>(N));
vector<vector<long double>> gamma(T_MAX, vector<long double>(N));
vector<vector<long double>> delta(T_MAX, vector<long double>(N));
vector<vector<long double>> sai(T_MAX, vector<long double>(N));
vector<vector<vector<long double>>> xi(T_MAX, vector<vector<long double>>(N, vector<long double>(N)));

//vector<vector<long double>> codebook(M, vector<long double>(P));
long double codebook[M][P];

// function to read PI from file
void read_pi(char *filename)
{
	int i = 0;
	ifstream file;
	file.open(filename);

	for(int i = 0; i < N; ++i)
		file >> pi[i];

	file.close();
}

// function to read a file
void read_file(char *filename, vector<vector<long double>> &arr, int rows, int cols)
{
	int i = 0;
	ifstream file;
	file.open(filename);

	for(int i = 0; i < rows; ++i)
	{
		for(int j = 0; j < cols; ++j)
		{
			file >> arr[i][j];
		}
	}
	file.close();
}

// function to calculate alpha by forward procedure and return P{O|Lambda)}
long double calculate_alpha()
{
	int T = curr_obs_seq.size();
	for(int i = 0; i < N; ++i)
	{
		int ind = curr_obs_seq[0]-1;
		alpha[0][i] = pi[i] * b[i][ind];
	}

	for(int t = 0; t < T-1; ++t)
	{
		for(int j = 0; j < N; ++j)
		{
			long double sum = 0;
			for(int i = 0; i < N; ++i)
			{
				sum += (alpha[t][i] * a[i][j]);
			}
			int ind = curr_obs_seq[t+1]-1;
			alpha[t+1][j] = sum * b[j][ind];
		}
	}

	long double p_alpha = 0.0;
	for(int i = 0; i < N; ++i)
	{
		p_alpha += alpha[T-1][i];
	}
	return p_alpha;
}

// function to calculate alpha by forward procedure for lambda_bar
long double calculate_alpha_bar()
{
	int T = curr_obs_seq.size();
	for(int i = 0; i < N; ++i)
	{
		int ind = curr_obs_seq[0]-1;
		alpha_bar[0][i] = pi_bar[i] * b_bar[i][ind];
	}

	for(int t = 0; t < T-1; ++t)
	{
		for(int j = 0; j < N; ++j)
		{
			long double sum = 0;
			for(int i = 0; i < N; ++i)
			{
				sum += (alpha_bar[t][i] * a_bar[i][j]);
			}
			int ind = curr_obs_seq[t+1]-1;
			alpha_bar[t+1][j] = sum * b_bar[j][ind];
		}
	}

	long double p_alpha = 0.0;
	for(int i = 0; i < N; ++i)
	{
		p_alpha += alpha_bar[T-1][i];
	}
	return p_alpha;
}

// function to calculate beta by backward procedure
long double calculate_beta()
{
	int T = curr_obs_seq.size();
	for(int i = 0; i < N; ++i)
	{
		beta[T-1][i] = 1;
	}

	for(int t = T-2; t >= 0; --t)
	{
		for(int i = 0; i < N; ++i)
		{
			long double sum = 0;
			for(int j = 0; j < N; ++j)
			{
				int ind = curr_obs_seq[t+1]-1;
				sum += (a[i][j] * b[j][ind] * beta[t+1][j]);
			}
			beta[t][i] = sum;
		}
	}
	long double p_beta = 0.0;
	for(int i = 0; i < N; ++i)
	{
		p_beta += beta[0][i];
	}
	return p_beta;
}

// function to implement Viterbi algorithm and return P*
long double viterbi_algorithm()
{
	int T = curr_obs_seq.size();
	for(int i = 0; i < N; ++i)
	{
		int ind = curr_obs_seq[0]-1;
		delta[0][i] = pi[i] * b[i][ind];
		sai[0][i] = 0;
	}

	for(int t = 1; t < T; ++t)
	{
		for(int j = 0; j < N; ++j)
		{
			int max_ind = -1;
			double max = 0;
			for(int i = 0; i < N; ++i)
			{
				if(delta[t-1][i] * a[i][j] > max)
				{
					max = delta[t-1][i] * a[i][j];
					max_ind = i;
				}

				delta[t][j] = max * b[j][curr_obs_seq[t]-1];
				sai[t][j] = max_ind;
			}
		}
	}

	vector<int> q_t_star(T);
	long double p_star = 0.0;
	for(int i = 0; i < N; ++i)
	{
		if(p_star < delta[T-1][i])
		{
			p_star = delta[T-1][i];
			q_t_star[T-1] = i;
		}
	}

	/*
	for(int i = T-2; i >= 0; --i)
	{
		int ind = q_t_star[i+1];
		q_t_star[i] = sai[i+1][ind];
	}

	
	printf("P* = %g\n", p_star);
	printf("State Sequence: ");
	for(int i = 0; i < T; ++i)
	{
		printf("%d ", q_t_star[i]);
	}
	printf("\n\n---------------------------------------------------------------------------------------------\n\n");
	*/

	return p_star;
}

// function to implement Viterbi algorithm and return P* fro lambda_bar
long double viterbi_algorithm_bar()
{
	int T = curr_obs_seq.size();
	for(int i = 0; i < N; ++i)
	{
		int ind = curr_obs_seq[0]-1;
		delta[0][i] = pi_bar[i] * b_bar[i][ind];
		sai[0][i] = 0;
	}

	for(int t = 1; t < T; ++t)
	{
		for(int j = 0; j < N; ++j)
		{
			int max_ind = -1;
			double max = 0;
			for(int i = 0; i < N; ++i)
			{
				if(delta[t-1][i] * a[i][j] > max)
				{
					max = delta[t-1][i] * a[i][j];
					max_ind = i;
				}

				delta[t][j] = max * b_bar[j][curr_obs_seq[t]-1];
				sai[t][j] = max_ind;
			}
		}
	}

	vector<int> q_t_star(T);
	long double p_star = 0.0;
	for(int i = 0; i < N; ++i)
	{
		if(p_star < delta[T-1][i])
		{
			p_star = delta[T-1][i];
			q_t_star[T-1] = i;
		}
	}

	/*
	for(int i = T-2; i >= 0; --i)
	{
		int ind = q_t_star[i+1];
		q_t_star[i] = sai[i+1][ind];
	}

	
	printf("P* = %g\n", p_star);
	printf("State Sequence: ");
	for(int i = 0; i < T; ++i)
	{
		printf("%d ", q_t_star[i]);
	}
	printf("\n\n---------------------------------------------------------------------------------------------\n\n");
	*/

	return p_star;
}

// function to calculate gamma matrix
void calculate_gamma()
{
	int T = curr_obs_seq.size();
	for(int t = 0; t < T; ++t)
	{
		long double sum = 0.0;
		for(int i = 0; i < N; ++i)
		{
			gamma[t][i] = alpha[t][i] * beta[t][i];
			sum += gamma[t][i];
		}

		for(int i = 0; i < N; ++i)
		{
			gamma[t][i] /= sum;
		}
	}
}

// function to calculate xi matrix
void calculate_xi()
{
	int T = curr_obs_seq.size();
	long double denominator = 0.0;

	for(int t = 0; t < T-1; ++t)
	{
		denominator = 0.0;
		for(int i = 0; i < N; ++i)
		{
			for(int j = 0; j < N; ++j)
			{
				int ind = curr_obs_seq[t+1]-1;
				xi[t][i][j] = alpha[t][i] * a[i][j] * b[j][ind] * beta[t+1][j];
				denominator += xi[t][i][j];
			}
		}

		for(int i = 0; i < N; ++i)
		{
			for(int j = 0; j < N; ++j)
			{
				int ind = curr_obs_seq[t+1]-1;
				xi[t][i][j] /= denominator;
			}
		}
	}
}

// function to make a matrix stochastic
void adjust_a_bar()
{
	long double sum = 0;
	int max_ind = 0;
	for(int i = 0; i < N; ++i)
	{
		sum = 0;
		max_ind = 0;
		for(int j = 0; j < N; ++j)
		{
			sum += a_bar[i][j];
			if(a_bar[i][j] > a_bar[i][max_ind])
				max_ind = j;
		}
		if(sum != 1)
		{
			long double diff = 1 - sum;
			a_bar[i][max_ind] += diff;
		}
	}
}

// function to make b matrix stochastic
void adjust_b_bar()
{
	long double default_val = 1e-30;
	for(int i = 0; i < N; ++i)
	{
		for(int j = 0; j < M; ++j)
		{
			if(b_bar[i][j] == 0)
			{
				b_bar[i][j] = default_val;
			}
		}
	}

	long double sum = 0;
	int max_ind = 0;
	for(int i = 0; i < N; ++i)
	{
		sum = 0;
		max_ind = 0;
		for(int j = 0; j < M; ++j)
		{
			sum += b_bar[i][j];
			if(b_bar[i][j] > b_bar[i][max_ind])
				max_ind = j;
		}
		if(sum != 1)
		{
			long double diff = 1 - sum;
			b_bar[i][max_ind] += diff;
		}
	}
}

// implement Baum-Welch algorithm
void re_estimate()
{
	int T = curr_obs_seq.size();
	long double numerator = 0.0;
	long double denominator = 0.0;

	for(int i = 0; i < N; ++i)
	{
		pi_bar[i] = gamma[0][i];
	}

	for(int i = 0; i < N; ++i)
	{
		for(int j = 0; j < N; ++j)
		{
			numerator = 0.0;
			denominator = 0.0;

			for(int t = 0; t < T-1; ++t)
			{
				numerator += xi[t][i][j];
				denominator += gamma[t][i];
			}

			a_bar[i][j] = numerator / denominator;
		}
	}

	for(int j = 0; j < N; ++j)
	{
		for(int k = 0; k < M; ++k)
		{
			numerator = 0.0;
			denominator = 0.0;

			for(int t = 0; t < T; ++t)
			{
				if(curr_obs_seq[t]-1 == k)
				{
					numerator += gamma[t][j];
				}
				denominator += gamma[t][j];
			}
			b_bar[j][k] = numerator / denominator;
		}
	}

	adjust_a_bar();
	adjust_b_bar();
}

// Initialize lambda with Feed Forward/Bakis Model
void init_feed_forward_lambda()
{
	read_pi("./Input/feed_forward_lambda/Pi.txt");
	read_file("./Input/feed_forward_lambda/A.txt", a, N, N);
	read_file("./Input/feed_forward_lambda/B.txt", b, N, M);
}

// function to read digit file
vector<long double> read_digit(char *filepath)
{
	in.open(filepath);
	int x;
	vector<long double> v;
	while(in >> x)
		v.push_back(x);
	in.close();
	return v;
}

// normalize() normalizes the array data to
// the range -5000 to +5000
void normalize(vector<long double> &amplitude)
{
	int n = amplitude.size();
	double max = INT_MIN;
	for(int i = 0; i < n; ++i)
	{
		if(max < amplitude[i])
			max = amplitude[i];
		if(max < -amplitude[i])
			max = -amplitude[i];
	}

	for(int i = 0; i < n; ++i)
	{
		amplitude[i] = (amplitude[i] * 5000.0) / max;
	}
}

// remove_DC_shift() removes the DC shift present in the signal
void remove_DC_shift(vector<long double> &arr)
{
	int size_of_array = arr.size();
	double sum = 0;
	int i = 0;

	// calculate DC shift for first 2000 samples
	for(i = 0; i < 2000; ++i)
	{
		sum += arr[i];
	}
	double DC_shift = sum / 2000;

	// remove DC shift
	for(i = 0; i < size_of_array; ++i)
	{
		arr[i] -= DC_shift;
	}
}

// calculate_ste() calculates the average ste per frame
int calculate_ste(vector<long double> arr, vector<long double> &frame_avg_ste)
{
	int size_of_array = arr.size();
	int frames = size_of_array / frame_size;
	long double ste = 0;
	int i = 0, j = 0;

	int max_ind = 0;

	// calculate average ste for
	// each frame of signal
	for(i = 0; i < frames; ++i)
	{
		ste = 0;
		for(j = frame_size*i; j < frame_size*(i+1); ++j)
		{
			ste += (arr[j] * arr[j]);
			//if(i == 0 && j == 0) continue;
		}
		frame_avg_ste.push_back(ste/frame_size);
		if(frame_avg_ste[max_ind] < frame_avg_ste[i])
			max_ind = i;
	}
	return max_ind;
}

// Function to calculate Ris using autocorrelation formula
void calculate_Ris(vector<long double> &arr, long double R[], int start, int end)
{
	for(int i=0;i<=P;i++)
	{
		long double sum = 0.0;
		for(int j = start; j+i <= end; j++)
		{
			sum += (arr[j] * arr[j+i]);   // Use the formula studied
		}
		sum /= frame_size;
		R[i] = sum;
	}
}

// Function to calculate Durbin's Coefficients, i.e., Ais.
void calculate_Ais(long double R[], long double A[])
{
	double  k[P+1], alpha[P+1][P+1], e[P+1];
	e[0] = R[0];
	k[1] = R[1] / R[0];

	for(int i = 1; i <= P; ++i)
	{
		if(i > 1)
		{
			k[i] = R[i];
			for(int j = 1; j <= i-1; ++j)
			{
				k[i] -= (alpha[i-1][j] * R[i-j]);
			}
			k[i] /= e[i-1];
		}

		alpha[i][i] = k[i];

		for(int j = i-1; j >= 1; --j)
		{
			alpha[i][j] = alpha[i-1][j] - k[i] * alpha[i-1][i-j];
		}

		e[i] = (1 - k[i] * k[i]) * e[i-1];
	}

	for(int i = 0; i <= P; ++i)
	{
		A[i] = alpha[12][i];
	}
}

// Function to calculate Cepstral coefficients, i.e., Cis.
void calculate_Cis(long double R[], long double A[], long double C[])
{
	C[0] = 2 * log10(R[0]);
	for(int i = 1; i <= P; ++i)
	{
		C[i] = A[i];
		for(int j = 1; j <= i-1; ++j)
		{
			C[i] += ((j / (double)i) * C[j] * A[i-j]);
		}
	}
}

// Function to apply raised sine window on an array
void apply_RSW(long double cis[])
{
	for(int i = 1; i <= P; ++i)
	{
		double theta = double(M_PI * i) / P;
		double w = 1 + (P / 2.0) * sin(theta);
		cis[i] *= w;
	}
}

// Function to compute Tokhuras distance
long double get_Tokhuras_distance(long double arr1[], long double arr2[])
{
	long double d = 0, x = 0;
	for(int i = 1; i <= P; ++i)
	{
		x = arr1[i] - arr2[i-1];
		d = d + tokhuraWeight[i-1] * x * x;
	}
	return d;
}

void initialize_HMM_globals()
{
	a.resize(N, vector<long double>(N, 0.0));
	b.resize(N, vector<long double>(M, 0.0));
	pi.resize(N, 0);
	a_bar.resize(N, vector<long double>(N, 0.0));
	b_bar.resize(N, vector<long double>(M, 0.0));
	pi_bar.resize(N, 0);
}

void initialize_HMM_params()
{
	int i, j, k;
	for(i = 0; i < N; ++i) pi[i] = 0;
	for(i = 0; i < N; ++i)
		for(j = 0; j < N; ++j)
			a[i][j] = 0;
	for(i = 0; i < N; ++i)
		for(j = 0; j < M; ++j)
			b[i][j] = 0;

	for(i = 0; i < N; ++i) pi_bar[i] = 0;
	for(i = 0; i < N; ++i)
		for(j = 0; j < N; ++j)
			a_bar[i][j] = 0;
	for(i = 0; i < N; ++i)
		for(j = 0; j < M; ++j)
			b_bar[i][j] = 0;
	
	for(i = 0; i < T_MAX; ++i)
		for(j = 0; j < N; ++j)
			alpha[i][j] = 0;

	for(i = 0; i < T_MAX; ++i)
		for(j = 0; j < N; ++j)
			beta[i][j] = 0;

	for(i = 0; i < T_MAX; ++i)
		for(j = 0; j < N; ++j)
			gamma[i][j] = 0;

	for(i = 0; i < T_MAX; ++i)
		for(j = 0; j < N; ++j)
			delta[i][j] = 0;

	for(i = 0; i < T_MAX; ++i)
		for(j = 0; j < N; ++j)
			sai[i][j] = 0;

	for(i = 0; i < T_MAX; ++i)
		for(j = 0; j < N; ++j)
			for(k = 0; k < N; ++k)
				xi[i][j][k] = 0;
}

// function to implement HMM
void run_HMM(int digit)
{
	init_feed_forward_lambda();
	//printf("%g\n", viterbi_algorithm());
	for(int i = 0; i < 100; ++i)
	{
		calculate_alpha();
		calculate_beta();
		calculate_gamma();
		calculate_xi();

		long double p_star_old = viterbi_algorithm();
		re_estimate();
		long double p_star_new = viterbi_algorithm_bar();
			
		if(p_star_new > p_star_old)
		{
			pi = pi_bar;
			a = a_bar;
			b = b_bar;
		}
	}
	//printf("%g\n", viterbi_algorithm());

	int i, j;
	for(i = 0; i < N; ++i) pi_mean[digit][i] += pi[i];

	for(i = 0; i < N; ++i)
		for(j = 0; j < N; ++j)
			a_mean[digit][i][j] += a[i][j];

	for(i = 0; i < N; ++i)
		for(j = 0; j < M; ++j)
			b_mean[digit][i][j] += b[i][j];
}

// function to process digits for HMM
void process_digit(bool isTraining, char curr_digit[], int utterance_num, char filenum[], int avg_num, int digit)
{
	char in_filepath[1000];
	char out_filepath[1000];

	_snprintf(in_filepath, sizeof(in_filepath), "%s%s%s%s%s%s%s", inputFolder, rollNumber, underscore, curr_digit, underscore, filenum, extension);

	_snprintf(out_filepath, sizeof(out_filepath), "%s%s%s%s%s%s%s", "./Input/Observation_Sequences/", rollNumber, underscore, curr_digit, underscore, filenum, extension);

	vector<long double> amplitude = read_digit(in_filepath);

	//printf("Reading %s\t%d\n", in_filepath, amplitude.size());

	remove_DC_shift(amplitude);	// remove dc shilft
	normalize(amplitude);		// normalize data

	if(isTraining)
	{
		printf("\nStarting training for digit %d observation sequence %d\n", digit, utterance_num);
		//printf("\n%d : %d", digit, utterance_num);
	}
	else
	{
		printf("\nStarting testing for digit %d observation sequence %d\n", digit, utterance_num);
	}

	vector<long double> frame_avg_ste;
	int peak_ind = calculate_ste(amplitude, frame_avg_ste) * frame_size;

	int start_ind = peak_ind * 0.3;
	int end_ind = peak_ind + (amplitude.size() - peak_ind) * 0.7;

	long double ris[P+1], ais[P+1], cis[P+1];

	out.open(out_filepath);

	curr_obs_seq.clear();
	for(int i = start_ind; i < end_ind; i += frame_size)
	{
		memset(ris, 0.0, sizeof(ris));
		memset(ais, 0.0, sizeof(ais));
		memset(cis, 0.0, sizeof(cis));
				
		// calculated ris, ais and cis
		calculate_Ris(amplitude, ris, i, i + frame_size - 1);
		calculate_Ais(ris, ais);
		calculate_Cis(ris, ais, cis);

		apply_RSW(cis);	// apply raised sine window on cis

		long double min_dist = DBL_MAX;
		int min_ind = 0;

		for(int i = 0; i < M; ++i)
		{
			long double d = get_Tokhuras_distance(cis, codebook[i]);

			if(d < min_dist)
			{
				min_dist = d;
				min_ind = i;
			}
		}
		++min_ind;
		out << min_ind << " ";
		//printf("%d ", min_ind);
		curr_obs_seq.push_back(min_ind);
	}
	out << "\n";
	//printf("\n");
	out.close();

	if(isTraining)
	{
		initialize_HMM_params();
		run_HMM(digit);
	}
	else
	{
		//initialize_HMM_params();
		
		long double p_alpha_max = -1;
		prediction = -1;
		int i, j, d;
		for(d = 0; d < 10; ++d)
		{
			for(i = 0; i < N; ++i) pi[i] = pi_mean[d][i];

			for(i = 0; i < N; ++i)
				for(j = 0; j < N; ++j)
					a[i][j] = a_mean[d][i][j];

			for(i = 0; i < N; ++i)
				for(j = 0; j < M; ++j)
					b[i][j] = b_mean[d][i][j];

			long double p_alpha = calculate_alpha();

			printf("Score for %d = %g\n", d, p_alpha);

			if(p_alpha > p_alpha_max)
			{
				p_alpha_max = p_alpha;
				prediction = d;
			}
		}
	}
}

void process_new_word(char *in_filepath, int digit)
{
	vector<long double> amplitude = read_digit(in_filepath);

	remove_DC_shift(amplitude);	// remove dc shilft
	normalize(amplitude);		// normalize data

	vector<long double> frame_avg_ste;
	int peak_ind = calculate_ste(amplitude, frame_avg_ste) * frame_size;

	int start_ind = peak_ind * 0.3;
	int end_ind = peak_ind + (amplitude.size() - peak_ind) * 0.7;

	long double ris[P+1], ais[P+1], cis[P+1];

	curr_obs_seq.clear();
	for(int i = start_ind; i < end_ind; i += frame_size)
	{
		memset(ris, 0.0, sizeof(ris));
		memset(ais, 0.0, sizeof(ais));
		memset(cis, 0.0, sizeof(cis));
				
		// calculated ris, ais and cis
		calculate_Ris(amplitude, ris, i, i + frame_size - 1);
		calculate_Ais(ris, ais);
		calculate_Cis(ris, ais, cis);

		apply_RSW(cis);	// apply raised sine window on cis

		long double min_dist = DBL_MAX;
		int min_ind = 0;

		for(int i = 0; i < M; ++i)
		{
			long double d = get_Tokhuras_distance(cis, codebook[i]);

			if(d < min_dist)
			{
				min_dist = d;
				min_ind = i;
			}
		}
		++min_ind;
		curr_obs_seq.push_back(min_ind);
	}


	initialize_HMM_params();
	run_HMM(digit);
}

// function to write a model to file
void write_model_to_file(char *filepath, int digit)
{
	out.open(filepath);
	//out << "PI Matrix:\n";
	int i, j;
	for(i = 0; i < N; ++i) out << pi_mean[digit][i] << "\t";
	out << "\n";

	//out << "A Matrix:\n";
	for(i = 0; i < N; ++i)
	{
		for(j = 0; j < N; ++j)
		{
			out << a_mean[digit][i][j] << "\t";
		}
		out << "\n";
	}
	//out << "\n\n";

	//out << "B Matrix:\n";
	for(i = 0; i < N; ++i)
	{
		for(j = 0; j < M; ++j)
		{
			out << b_mean[digit][i][j] << "\t";
		}
		out << "\n";
	}
	out.close();
}

// function to read a model from file
void read_model(char *filepath, int digit)
{
	in.open(filepath);
	int i, j;
	for(i = 0; i < N; ++i) in >> pi_mean[digit][i];

	for(i = 0; i < N; ++i)
	{
		for(j = 0; j < N; ++j)
		{
			in >> a_mean[digit][i][j];
		}
	}

	for(i = 0; i < N; ++i)
	{
		for(j = 0; j < M; ++j)
		{
			in >> b_mean[digit][i][j];
		}
	}
	in.close();
}

// function to process live data for HMM
void process_live_data(char *in_filepath)
{
	vector<long double> amplitude = read_digit(in_filepath);

	//printf("Reading %s\t%d\n", in_filepath, amplitude.size());

	remove_DC_shift(amplitude);	// remove dc shilft
	normalize(amplitude);		// normalize data

	printf("\nStarting testing for input data\n");

	vector<long double> frame_avg_ste;
	int peak_ind = calculate_ste(amplitude, frame_avg_ste) * frame_size;

	int start_ind = peak_ind * 0.3;
	int end_ind = peak_ind + (amplitude.size() - peak_ind) * 0.7;

	long double ris[P+1], ais[P+1], cis[P+1];

	//out.open(out_filepath);

	curr_obs_seq.clear();
	for(int i = start_ind; i < end_ind; i += frame_size)
	{
		memset(ris, 0.0, sizeof(ris));
		memset(ais, 0.0, sizeof(ais));
		memset(cis, 0.0, sizeof(cis));
				
		// calculated ris, ais and cis
		calculate_Ris(amplitude, ris, i, i + frame_size - 1);
		calculate_Ais(ris, ais);
		calculate_Cis(ris, ais, cis);

		apply_RSW(cis);	// apply raised sine window on cis

		long double min_dist = DBL_MAX;
		int min_ind = 0;

		for(int i = 0; i < M; ++i)
		{
			long double d = get_Tokhuras_distance(cis, codebook[i]);

			if(d < min_dist)
			{
				min_dist = d;
				min_ind = i;
			}
		}
		++min_ind;
		//out << min_ind << " ";
		//printf("%d ", min_ind);
		curr_obs_seq.push_back(min_ind);
	}
	//out << "\n";
	//printf("\n");
	//out.close();

	//initialize_HMM_params();
		
	long double p_alpha_max = -1;
	prediction = -1;
	int i, j, d;
	for(d = 0; d < 10; ++d)
	{
		for(i = 0; i < N; ++i) pi[i] = pi_mean[d][i];

		for(i = 0; i < N; ++i)
			for(j = 0; j < N; ++j)
				a[i][j] = a_mean[d][i][j];

		for(i = 0; i < N; ++i)
			for(j = 0; j < M; ++j)
				b[i][j] = b_mean[d][i][j];

		long double p_alpha = calculate_alpha();

		printf("Score for %d = %g\n", d, p_alpha);

		if(p_alpha > p_alpha_max)
		{
			p_alpha_max = p_alpha;
			prediction = d;
		}
	}
}

/*
int _tmain(int argc, _TCHAR* argv[])
{
	in.open("./Input/Codebook.txt");
	for(int i = 0; i < M; ++i)
	{
		for(int j = 0; j < P; ++j)
		{
			in >> codebook[i][j];
		}
	}
	in.close();

	bool isTraining = true;
	int i, j;

	int choice = 0;

	while(choice != 4)
	{
		printf("\t\tMenu:\n1. Train the model.\n2. Test the model on the offline testing data.\n3. Test the model on live data.\n4. Exit\n");
		scanf("%d", &choice);
		if(choice == 1)
		{
			if(isTraining)
			{
				for(int digit = 0; digit < 10; ++digit)
				{
					for(i = 0; i < N; ++i) pi_mean[digit][i] = 0;

					for(i = 0; i < N; ++i)
						for(j = 0; j < N; ++j)
							a_mean[digit][i][j] = 0;

					for(i = 0; i < N; ++i)
						for(j = 0; j < M; ++j)
							b_mean[digit][i][j] = 0;

					for(int avg_num = 0; avg_num < 1; ++avg_num)
					{
						initialize_HMM_globals();
						//printf("\nAverage number : %d", avg_num+1);
						for(int utterance_num = 1; utterance_num <= training_utterances; utterance_num++)
						{
							char filenumber[3] = "";
							sprintf(filenumber,"%d",utterance_num);
							process_digit(isTraining, digits[digit], utterance_num, filenumber, avg_num, digit);
						}
					}
					for(i = 0; i < N; ++i) pi_mean[digit][i] /= training_utterances;

					for(i = 0; i < N; ++i)
						for(j = 0; j < N; ++j)
							a_mean[digit][i][j] /= training_utterances;

					for(i = 0; i < N; ++i)
						for(j = 0; j < M; ++j)
							b_mean[digit][i][j] /= training_utterances;

					char model_filepath[1000] = "";
					_snprintf(model_filepath, sizeof(model_filepath), "%s%s%s", "./Output/Models/Model_", digits[digit], extension);
					printf("Writing into File %s\n", model_filepath);
					write_model_to_file(model_filepath, digit);
				}
			}
		}


//======================================================TESTING ON OFFLINE DATA========================================================

		else if(choice == 2)
		{
			double correct = 0, total = 0;

			for(int digit = 0; digit < 10; ++digit)
			{
				char model_filepath[1000] = "";
				_snprintf(model_filepath, sizeof(model_filepath), "%s%s%s", "./Output/Models/Model_", digits[digit], extension);
				printf("Reading File %s\n", model_filepath);
				read_model(model_filepath, digit);
			}

			vector<int> map(10);

			for(int digit = 0; digit < 10; ++digit)
			{
				for(int utterance_num = 1; utterance_num <= testing_utterances; utterance_num++)
				{
					char filenumber[3] = "";
					sprintf(filenumber,"%d",utterance_num + training_utterances);
					process_digit(false, digits[digit], utterance_num, filenumber, 1, digit);
					total += 1;
					if(prediction == digit)
					{
						correct += 1;
						map[digit]++;
					}
					printf("Prediction = %d\n", prediction);
				}
			}

			printf("\nDigit\tAccuracy\n");
			printf("=================\n");

			for(int i = 0; i < 10; ++i) printf("%d\t%d%%\n", i, map[i]*10);

			printf("\nOverall Accuracy = %g%%\n", (correct*100) / total);
		}

//======================================================TESTING ON LIVE DATA========================================================

		else if(choice == 3)
		{
			for(int digit = 0; digit < 10; ++digit)
			{
				char model_filepath[1000] = "";
				_snprintf(model_filepath, sizeof(model_filepath), "%s%s%s", "./Output/Models/Model_", digits[digit], extension);
				printf("Reading File %s\n", model_filepath);
				read_model(model_filepath, digit);
			}

			system("Recording_Module.exe 2 input_file.wav input_file.txt");
			process_live_data("input_file.txt");
			printf("Prediction = %d\n\n", prediction);
		}

//==================================================================================================================================

		else if(choice == 4) printf("Bye Bye!\nWill miss you!\n");
		else printf("Invalid Choice!\nTry Again!\n\n");

		printf("\n======================================================================================\n\n");
	}
//==================================================================================================================================

	system("pause");
	return 0;
}
*/
