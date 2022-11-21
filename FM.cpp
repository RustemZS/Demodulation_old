#include <iostream>
#include <fstream>
#include <cstdio>
#include <complex>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

FILE* FMfile;

// ������� unwrap
double unwrap(double re_prev, double im_prev, double re_now, double im_now, double ans_prev)
{
	double razn = (im_now / re_now) - (im_prev / re_prev);
		while ((razn > M_PI) || (razn < -1 * M_PI)) // ���������� � [-pi, pi]
		{
			if (razn > M_PI)
				razn -= 2*M_PI;
			else
				razn += 2*M_PI;
		}
		ans_prev += razn;
		return ans_prev;
}

// ������ ����������� ��������
void filter(double * x, double* y, long int n, int size) {
	// size - ���������� �������� ��������� ����������
	double sumx = 0; // ����� �������� �� ���������
	double* mas; // ������ ��� �������� size ��������
	int index = 0; // ������ �������� �������
	mas = new double[size];
	for (int i = 0; i < size; i++) 
		mas[i] = 0;
	for (int i = 0; i < n; i++) {
		sumx -= mas[index];
		mas[index] = x[i];
		sumx += mas[index];
		index++;
		if (index >= size)
			index = 0; // ������� � ������ "����"
		y[i] = sumx / size;
	}
	delete[] mas;
	return;
}

int main()
{
	setlocale(LC_ALL, "ru");

	// �������� �����
	errno_t err;
	err = fopen_s(&FMfile, "file1EuropaPlus (1).bin", "rb"); // ��������������, ��� ���� ����� � ����� �������
	if (err == 0)
	{
		cout << "���� ������." << endl;
	}

	// �������� ���������
	long lSize;
	int window;
	int down;
	cout << "������� ���������� ��������: "; // �� �������� ��������� ������ ���� ����������� ����� ����� � ������ � ������ �� ������ �����
	cin >> lSize;
	cout << "������� ����: "; // �������� ��� ������� 32
	cin >> window;
	cout << "������� �������� �����������������: "; // �������� ��� ����������������� 16
	cin >> down;

	// ����������
	int j = 0;
	int o = 0;
	double Fd = 32000;

	// ��������� ������ ��� �������� ����������� �����
	float* buffer = new float[4*lSize];
	float* ReFM = new float[2 * lSize];
	float* ImFM = new float[2 * lSize];
	double* phase = new double[4 * lSize];
	double* freq = new double[4 * lSize - 1];
	double* fltr = new double[4 * lSize - 1];
	double* ans = new double[lSize / 2];
	
	// ���������� ����� � �����
	size_t result = fread(buffer, 4, lSize, FMfile);
	

	// ������������ ������� � ������������� ��������
	for (int i = 0; i < lSize; i++)
	{
		if (i % 2 == 0)
		{
			ReFM[j] = buffer[i];
			//cout << ReFM[j] << " ";	// ���������� ������. �� ��������� ��� ������� lSize
			j++;
		}
		else
		{
			ImFM[o] = buffer[i];
			//cout << ImFM[o] << endl;	// ���������� ������. �� ��������� ��� ������� lSize
			o++;
		}
	}

	// ���������� ������������� �������
	for (int i = 0; i < lSize / 2; i++)
	{
		if (i == 0)
		{
			phase[i] = ImFM[i] / ReFM[i];
		}
		else
		{
			phase[i] = unwrap(ReFM[i-1], ImFM[i-1], ReFM[i], ImFM[i], phase[i-1]);
			freq[i - 1] = (phase[i] - phase[i - 1]) * Fd;
			//cout << ans[i-1] << endl;		// ���������� ������. �� ��������� ��� ������� lSize
		}
		//cout << phase[i] << endl;		// ���������� ������. �� ��������� ��� ������� lSize
	}

	// ����������
	for (int i = 0; i < (lSize / 2) - 1; i++)
	{
		filter(freq, fltr, (lSize / 2) - 1, window);
	}

	// �����������������
	for (int i = 0; i < (lSize / 2) - 1; i += down)
	{
		ans[i]=fltr[i];
		//cout << ans[i] << endl;		// ���������� ������. �� ��������� ��� ������� lSize
	}

	// ��������� ����� � wav ���� � ������������� ����� ������� playsound

	// ���������� ������
	if (FMfile)
	{
		err = fclose(FMfile);
	}

	// ������������ ������
	delete [] buffer;
	delete [] ReFM;
	delete [] ImFM;
	delete [] phase;
	delete [] freq;
	delete [] fltr;
	delete[] ans;

	return 0;
}