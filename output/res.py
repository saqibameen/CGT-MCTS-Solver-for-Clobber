import os

tests = {
    30: {
        'path': './30s',
        "6": "29657359",
        "8": "29660410",
        "10": "3944862",
    }
}

# enumerate keys of tests.
for key in tests.keys():

    # enumerate keys of tests[key].
    board_sizes = [6, 8, 10]
    for bs in board_sizes:
        files = [f for f in os.listdir(tests[key]['path']) if os.path.isfile(tests[key]['path'] + "/" + f) and (str(tests[key][str(bs)])) in f]
        w_wins = 0
        b_wins = 0

        for file in files:
            with open(tests[key]['path'] + "/" + file, 'r') as f:
                lines = f.readlines()
                for line in lines:
                    if "winner:W" in line:
                        w_wins += 1
                    elif "winner:B" in line: 
                        b_wins += 1
                
        # Total games:
        total_games = w_wins + b_wins
        # white wins.
        w_win_percent = w_wins / total_games
        # black wins.
        b_win_percent = b_wins / total_games

        print(str(key) + " " + str(bs) + "x" + str(bs) + " 100 " + str(w_win_percent) + " " + str(b_win_percent))


# print("total games:", total_games)
# print("White wins: " + str(w_win_percent))
# print("Black wins: " + str(b_win_percent))

