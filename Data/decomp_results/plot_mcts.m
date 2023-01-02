close all;
clear all;
clc;

files = {'cd_mcts_04.txt', 'cd_mcts_06.txt', 'cd_mcts_08.txt', 'cd_mcts_10.txt'};
dims = [4, 6, 8, 10];
cs = ['k', 'r', 'g', 'b'];

figure; hold on;
for i = 1:numel(dims)
    plot(mean(dlmread(files{i}, ','), 1), cs(i));
end

axis([1, 70, 1, 7]);
legend('4x4', '6x6', '8x8', '10x10', 'location', 'northwest');
xlabel('MCTS Moves');
ylabel('Average Subgames');
title('Clobber Decomposition Test');

%{
vals = dlmread('cd_mcts_vals.csv', ',');
ends = zeros(30, 1);
for i = 1:30
    ends(i) = find(vals(i, :) > 0.7, 1, 'first');
end
ends = [mean(ends(1:10)); mean(ends(11:20)); mean(ends(21:30))];
for i = 1:3
    plot([ends(i) ends(i)], [1 7], [cs(i + 1) '--']);
end
%}