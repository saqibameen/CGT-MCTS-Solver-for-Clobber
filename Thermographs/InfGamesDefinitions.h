#ifndef INFGAMESDEFINITIONS_H
#define INFGAMESDEFINITIONS_H

#include "InfinitesimalGame.h"

using namespace std;
class ReferenceGames {
public:
    InfinitesimalGame zero = InfinitesimalGame(ASvalue(), "zero");
    InfinitesimalGame star = InfinitesimalGame(&zero, &zero, 1, -1, ASvalue(0,0,0,1), "star");
    InfinitesimalGame u = InfinitesimalGame(&zero, &star, 1, 0, ASvalue(1,0,0,0), "up");
    InfinitesimalGame d = InfinitesimalGame(&star, &zero, 0, -1, ASvalue(-1,0,0,0), "down");

    // down + star
    InfinitesimalGame dps = InfinitesimalGame(&zero, vector<InfinitesimalGame*> {&zero, &star}, 1, -1,
                                              ASvalue(-1,0,0,1), "down + star"); //
    // up^2
    InfinitesimalGame u2 = InfinitesimalGame(&zero, &dps, 1, 0, ASvalue(0,1,0,0), "up^2"); //

    // up + star
    InfinitesimalGame ups = InfinitesimalGame(vector<InfinitesimalGame*> {&zero, &star}, &zero, 1, -1,
                                              ASvalue(1,0,0,1), "up + star"); //
    // down^2
    InfinitesimalGame d2 = InfinitesimalGame(&ups, &zero, 0, -1, ASvalue(0,-1,0,0), "down^2"); //

    // down + down^2 + star
    InfinitesimalGame dpd2ps = InfinitesimalGame(&zero, vector<InfinitesimalGame*> {&zero, &dps}, 1, -1,
                                                 ASvalue(-1,-1,0,1), "down + down^2 + star"); //

    // up^3
    InfinitesimalGame u3 = InfinitesimalGame(&zero, &dpd2ps, 1, 0, ASvalue(0,0,1, 0), "up^3"); //

    // up + up^2 + star
    InfinitesimalGame upu2ps = InfinitesimalGame(vector<InfinitesimalGame*> {&zero, &ups}, &zero, 1, -1,
                                                 ASvalue(1,1,0,1), "up + up^2 + star"); //

    // down^3
    InfinitesimalGame d3 = InfinitesimalGame(&upu2ps, &zero, 0, -1, ASvalue(0,0,-1,0), "down^3"); //

    // up + up^2
    InfinitesimalGame upu2 = InfinitesimalGame(&u, &star, 1, 0, ASvalue(1,1,0,0), "up + up^2"); //

    // down + down^2
    InfinitesimalGame dpd2 = InfinitesimalGame(&star, &d, 0, -1, ASvalue(-1,-1,0,0), "down + down^2"); //

    // down^2 + star
    InfinitesimalGame d2ps = InfinitesimalGame(&u, vector<InfinitesimalGame*> {&zero, &star}, 1, -1,
                                               ASvalue(0,-1,0,1), "down^2 + star"); //

    // up + up^3
    InfinitesimalGame upu3 = InfinitesimalGame(&u, &d2ps, 1, 0, ASvalue(1,0,1,0), "up + up^3"); //

    // up^2 + star
    InfinitesimalGame u2ps = InfinitesimalGame(vector<InfinitesimalGame*> {&zero, &star}, &d, 1, -1,
                                               ASvalue(0,1,0,1), "up^2 + star"); //

    // down + down^3
    InfinitesimalGame dpd3 = InfinitesimalGame(&u2ps, &d, 0, -1, ASvalue(-1,0,-1,0), "down + down^3"); //

    // up + up
    InfinitesimalGame upu = InfinitesimalGame(&zero, &ups, 1, 0, ASvalue(2,0,0,0), "up + up"); //

    // down + down
    InfinitesimalGame dpd = InfinitesimalGame(&dps, &zero, 0, -1, ASvalue(-2,0,0,0), "down + down"); //

    // up + up + up^2
    InfinitesimalGame upupu2 = InfinitesimalGame(&upu, &ups, 1, 0, ASvalue(2,1,0,0), "up + up + up^2"); //

    // down + down + down^2
    InfinitesimalGame dpdpd2 = InfinitesimalGame(&dps, &dpd, 0, -1, ASvalue(-2,-1,0,0), "down + down + down^2"); //

    // up + up + up^3
    InfinitesimalGame uw = InfinitesimalGame(&upu, vector<InfinitesimalGame*> {&zero, &ups}, 1, -1,
                                             ASvalue(), "Weird Up Game"); //
    InfinitesimalGame upupu3 = InfinitesimalGame(&upu, &uw, 1, 0, ASvalue(2,0,1,0), "up + up + up^3"); //

    // down + down + down^3
    InfinitesimalGame dw = InfinitesimalGame(vector<InfinitesimalGame*> {&zero, &dps}, &dpd, 1, -1,
                                             ASvalue(), "Weird Down Game"); //
    InfinitesimalGame dpdpd3 = InfinitesimalGame(&dw, &dpd, 0, -1, ASvalue(-2,0,-1,0), "down + down + down^3"); //

    // up + up + star
    InfinitesimalGame upups = InfinitesimalGame(&zero, &u, 1, 0, ASvalue(2,0,0,1), "up + up + star"); //
    // up + up + up
    InfinitesimalGame upupu = InfinitesimalGame(&zero, &upups, 1, 0, ASvalue(3,0,0,0), "up + up + up"); //

    // down + down + star
    InfinitesimalGame dpdps = InfinitesimalGame(&d, &zero, 0, -1, ASvalue(-2,0,0,1), "down + down + star"); //
    // down + down + down
    InfinitesimalGame dpdpd = InfinitesimalGame(&dpdps, &zero, 0, -1, ASvalue(-3,0,0,0), "up + up + up"); //

    // up^3 + star
    InfinitesimalGame u3ps = InfinitesimalGame(vector<InfinitesimalGame*> {&zero, &star}, &dpd2, 1, -1,
                                               ASvalue(0,0,1,1), "down^3 + star"); //

    // up + up^3 + star
    InfinitesimalGame upu3ps = InfinitesimalGame(vector<InfinitesimalGame*> {&zero, &ups}, &d2, 1, -1,
                                                 ASvalue(1,0,1,1), "up + up^3 + star"); //

    // down^3 + star
    InfinitesimalGame d3ps = InfinitesimalGame(&upu2, vector<InfinitesimalGame*> {&zero, &star}, 1, -1,
                                               ASvalue(0,0,-1,1), "down^3 + star"); //

    // down + down^3 + star
    InfinitesimalGame dpd3ps = InfinitesimalGame(&u2, vector<InfinitesimalGame*> {&zero, &dps}, 1, -1,
                                                 ASvalue(-1,0,-1,1), "down + down^3 + star"); //

    // unknown game: token game for when it is not possible to determine an upper or lower bound
    InfinitesimalGame unkn = InfinitesimalGame(&zero,&zero, 0, 0, ASvalue(2,-2,2,-2), "unknown");

    vector<InfinitesimalGame> ReferenceGames = {upups, upupu, ups, upupu2, upupu3, upu, upu2, upu3, u, u2, u3, star,
                                                d3, d2, d, dpd3, dpd2, dpd, dpdpd3, dpdpd2, dps, dpdpd, dpdps};

    vector<InfinitesimalGame> PositiveStarGames = {u3ps, u2ps, ups, upu3ps, upu2ps, upups};
    vector<InfinitesimalGame> NegativeStarGames = {d3ps, d2ps, dps, dpd3ps, dpd2ps, dpdps};
    vector<InfinitesimalGame> PositiveGames = {u3, u2, u, upu3, upu2, upu, upupu3, upupu2, upupu, upups};
    vector<InfinitesimalGame> NegativeGames = {d3, d2, d, dpd3, dpd2, dpd, dpdpd3, dpdpd2, dpdpd, dpdps};
};

#endif //INFGAMESDEFINITIONS_H
