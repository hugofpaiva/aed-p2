T1 = readtable("results_fna_test.txt");
T2 = readtable("results_fla_test.txt");

w_fna = T1.Word;
x = string(w_fna);
x = length(x);
y1 = double(T1.Dmin);
y2 = double(T1.AvgD);
y3 = double(T1.Dmax);

figure
bar_1 = bar(x, y3, 'r', 'BarWidth', 2);
hold on
bar_2 = bar(x, y2, 'b', 'BarWidth', 2);
hold on
bar_3 = bar(x, y1, 'g', 'BarWidth', 2);


figure
plot(y3, x, 'r')
hold on
plot(y2, x, 'b')
hold on
plot(y1, x, 'g')
