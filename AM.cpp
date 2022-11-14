#include <iostream>
#include <fstream>
#include <cstdio>
#include <complex>
using namespace std;

FILE* AMfile;

int main()
{
	setlocale(LC_ALL, "ru");

	// Открытие файла
	errno_t err;
	err = fopen_s(&AMfile, "am_sound (1).dat", "rb"); // Предполагается, что файл лежит в папке проекта
	if (err == 0)
	{
		cout << "Файл открыт." << endl;
	}
	
	// Количество отсчётов
	long lSize;
	cout << "Введите количество отсчётов: "; // По хорошему программа должна сама высчитывать объём файла в байтах и делить на размер блока
	cin >> lSize;
	
	// Переменные
	int j = 0;
	int o = 0;
	double mean = 0; 
	double summ = 0;
	
	// Выделение памяти для хранения содержимого файла
	int* buffer = (int*)malloc(4 * lSize); 
	int* ReAM = (int*)malloc(2 * lSize);
	int* ImAM = (int*)malloc(2 * lSize);
	double* abs = (double*)malloc(4 * lSize);
	if (buffer == NULL)
	{
		fputs("Ошибка памяти", stderr);
		exit(2);
	}

	// Считывание файла в буфер
	size_t result = fread(buffer, 4, lSize, AMfile);
	if (result != lSize)
	{
		fputs("Ошибка чтения", stderr);
		exit(3);
	}

	// Формирование мнимого и вещественного векторов
	for (int i = 0; i < lSize; i++)
	{
		if (i % 2 == 0)
		{
			ReAM[j] = buffer[i];
			//cout << ReAM[j] << " ";	// Отладочная строка. Не выполнять при больших lSize
			j++;
		}
		else
		{
			ImAM[o] = buffer[i];
			//cout << ImAM[o] << endl;	// Отладочная строка. Не выполнять при больших lSize
			o++;
		}
	}

	// Нахождение модулирующего сигнала
	for (int i = 0; i < lSize/2; i++)
	{
		std::complex<double> z(ReAM[i], ImAM[i]);
		double rho = std::abs(z);
		abs[i] = rho;
		summ = summ + abs[i];		
	}
	mean = 2 * summ / lSize;
	cout << mean << endl; // Проверочная информация
	for (int i = 0; i < lSize / 2; i++)
	{
		abs[i] = abs[i] - mean;
		//cout << abs[i] << endl;	// Отладочная строка. Не выполнять при больших lSize
	}

	// Возможно нужен фильтр скользящего среднего

	// Необходим вывод в wav файл и прослушивание через функцию playsound

	// завершение работы
	if (AMfile)
	{
		err = fclose(AMfile);
	}

	//Освобождение памяти
	free(buffer);
	free(ReAM);
	free(ImAM);
	free(abs);

	return 0;
}