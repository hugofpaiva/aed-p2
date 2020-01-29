obt_x = [1, 2, 3, 4]
obt_r_y = [0.286727, 0.291638, 0.298523, 0.284660]
obt_t_y = [1.115338, 1.113946, 0.996195, 1.118534]

ll_r_y = [0.395092, 0.388120, 0.409097, 0.526286]
ll_t_y = [1.122216, 1.034441, 1.072920, 1.077986]

figure(1);
plot_1 = plot(obt_x, obt_r_y);
hold
plot_2 = plot(obt_x, ll_r_y);

title('Reading Speed')
ylabel('Time(s)')
set(gca, 'YScale', 'log')

legend([plot_1 plot_2], "BTree", "LinkList")


figure(2);
plot_1 = plot(obt_x, obt_t_y);
hold
plot_2 = plot(obt_x, ll_t_y);

title('Travel Speed')
ylabel('Time(s)')
set(gca, 'YScale', 'log')

legend([plot_1 plot_2], "BTree", "LinkList")