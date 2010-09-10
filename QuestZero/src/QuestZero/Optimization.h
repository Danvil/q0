/*
 * Optimization.h
 *
 *  Created on: Sep 10, 2010
 *      Author: david
 */

#ifndef QUESTZERO_OPTIMIZATION_H_
#define QUESTZERO_OPTIMIZATION_H_
//---------------------------------------------------------------------------
#include "Tracer/NoTracer.h"
#include "StartingValues.h"
#include "TakePolicy.h"
//---------------------------------------------------------------------------

template<
	typename State,
	class Algorithm,
	class StartingStates = StatePicker::RandomPicker<State>,
	class Take = TakePolicy::TakeBest<State>,
	class Tracer = NoTracer<State>
>
struct Optimization
: public Algorithm,
  public StartingStates,
  public Take,
  public Tracer
{
};

//---------------------------------------------------------------------------
#endif
