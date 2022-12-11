#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;

// Комплексный отсчёт
template <class T>
struct Complex {
	Complex()
	{
		re = 0;
		im = 0;
		result = 0;
		phase = 0;
	}
	Complex(float re, float im)
	{
		this->re = re;
		this->im = im;
		this->phase = 0;
		this->result = 0;
	}

	// Нахождение модуля комплексного числа
	T ABS()
	{
		this->result = sqrt(re * re + im * im);
		return sqrt(re * re + im * im);
	}

	// Нахождение фазы комплексного числа
	void ARG(int & i, std::vector<Complex<float>> & signal)
	{
		if (i == 0)
		{
			this->phase = re / im;
		}
		else
		{
			T razn = (im / re) - (signal[i-1].im / signal[i-1].re);
			while((razn >= M_PI) || (razn <= -1 * M_PI)) // Приведение к [-pi, pi]
			{
				if (razn > M_PI)
					razn -= 2 * M_PI;
				else
					razn += 2 * M_PI;
			}
			this->phase = (signal[i - 1].phase + razn);
		}
		
	}

	// Определение частоты
	void FREQ(int& i, int & Fd, std::vector<Complex<float>>& signal)
	{
		if (i == 0)
		{
			this->result = phase * Fd;
		}
		else
		{		
			this->result = (phase - signal[i - 1].phase) * Fd;
		}
	}

	T re ;
	T im ;
	T result;
	T phase;
};

// Файл менеджер
struct FileManager {
	// Определение размера файла
	FileManager(string & path, long int & size)
	{
		ifstream file;

		file.open(path, ios::binary | ios::in);
		if (!file.is_open())
		{
			cout << "Файл не открылся" << endl;
			exit(1);
		}

		file.seekg(0, std::ios::end);
		size = file.tellg() / 4;
		
		file.close();			
	}
	
	// Извлечение отсчётов из файла
	FileManager(string& path, string& modulation, long int& size, Complex<float> & sample, std::vector<Complex<float>> & signal)
	{
		ifstream file;

		file.open(path, ios::binary | ios::in);
		if (!file.is_open())
		{
			cout << "Файл не открылся" << endl;
			exit(1);
		}
		file.seekg(0, std::ios::beg);
		int j = 0, k = 0;
		for (int i = 0; i < size; i++)
		{

			if (i % 2 == 0)
			{
				if (modulation == "AM")
				{
					int data_re = 0;
					file.read((char*)&data_re, sizeof data_re);
					sample.re = data_re;
				}
				else if (modulation == "FM")
				{
					file.read((char*)&sample.re, sizeof sample.re);
				}
				j++;
			}
			else
			{
				if (modulation == "AM")
				{
					int data_im = 0;
					file.read((char*)&data_im, sizeof data_im);
					sample.im = data_im;
				}
				else if (modulation == "FM")
				{
					file.read((char*)&sample.im, sizeof sample.im);
				}
				k++;
				signal[k - 1] = sample;
			}

		}

		file.close();
	}
};

// Интерфейс демодулятора
struct Demodulator {
	virtual void demodulator(long int & size, int & Fd, std::vector<Complex<float>> & signal) = 0;
};

struct DemodulatorAM : Demodulator {
	void demodulator(long int & size, int& Fd, std::vector<Complex<float>> & signal) override
	{
		float summ = 0;
		for (int i = 0; i < size / 2; i++)
		{
			summ = summ + signal[i].ABS();
		}
		float mean = 2*summ/size;
		for (int i = 0; i < size / 2; i++)
		{
			signal[i].result = signal[i].result - mean;
		}
	}
};

struct DemodulatorFM : Demodulator {
	void demodulator(long int& size, int& Fd, std::vector<Complex<float>>& signal) override
	{
		for (int i = 0; i < size / 2; i++)
		{
			signal[i].ARG(i, signal);
			signal[i].FREQ(i, Fd, signal);
		}
	}
};

// Фильтр скользящего среднего
struct Filter {
	Filter(bool & type, int & window, long int n, std::vector<Complex<float>>& signal)
	{
		if (type)
		{
			// Нерекурсивный фильтр
			// window - Количество отсчетов интервала усреднения
			float sumx = 0; // Сумма отсчетов на интервале
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < window - 1; j++)
				{
					while (i >= j)
					{
						sumx += signal[i - j].result;
					}
					
				}
				signal[i].result = sumx / window;
				sumx = 0;
			}
		}
		else
		{
			// Рекурсивный фильтр
			// window - Количество отсчетов интервала усреднения
			float sumx = 0; // Сумма отсчетов на интервале
			float* mas; // Массив для хранения window отсчетов
			int index = 0; // Индекс элемента массива
			mas = new float[window];
			for (int i = 0; i < window; i++)
				mas[i] = 0;
			for (int i = 0; i < n; i++) {
				sumx -= mas[index];
				mas[index] = signal[i].result;
				sumx += mas[index];
				index++;
				if (index >= window)
					index = 0; // Возврат к началу "окна"
				signal[i].result = sumx / window;
			}
			delete[] mas;
		}
		
	}
};

// Передискретизация
void Downsample(int & parameter_down,long int & size, std::vector<Complex<float>>& signal)
{
	int k = size / (parameter_down * 2);
	std::vector<Complex<float>> copy(k);
	int j = 0;
	for (int i = 1; i < size / 2; i+= parameter_down)
	{
		copy[j] = signal[i];
		j++;
	}

	signal = copy;
}

int main()
{
	setlocale(LC_ALL, "ru");
	// Объявление переменных
	string modulation; // Тип модуляции
	string path; // Путь к файлу
	int Fd;	// Частота дискретизации
	long int size = 0; // Размер файла
	int window = 1; // Размер окна
	int parameter_down = 1; // Параметр передискретизации
	bool flt = 0; // Условие включения фильтра
	bool dwn = 0; // Условие включения передискретизации
	bool type = 0; // Выбор типа фильтра
	
	// Ввод данных
	cout << "Тип модуляции: ";
	getline(cin, modulation);
	cout << "Путь к файлу: ";
	getline(cin, path);
	cout << "Частота дискретизации: ";
	cin >> Fd;
	cout << "Нужна ли фильтрация (0 - нет, 1 - да): ";
	cin >> flt;
	if (flt) 
	{
		cout << "Тип фильтра (1 - нерекурсивный, 0 - рекурсивный): ";
		cin >> type;
		cout << "Размер окна: ";
		cin >> window;
	}
	cout << "Нужна ли передискретизация (0 - нет, 1 - да): ";
	cin >> dwn;
	if (dwn)
	{
		cout << "Параметр передискретизации: ";
		cin >> parameter_down;
	}
	
	// Считывание отсчётов сигнала из файла
	Complex<float> sample;
	FileManager filesize(path, size); // Определение размера файла
	std::vector<Complex<float>> signal(size/2);
	FileManager filesamples(path, modulation, size, sample, signal); // Считывание отсчётов из файла
	
	// Демодуляция
	DemodulatorAM AM;
	DemodulatorFM FM;
	if (modulation == "AM") 
	{
		AM.demodulator(size, Fd, signal);
	}
	else if (modulation == "FM")
	{
		FM.demodulator(size, Fd, signal);
	}
	
	// Фильтрация
	if (flt)
	{
		Filter filter(type, window, size / 2, signal);
	}
	
	// Передискретизация
	if (dwn)
	{
		Downsample(parameter_down, size, signal);
	}

	ofstream fout;
	fout.open("LogSignal.txt");
	for (int i=0; i < size / (parameter_down * 2); i++)
	{
		fout << signal[i].result << endl;
	}
	fout.close();
	
	return 0;
}