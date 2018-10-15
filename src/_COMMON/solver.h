#pragma once

#include <QMap>

#include "_comm_types.h"
#include "xeno.h"

class Solver
{
public:
//  TODO	отделить Drop:: от Sieve::
	struct Sieve	{
			static	const	uint32_t	trimBorderRight	= 0b11101111111011111110111111101111;
			static	const	uint32_t	trimBorderLeft	= 0b11110111111101111111011111110111;
			static	const	uint32_t	trimRowFirst	= 0b11111111111111111111111111110000;
			static	const	uint32_t	trimRowLast		= 0b00001111111111111111111111111111;
			static	const	uint32_t	trimFR			= trimRowLast & trimBorderRight;
			static	const	uint32_t	trimFL			= trimRowLast & trimBorderLeft;
			static	const	uint32_t	trimBL			= trimRowFirst & trimBorderLeft;
			static	const	uint32_t	trimBR			= trimRowFirst & trimBorderRight;
			static	const	uint32_t	trimPerimeter	= trimFR & trimBL;
			static	const	uint32_t	maskBorderRight	= ~trimBorderRight;
			static	const	uint32_t	maskBorderLeft	= ~trimBorderLeft;
			static	const	uint32_t	maskRowFirst	= ~trimRowFirst;
			static	const	uint32_t	maskRowLast		= ~trimRowLast;
			static	const	uint32_t	maskFR			= ~trimFR;
			static	const	uint32_t	maskFL			= ~trimFL;
			static	const	uint32_t	maskBL			= ~trimBL;
			static	const	uint32_t	maskBR			= ~trimBR;
			static	const	uint32_t	maskPerimeter	= ~trimPerimeter;
			static	const	uint32_t	maskOdd			= 0b11110000111100001111000011110000;
			static	const	uint32_t	maskEven		= ~maskOdd;
			static	const	uint32_t	maskEvenFR		= maskEven & trimFR;
			static	const	uint32_t	maskEvenBL		= maskEven & trimBL;
			static	const	uint32_t	maskEvenFL		= maskEven & trimFL;
			static	const	uint32_t	maskEvenBR		= maskEven & trimBR;
			static	const	uint32_t	maskOddFR		= maskOdd & trimFR;
			static	const	uint32_t	maskOddBL		= maskOdd & trimBL;
			static	const	uint32_t	maskOddFL		= maskOdd & trimFL;
			static	const	uint32_t	maskOddBR		= maskOdd & trimBR;

							bool		color;
							uint32_t	player;
							uint32_t	enemy;
							uint32_t	super;
							uint32_t	playerPerimeter;
							uint32_t	enemyPerimeter;
							uint32_t	enemyHoles;
							uint32_t	playerHoles;
							uint32_t	all;
							uint32_t	empty;

							struct Route	{
									uint32_t	interval;
									uint32_t	unlocked;
									uint32_t	hunterOrdinary;
									uint32_t	prey;
									uint32_t	preyOrdinary;
							} routeFL, routeFR, routeBL, routeBR;

							struct Moves {
									struct Drops {
											uint32_t					available	= 0b0;
											uint32_t					strike		= 0b0;
											uint32_t					super		= 0b0;
											QMap <uint32_t, uint32_t>	strikes;
									};
									QMap <uint32_t, Drops> drops;
									uint32_t	unlocked;
									uint32_t	hunter;
									uint32_t	prey;
							} moves;
	};

							Solver();
	void					SetMoves(const Xeno::Move::Snap & snap, Color color);
	Solver::Sieve::Moves *	getMoves();

private:
	Sieve		_sieve;
	uint32_t	_playerFR(const uint32_t &cells);
	uint32_t	_playerBL(const uint32_t &cells);
	uint32_t	_playerFL(const uint32_t &cells);
	uint32_t	_playerBR(const uint32_t &cells);
	uint32_t	_enemyFR(const uint32_t &cells);
	uint32_t	_enemyBL(const uint32_t &cells);
	uint32_t	_enemyFL(const uint32_t &cells);
	uint32_t	_enemyBR(const uint32_t &cells);
	bool		_FR(uint32_t &point);
	bool		_FL(uint32_t &point);
	bool		_BR(uint32_t &point);
	bool		_BL(uint32_t &point);
	void		_FindIntervals();
	void		_FindUnlocked();
	void		_FindPrey();
	void		_ProcessOrdinary();
	void		_ProcessSuper();
};

