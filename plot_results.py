import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

fp = open("/Users/Janjua/Desktop/MSCS/Semester 2nd/CMPUT655/project/new_code/clobber2d-cgt-player/output/results/results.txt", "r")
lines = fp.readlines()
data = {}
for line in lines:
    move_secs, board_size, no_games, white_ratio, black_ratio = line.strip().split(" ")
    if move_secs not in data.keys():
        data[move_secs] = []
        data[move_secs].append([board_size, no_games, white_ratio, black_ratio])
    else:
        data[move_secs].append([board_size, no_games, white_ratio, black_ratio])

ratios6x6_white, ratios8x8_white, ratios10x10_white = [], [], []
ratios6x6_black, ratios8x8_black, ratios10x10_black = [], [], []
for key, val in data.items():
    for k in val:
        board_size, no_games, white_ratio, black_ratio = k
        if board_size == "6x6":
            ratios6x6_white.append(float(white_ratio))
            ratios6x6_black.append(float(black_ratio))
        if board_size == "8x8":
            ratios8x8_white.append(float(white_ratio))
            ratios8x8_black.append(float(black_ratio))
        if board_size == "10x10":
            ratios10x10_white.append(float(white_ratio))
            ratios10x10_black.append(float(black_ratio))
        
move_seconds = data.keys()

plt.figure(figsize=(10, 6))
plt.rcParams['text.usetex'] = True
X_axis = np.arange(len(move_seconds))
plt.bar(X_axis - 0.4, ratios6x6_white, 0.1, color='tab:orange', label=r'CGT$(6\times 6)$')
plt.bar(X_axis - 0.2, ratios6x6_black, 0.1, color='tab:red', label=r'Ours$(6\times 6)$')

plt.bar(X_axis, ratios8x8_white, 0.1, color='tab:cyan', label=r'CGT$(8\times 8)$')
plt.bar(X_axis + 0.2, ratios8x8_black, 0.1, color='tab:green', label=r'Ours$(8\times 8)$')

plt.bar(X_axis + 0.4, ratios10x10_white, 0.1, color='tab:olive', label=r'CGT$(10\times 10)$')
plt.bar(X_axis + 0.6, ratios10x10_black, 0.1, color='salmon', label=r'Ours$(10\times 10)$')

# plt.figtext(0.5, 0.01, r'Board Sizes = $(6\times 6, 8\times 8, 10\times 10)$', ha='center')

plt.axhline(y=0.5, color='k', linestyle='--', label=r'$50\%$')

plt.legend(loc='center left', bbox_to_anchor=(1, 0.5))
plt.title("Modified Backprop + SecureChild Win Ratio Comparison of CGT (Ours) and CGT (Kris's)")
# plt.xticks(X_axis, move_seconds)
plt.ylabel("Win Ratio")
plt.xlabel(r"30 Seconds Move Time $\rightarrow$")
plt.savefig("modified_backprop_secure_child_win_ratio_results.png", dpi=200, bbox_inches='tight')
plt.show()