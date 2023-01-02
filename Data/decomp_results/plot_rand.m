close all;
clear all;
clc;

files = {'cd_rand_04.txt', 'cd_rand_06.txt', 'cd_rand_08.txt', 'cd_rand_10.txt'};
dims = [4, 6, 8, 10];
cs = ['k', 'r', 'g', 'b'];

figure; hold on;
for i = 1:numel(dims)
    plot(mean(dlmread(files{i}, ','), 1), cs(i));
end

axis([1, 70, 1, 7]);
legend('4x4', '6x6', '8x8', '10x10', 'location', 'northwest');
xlabel('Random Moves');
ylabel('Average Subgames');
title('Clobber Decomposition Test');