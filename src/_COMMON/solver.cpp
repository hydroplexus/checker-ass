#include <QString>

#include "solver.h"

Solver::Solver()
{
}

Solver::Sieve::Moves* Solver::getMoves()
{
    return &_sieve.moves;
}

void Solver::SetMoves(const Xeno::Move::Snap & snap, Color color)
{
    _sieve.color			= color;
    _sieve.player			= color ? snap.W : snap.B;
    _sieve.enemy			= color ? snap.B : snap.W;
    _sieve.super			= snap.S;
    _sieve.playerHoles		= ~_sieve.player;
    _sieve.enemyHoles		= ~_sieve.enemy;
    _sieve.playerPerimeter	= _sieve.player & _sieve.maskPerimeter;
    _sieve.enemyPerimeter	= _sieve.enemy & _sieve.maskPerimeter;
    _sieve.all				= _sieve.player | _sieve.enemy;
    _sieve.empty			= ~_sieve.all;
    _sieve.moves.unlocked = _sieve.moves.hunter = _sieve.moves.prey = 0;
    _sieve.moves.drops.clear();

    _FindIntervals();
    _FindPrey();
    _FindUnlocked();
    _ProcessOrdinary();
    _ProcessSuper();
}

void Solver::_FindIntervals()
//	в каждом направлении ищем непрерывные цепочки из фигур (одиночки отсекаются) + те, что вдоль бортика
{
	_sieve.routeFR.interval = _sieve.routeBL.interval =
			~((_sieve.enemy & _enemyFR(_sieve.enemy)) | (_sieve.enemy & _enemyBL(_sieve.enemy)) | _sieve.enemyPerimeter);
	_sieve.routeFL.interval = _sieve.routeBR.interval =
			~((_sieve.enemy & _enemyFL(_sieve.enemy)) | (_sieve.enemy & _enemyBR(_sieve.enemy)) | _sieve.enemyPerimeter);
}

void Solver::_FindPrey()
//	в каждом направлении ищем неприкрытые фигуры противника
{
    _sieve.routeFR.prey = _sieve.enemy & _sieve.routeFR.interval & _playerBL(_sieve.playerHoles);
    _sieve.routeFL.prey = _sieve.enemy & _sieve.routeFL.interval & _playerBR(_sieve.playerHoles);
    _sieve.routeBR.prey = _sieve.enemy & _sieve.routeBR.interval & _playerFL(_sieve.playerHoles);
    _sieve.routeBL.prey = _sieve.enemy & _sieve.routeBL.interval & _playerFR(_sieve.playerHoles);
}

void Solver::_FindUnlocked()
//	в каждом направлении ищем фигуры игрока перед которыми либо пустое поле, либо жертва
{
    _sieve.routeFR.unlocked = (_sieve.player & _playerBL(_sieve.empty)) | (_sieve.player & _enemyFR(_sieve.routeFR.prey));
    _sieve.routeFL.unlocked = (_sieve.player & _playerBR(_sieve.empty)) | (_sieve.player & _enemyFL(_sieve.routeFL.prey));
    _sieve.routeBR.unlocked = (_sieve.player & _playerFL(_sieve.empty)) | (_sieve.player & _enemyBR(_sieve.routeBR.prey));
    _sieve.routeBL.unlocked = (_sieve.player & _playerFR(_sieve.empty)) | (_sieve.player & _enemyBL(_sieve.routeBL.prey));
}

void Solver::_ProcessOrdinary()
{
    _sieve.routeFR.preyOrdinary = _sieve.routeFR.prey & _playerFR(_sieve.routeFR.unlocked ^ _sieve.super);
    _sieve.routeFL.preyOrdinary = _sieve.routeFL.prey & _playerFL(_sieve.routeFL.unlocked ^ _sieve.super);
    _sieve.routeBR.preyOrdinary = _sieve.routeBR.prey & _playerBR(_sieve.routeBR.unlocked ^ _sieve.super);
    _sieve.routeBL.preyOrdinary = _sieve.routeBL.prey & _playerBL(_sieve.routeBL.unlocked ^ _sieve.super);
    _sieve.moves.prey |= _sieve.routeFR.preyOrdinary | _sieve.routeFL.preyOrdinary | _sieve.routeBR.preyOrdinary | _sieve.routeBL.preyOrdinary;

    _sieve.routeFR.hunterOrdinary = _sieve.routeFR.unlocked & _enemyFR(_sieve.routeFR.preyOrdinary);
    _sieve.routeFL.hunterOrdinary = _sieve.routeFL.unlocked & _enemyFL(_sieve.routeFL.preyOrdinary);
    _sieve.routeBR.hunterOrdinary = _sieve.routeBR.unlocked & _enemyBR(_sieve.routeBR.preyOrdinary);
    _sieve.routeBL.hunterOrdinary = _sieve.routeBL.unlocked & _enemyBL(_sieve.routeBL.preyOrdinary);
    _sieve.moves.hunter |= _sieve.routeFR.hunterOrdinary | _sieve.routeFL.hunterOrdinary | _sieve.routeBR.hunterOrdinary | _sieve.routeBL.hunterOrdinary;

    _sieve.routeBR.unlocked ^= ((_sieve.routeBR.unlocked ^ _sieve.super) & _enemyBR(~_sieve.routeBR.preyOrdinary));
    _sieve.routeBL.unlocked ^= ((_sieve.routeBL.unlocked ^ _sieve.super) & _enemyBL(~_sieve.routeBL.preyOrdinary));

    _sieve.moves.unlocked |= (_sieve.routeBL.unlocked | _sieve.routeBR.unlocked | _sieve.routeFL.unlocked | _sieve.routeFR.unlocked) & _sieve.player;

    uint32_t super		= _sieve.color ? _sieve.maskRowLast : _sieve.maskRowFirst;
    uint32_t members	= _sieve.moves.unlocked ^ _sieve.super;
    uint32_t mask		= 0b1;
    uint32_t unlocked;
    uint32_t strike;
    uint32_t step;

    do {
        unlocked = (members & mask);
        if (unlocked) {
            Sieve::Moves::Drops drops;

            step = unlocked;
            _FR(step);
            if (step & _sieve.empty) {drops.available |= step;}
            if (step & _sieve.routeFR.prey) {
                    strike = step;
                    _FR(step);
                    drops.available |= step;
                    drops.strike |= step;
                    drops.strikes.insert(step, strike);
            }
            drops.super |= (step &= super);

            step = unlocked;
            _FL(step);
            if (step & _sieve.empty) {drops.available |= step;}
            if (step & _sieve.routeFL.prey) {
                    strike = step;
                    _FL(step);
                    drops.available |= step;
                    drops.strike |= step;
                    drops.strikes.insert(step, strike);
            }
            drops.super |= (step &= super);

            step = unlocked;
            _BR(step);
            if (step & _sieve.routeBR.prey) {
                    strike = step;
                    _BR(step);
                    drops.available |= step;
                    drops.strike |= step;
                    drops.strikes.insert(step, strike);
            }

            step = unlocked;
            _BL(step);
            if (step & _sieve.routeBL.prey) {
                    strike = step;
                    _BL(step);
                    drops.available |= step;
                    drops.strike |= step;
                    drops.strikes.insert(step, strike);
            }

            _sieve.moves.drops.insert(unlocked, drops);
        }
    }	while (mask<<=1);
    (void)0;
}

void Solver::_ProcessSuper()
{
    uint32_t members	= _sieve.moves.unlocked & _sieve.super & _sieve.player;
    uint32_t mask		= 0b1;
    uint32_t unlocked;
    uint32_t strike;
    uint32_t empty;
    uint32_t step;
    do {
        unlocked = (members & mask);
        if (unlocked) {
            Sieve::Moves::Drops drops;

            step = unlocked;
            strike = empty = 0;
            while (_FR(step)) {
                    if (step & (_sieve.all ^ _sieve.routeFR.prey)) {break;}
                    if (strike) {empty |= step; drops.strike |= step;}
                    if (step & _sieve.routeFR.prey) {
                            if (strike) {break;}
                            else {
                                    strike = step;
                                    _sieve.moves.hunter |= unlocked;
                                    _sieve.moves.prey |= step;
                            }
                    }
                    if (step & _sieve.empty) {drops.available |= step;}
            }
            if (strike) {drops.strikes.insert(empty, strike);}

            step = unlocked;
            strike = empty = 0;
            while (_FL(step)) {
                    if (step & (_sieve.all ^ _sieve.routeFL.prey)) {break;}
                    if (strike) {empty |= step; drops.strike |= step;}
                    if (step & _sieve.routeFL.prey) {
                            if (strike) {break;}
                            else {
                                    strike = step;
                                    _sieve.moves.hunter |= unlocked;
                                    _sieve.moves.prey |= step;
                            }
                    }
                    if (step & _sieve.empty) {drops.available |= step;}
            }
            if (strike) {drops.strikes.insert(empty, strike);}

            step = unlocked;
            strike = empty = 0;
            while (_BR(step)) {
                    if (step & (_sieve.all ^ _sieve.routeBR.prey)) {break;}
                    if (strike) {empty |= step; drops.strike |= step;}
                    if (step & _sieve.routeBR.prey) {
                            if (strike) {break;}
                            else {
                                    strike = step;
                                    _sieve.moves.hunter |= unlocked;
                                    _sieve.moves.prey |= step;
                            }
                    }
                    if (step & _sieve.empty) {drops.available |= step;}
            }
            if (strike) {drops.strikes.insert(empty, strike);}

            step = unlocked;
            strike = empty = 0;
            while (_BL(step)) {
                    if (step & (_sieve.all ^ _sieve.routeBL.prey)) {break;}
                    if (strike) {empty |= step; drops.strike |= step;}
                    if (step & _sieve.routeBL.prey) {
                            if (strike) {break;}
                            else {
                                    strike = step;
                                    _sieve.moves.hunter |= unlocked;
                                    _sieve.moves.prey |= step;
                            }
                    }
                    if (step & _sieve.empty) {drops.available |= step;}
            }
            if (strike) {drops.strikes.insert(empty, strike);}

            _sieve.moves.drops.insert(unlocked, drops);
        }
    }	while (mask<<=1);
    (void)0;
}

//	сдвиги
uint32_t Solver::_playerFR(const uint32_t &cells)
{
    if (_sieve.color) {
        return ((cells & _sieve.maskEvenFR)<<4) | ((cells & _sieve.maskOddFR)<<3);
    }
    else {
        return ((cells & _sieve.maskEvenBL)>>3) | ((cells & _sieve.maskOddBL)>>4);
    }
}

uint32_t Solver::_playerBL(const uint32_t &cells)
{
    if (!_sieve.color) {
        return ((cells & _sieve.maskEvenFR)<<4) | ((cells & _sieve.maskOddFR)<<3);
    }
    else {
        return ((cells & _sieve.maskEvenBL)>>3) | ((cells & _sieve.maskOddBL)>>4);
    }
}

uint32_t Solver::_playerFL(const uint32_t &cells)
{
    if (_sieve.color) {
        return ((cells & _sieve.maskEvenFL)<<5) | ((cells & _sieve.maskOddFL)<<4);
    }
    else {
        return ((cells & _sieve.maskEvenBR)>>4) | ((cells & _sieve.maskOddBR)>>5);
    }
}

uint32_t Solver::_playerBR(const uint32_t &cells)
{
    if (!_sieve.color) {
        return ((cells & _sieve.maskEvenFL)<<5) | ((cells & _sieve.maskOddFL)<<4);
    }
    else {
        return ((cells & _sieve.maskEvenBR)>>4) | ((cells & _sieve.maskOddBR)>>5);
    }
}

uint32_t Solver::_enemyFR(const uint32_t &cells)
{
    if (!_sieve.color) {
        return ((cells & _sieve.maskEvenFR)<<4) | ((cells & _sieve.maskOddFR)<<3);
    }
    else {
        return ((cells & _sieve.maskEvenBL)>>3) | ((cells & _sieve.maskOddBL)>>4);
    }
}

uint32_t Solver::_enemyBL(const uint32_t &cells)
{
    if (_sieve.color) {
        return ((cells & _sieve.maskEvenFR)<<4) | ((cells & _sieve.maskOddFR)<<3);
    }
    else {
        return ((cells & _sieve.maskEvenBL)>>3) | ((cells & _sieve.maskOddBL)>>4);
    }
}

uint32_t Solver::_enemyFL(const uint32_t &cells)
{
    if (!_sieve.color) {
        return ((cells & _sieve.maskEvenFL)<<5) | ((cells & _sieve.maskOddFL)<<4);
    }
    else {
        return ((cells & _sieve.maskEvenBR)>>4) | ((cells & _sieve.maskOddBR)>>5);
    }
}

uint32_t Solver::_enemyBR(const uint32_t &cells)
{
    if (_sieve.color) {
        return ((cells & _sieve.maskEvenFL)<<5) | ((cells & _sieve.maskOddFL)<<4);
    }
    else {
        return ((cells & _sieve.maskEvenBR)>>4) | ((cells & _sieve.maskOddBR)>>5);
    }
}

bool Solver::_FR(uint32_t &point)
{
    if (_sieve.color) {
            if (point & _sieve.maskEvenFR) {point &= _sieve.maskEvenFR; point<<=4;}
            else {point &= _sieve.maskOddFR; point<<=3;}
    }
    else {
            if (point & _sieve.maskEvenBL) {point &= _sieve.maskEvenBL; point>>=3;}
            else {point &= _sieve.maskOddBL; point>>=4;}
    }
    return point;
}

bool Solver::_BL(uint32_t &point)
{
    if (!_sieve.color) {
            if (point & _sieve.maskEvenFR) {point &= _sieve.maskEvenFR; point<<=4;}
            else {point &= _sieve.maskOddFR; point<<=3;}
    }
    else {
            if (point & _sieve.maskEvenBL) {point &= _sieve.maskEvenBL; point>>=3;}
            else {point &= _sieve.maskOddBL; point>>=4;}
    }
    return point;
}

bool Solver::_FL(uint32_t &point)
{
    if (_sieve.color) {
            if (point & _sieve.maskEvenFL) {point &= _sieve.maskEvenFL; point<<=5;}
            else {point &= _sieve.maskOddFL; point<<=4;}
    }
    else {
            if (point & _sieve.maskEvenBR) {point &= _sieve.maskEvenBR; point>>=4;}
            else {point &= _sieve.maskOddBR; point>>=5;}
    }
    return point;
}

bool Solver::_BR(uint32_t &point)
{
    if (!_sieve.color) {
            if (point & _sieve.maskEvenFL) {point &= _sieve.maskEvenFL; point<<=5;}
            else {point &= _sieve.maskOddFL; point<<=4;}
    }
    else {
            if (point & _sieve.maskEvenBR) {point &= _sieve.maskEvenBR; point>>=4;}
            else {point &= _sieve.maskOddBR; point>>=5;}
    }
    return point;
}
