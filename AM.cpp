#include <iostream>
#include <fstream>
#include <cstdio>
#include <complex>
using namespace std;

FILE* AMfile;

int main()
{
	setlocale(LC_ALL, "ru");

	// �������� �����
	errno_t err;
	err = fopen_s(&AMfile, "am_sound (1).dat", "rb"); // ��������������, ��� ���� ����� � ����� �������
	if (err == 0)
	{
		cout << "���� ������." << endl;
	}
	
	// ���������� ��������
	long lSize;
	cout << "������� ���������� ��������: "; // �� �������� ��������� ������ ���� ����������� ����� ����� � ������ � ������ �� ������ �����
	cin >> lSize;
	
	// ����������
	int j = 0;
	int o = 0;
	double mean = 0; 
	double summ = 0;
	
	// ��������� ������ ��� �������� ����������� �����
	int* buffer = (int*)malloc(4 * lSize); 
	int* ReAM = (int*)malloc(2 * lSize);
	int* ImAM = (int*)malloc(2 * lSize);
	double* abs = (double*)malloc(4 * lSize);
	if (buffer == NULL)
	{
		fputs("������ ������", stderr);
		exit(2);
	}

	// ���������� ����� � �����
	size_t result = fread(buffer, 4, lSize, AMfile);
	if (result != lSize)
	{
		fputs("������ ������", stderr);
		exit(3);
	}

	// ������������ ������� � ������������� ��������
	for (int i = 0; i < lSize; i++)
	{
		if (i % 2 == 0)
		{
			ReAM[j] = buffer[i];
			//cout << ReAM[j] << " ";	// ���������� ������. �� ��������� ��� ������� lSize
			j++;
		}
		else
		{
			ImAM[o] = buffer[i];
			//cout << ImAM[o] << endl;	// ���������� ������. �� ��������� ��� ������� lSize
			o++;
		}
	}

	// ���������� ������������� �������
	for (int i = 0; i < lSize/2; i++)
	{
		std::complex<double> z(ReAM[i], ImAM[i]);
		double rho = std::abs(z);
		abs[i] = rho;
		summ = summ + abs[i];		
	}
	mean = 2 * summ / lSize;
	cout << mean << endl; // ����������� ����������
	for (int i = 0; i < lSize / 2; i++)
	{
		abs[i] = abs[i] - mean;
		//cout << abs[i] << endl;	// ���������� ������. �� ��������� ��� ������� lSize
	}

	// �������� ����� ������ ����������� ��������

	// ��������� ����� � wav ���� � ������������� ����� ������� playsound

	// ���������� ������
	if (AMfile)
	{
		err = fclose(AMfile);
	}

	//������������ ������
	free(buffer);
	free(ReAM);
	free(ImAM);
	free(abs);

	return 0;
}