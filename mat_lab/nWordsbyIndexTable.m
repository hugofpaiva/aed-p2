T1 = readtable("palavras_por_indice_linked_list_test.txt");
T2 = readtable("palavras_por_indice_linked_list_sherlock.txt");

x = T1.Var1;
y = T1.Var2;

figure
bar(x, y, 'b');
title('Test Index of Table / Number of Words');

x = T2.Var1;
y = T2.Var2;

figure
bar(x, y, 'b');
title('Sherlock Index of Table / Number of Words');


