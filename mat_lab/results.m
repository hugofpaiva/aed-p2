T1 = readtable("taxa_de_ocupa��o_linked_list_test.txt");
T2 = readtable("taxa_de_ocupa��o_linked_list_sherlock.txt");

x = (T1.Var1);
y = [1:length(x)];

figure
plot(y, x, 'b');
title('%Table Occupa / Number of Words');


x = (T2.Var1);
y = [1:length(x)];

figure
plot(y, x, 'b');
title('%Table Occupa / Number of Words');