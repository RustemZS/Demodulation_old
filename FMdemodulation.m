%Открытие файла
fileID = fopen('file1EuropaPlus (1).bin');
%Чтение данных в вектор
fm_S = fread(fileID,'float32');
Fd=32000; %Частота дискретизации
ReS=fm_S(1:2:end-1); %Действительная часть отсчётов
ImS=fm_S(2:2:end); %Мнимая часть отсчётов
psi=unwrap(ImS./ReS); %Фаза комплексного сигнала
y_fm=diff(psi)*Fd; %Частота комплексного сигнала

%Фильтр скользящего среднего
windowSize = 32; 
b = (1/windowSize)*ones(1,windowSize);
a = 1;
z_fm = filter(b,a,y_fm);

%Передискретизация
z_fm = downsample(z_fm,16);

%Отладочная часть
%L=length(z_fm);
%x=1:L;
%stem(x,z_fm);

%Проверка звука
sound(z_fm,Fd);