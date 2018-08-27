//==============================================================================
// QMIP.cc
//	: program file for mixed integer programming
//
//------------------------------------------------------------------------------
//
//				Date: Fri Dec  7 22:34:19 2012
//
//==============================================================================
//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------

#include <cassert>

// using namespace std;

#include "optimization/QMIP.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Embedded Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  QMIP::QMIP --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
QMIP::QMIP()
{
    _config();
}


//
//  QMIP::QMIP --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
QMIP::QMIP( const QMIP & obj )
{
    _model		= obj._model;
    _env		= obj._env;
    _x			= obj._x;
    _constr		= obj._constr;
    _objective	= obj._objective;

    //_isFound	= obj._isFound;

    _nVars		= obj._nVars;
    _nConstrs	= obj._nConstrs;

    //_limitTime  = obj._limitTime;
    //_limitGap	  = obj._limitGap;
    //_runTime    = obj._runTime;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  QMIP::~QMIP --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
QMIP::~QMIP()
{
    // free();
}



void QMIP::allocate( void )
{

//------------------------------------------------------------------------------
//	Initialize the linear programming problem
//------------------------------------------------------------------------------
    _env	    = IloEnv();
    _model	    = IloModel( _env );

    _objExpr    = IloExpr( _env );
    _objective  = IloMinimize( _env, _objExpr );
    //_objective	= IloMinimize( _env );

    _cplex	    = IloCplex( _env );
    _cplex.extract( _model );

}


void QMIP::solve( void )
{
    // When a non-convex objective function is present, CPLEX will
    // raise an exception unless the parameter
    // IloCplex::Param::SolutionTarget is set to accept first-order optimal
    // solutions
    //_cplex.setParam( IloCplex::Param::OptimalityTarget, IloCplex::OptimalityAuto );
    _cplex.setParam( IloCplex::Param::OptimalityTarget, IloCplex::OptimalityOptimalConvex );
    //_cplex.setParam( IloCplex::Param::OptimalityTarget, IloCplex::OptimalityFirstOrder );
    //_cplex.setParam( IloCplex::Param::OptimalityTarget, IloCplex::OptimalityOptimalGlobal );

    MIP::solve();
}

//
//  QMIP::free --	release memory
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void QMIP::free( void )
{
    //_solution.clear();
    _constr.endElements();
    _x.endElements();

    _constr.end();
    _x.end();
    _cplex.end();
    _model.end();
    _objective.end();
    _env.end();

    _useInitSol = false;
    _limitSols = 0;
}

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
//
//  QMIP::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
QMIP & QMIP::operator = ( const QMIP & obj )
{
    if ( this != &obj ) {

    _model		= obj._model;
	_env		= obj._env;
	_x		    = obj._x;
	_constr		= obj._constr;
	_objective	= obj._objective;

	//_isFound	= obj._isFound;

	//_limitTime	= obj._limitTime;
	//_limitGap	= obj._limitGap;

	_nVars		= obj._nVars;
	_nConstrs	= obj._nConstrs;
    }
    return *this;
}


//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
//
//  operator << --	output
//
//  Argument
//	stream	: output stream
//	obj	: object of this class
//
//  Outputs
//	output stream
//
ostream & operator << ( ostream & stream, const QMIP & obj )
{
    return stream;
}


//
//  operator >> --	input
//
//  Inputs
//	stream	: input stream
//	obj	: object of this class
//
//  Outputs
//	input stream
//
istream & operator >> ( istream & stream, QMIP & obj )
{
    // do nothing
    return stream;
}


// end of header file
// Do not add any stuff under this line.
