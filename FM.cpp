#include <iostream>
#include <fstream>
#include <cstdio>
#include <complex>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

FILE* FMfile;

// Функция unwrap
double unwrap(double re_prev, double im_prev, double re_now, double im_now, double ans_prev)
{
	double razn = (im_now / re_now) - (im_prev / re_prev);
		while ((razn > M_PI) || (razn < -1 * M_PI)) // Приведение к [-pi, pi]
		{
			if (razn > M_PI)
				razn -= 2*M_PI;
			else
				razn += 2*M_PI;
		}
		ans_prev += razn;
		return ans_prev;
}

// Фильтр скользящего среднего
void filter(double * x, double* y, long int n, int size) {
	// size - Количество отсчетов интервала усреднения
	double sumx = 0; // Сумма отсчетов на интервале
	double* mas; // Массив для хранения size отсчетов
	int index = 0; // Индекс элемента массива
	mas = new double[size];
	for (int i = 0; i < size; i++) 
		mas[i] = 0;
	for (int i = 0; i < n; i++) {
		sumx -= mas[index];
		mas[index] = x[i];
		sumx += mas[index];
		index++;
		if (index >= size)
			index = 0; // Возврат к началу "окна"
		y[i] = sumx / size;
	}
	delete[] mas;
	return;
}

int main()
{
	setlocale(LC_ALL, "ru");

	// Открытие файла
	errno_t err;
	err = fopen_s(&FMfile, "file1EuropaPlus (1).bin", "rb"); // Предполагается, что файл лежит в папке проекта
	if (err == 0)
	{
		cout << "Файл открыт." << endl;
	}

	// Вводимые параметры
	long lSize;
	int window;
	int down;
	cout << "Введите количество отсчётов: "; // По хорошему программа должна сама высчитывать объём файла в байтах и делить на размер блока
	cin >> lSize;
	cout << "Введите окно: "; // Параметр для фильтра 32
	cin >> window;
	cout << "Введите параметр передискретизации: "; // Параметр для передискретизации 16
	cin >> down;

	// Переменные
	int j = 0;
	int o = 0;
	double Fd = 32000;

	// Выделение памяти для хранения содержимого файла
	float* buffer = new float[4*lSize];
	float* ReFM = new float[2 * lSize];
	float* ImFM = new float[2 * lSize];
	double* phase = new double[4 * lSize];
	double* freq = new double[4 * lSize - 1];
	double* fltr = new double[4 * lSize - 1];
	double* ans = new double[lSize / 2];
	
	// Считывание файла в буфер
	size_t result = fread(buffer, 4, lSize, FMfile);
	

	// Формирование мнимого и вещественного векторов
	for (int i = 0; i < lSize; i++)
	{
		if (i % 2 == 0)
		{
			ReFM[j] = buffer[i];
			//cout << ReFM[j] << " ";	// Отладочная строка. Не выполнять при больших lSize
			j++;
		}
		else
		{
			ImFM[o] = buffer[i];
			//cout << ImFM[o] << endl;	// Отладочная строка. Не выполнять при больших lSize
			o++;
		}
	}

	// Нахождение модулирующего сигнала
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
			//cout << ans[i-1] << endl;		// Отладочная строка. Не выполнять при больших lSize
		}
		//cout << phase[i] << endl;		// Отладочная строка. Не выполнять при больших lSize
	}

	// Фильтрация
	for (int i = 0; i < (lSize / 2) - 1; i++)
	{
		filter(freq, fltr, (lSize / 2) - 1, window);
	}

	// Передискретизация
	for (int i = 0; i < (lSize / 2) - 1; i += down)
	{
		ans[i]=fltr[i];
		//cout << ans[i] << endl;		// Отладочная строка. Не выполнять при больших lSize
	}

	// Необходим вывод в wav файл и прослушивание через функцию playsound

	// Завершение работы
	if (FMfile)
	{
		err = fclose(FMfile);
	}

	// Освобождение памяти
	delete [] buffer;
	delete [] ReFM;
	delete [] ImFM;
	delete [] phase;
	delete [] freq;
	delete [] fltr;
	delete[] ans;

	return 0;
}