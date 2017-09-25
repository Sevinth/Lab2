
#include <stdlib.h>
#include <functional>
#include <omp.h>
#include <iostream>
#include <chrono>

typedef double(*functionCall)(double arg);

double piCalc(double);
void simspon(functionCall, double*, double, int, double);
void simspon_par(functionCall, double*, double, int, double);
double relative_error(double, double);

auto timing = [](auto&& F, auto&& ... params) {
	auto start = std::chrono::steady_clock::now();
	F(std::forward<decltype(params)>(params)...);
	return std::chrono::duration_cast<std::chrono::nanoseconds>(
		std::chrono::steady_clock::now() - start).count();
};



int main() {
	static double pi = 4.0*atan(1.0);
	
	int n = 150;
	double a = 0.0;
	double b = 1.0;

	double dx = (b-a) / static_cast<double>(n);

	double *x = NULL;
	x = new double[n];


	auto par_start = std::chrono::steady_clock::now();
	simspon_par(piCalc, x, dx, n, a);
	auto par_end = std::chrono::steady_clock::now() - par_start;
	auto ns_par = std::chrono::duration_cast<std::chrono::microseconds>(par_end).count();

	std::cout << "OMP Version took " << ns_par << " microseconds." << std::endl;

	auto ser_start = std::chrono::steady_clock::now();

	simspon(piCalc, x, dx, n, a);


	auto ser_end = std::chrono::steady_clock::now() - ser_start;
	auto ns_ser = std::chrono::duration_cast<std::chrono::microseconds>(ser_end).count();

	std::cout << "Serial version took " << ns_ser << " microseconds." << std::endl;

	std::cout << "Speed difference: " << ns_par - ns_ser << " Positive number means parallel was slower." << std::endl;
	system("PAUSE");
}





double piCalc(double x) {

	return sqrt(1.0 - pow(x, 2));

}

void simspon(functionCall func, double* x, double dx, int n , double a) {


	double sum = 0;

	for (int i = 1; i < n; i++) {

		if (i < (n - 2) && i % 2 == 0) {

			sum += 4.0*func(a + (i)*dx);

		}

		if (i < (n - 1) && i % 2 != 0) {

			sum += 2.0*func(a + (i)*dx);
		}
	}

	sum += func(a);
	sum += func(a + n*dx);
	sum = (dx / 3.0)*sum;

	std::cout << "Result:" << sum << std::endl;

}



void simspon_par(functionCall func, double* x, double dx, int n, double a) {


double sum = 0;
int numThreads = 0;
#pragma omp parallel 
{
	numThreads = omp_get_num_threads();
#pragma omp for
	for (int i = 1; i < n; i++) {

		if (i < (n - 2) && i % 2 == 0) {

			sum += 4.0*func(a + (i)*dx);

		}

		if (i < (n - 1) && i % 2 != 0) {

			sum += 2.0*func(a + (i)*dx);
		}
	}

	sum += func(a);
	sum += func(a + n*dx);
	sum = (dx / 3.0)*sum;

	std::cout << "Result:" << sum << std::endl;
	}

std::cout << "Used " << numThreads << " threads." << std::endl;

}
double relative_error(double val1, double val2) {


	return 1.0;
}
