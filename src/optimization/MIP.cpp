//==============================================================================
// MIP.cc
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

#include "optimization/MIP.h"


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------
//#define CPLEX_BEFORE
//#define CPLEX_AFTER


//------------------------------------------------------------------------------
//	Protected Variables
//------------------------------------------------------------------------------
//int	MIP::_counter = 0;
//bool	MIP::_isFound = false;
double	MIP::laptime = 0.0;


//------------------------------------------------------------------------------
//	Embedded Functions
//------------------------------------------------------------------------------
ILOHEURISTICCALLBACK2( Rounddown, IloNumVarArray, vars, IloNumArray, vals )
{
    IntegerFeasibilityArray	feas;
    IloNumArray			obj;
    try {
	// cerr << "***** Now the solution has been set ****" << endl;
	setSolution( vars, vals );
    }
    catch (...) {
	throw;
    }
}


ILOSOLVECALLBACK3( UserSolve, IloNumVarArray, vars,
		   IloNumArray, x,
		   IloBool, done ) {
   if ( !done ) {
       cerr << "***** Now the initial solution has been set ****" << endl;
       cerr << "\a\a\a\a";
       setStart( x, vars, 0, 0 );
       //setVectors( x, vars, 0, 0 ); // removed since v.127, use setStart() instead
       //solve( IloCplex::Primal );
       solve();
       if ( getStatus() == IloAlgorithm::Optimal ) useSolution();
       done = IloTrue;
   }
}


ILOMIPINFOCALLBACK5( timeLimitCallback,
		     IloCplex, cplex,
		     IloBool,  aborted,
		     IloNum,   timeStart,
		     IloNum,   timeLimit,
		     IloNum,   acceptableGap )
{
   if ( !aborted  &&  hasIncumbent() ) {
      IloNum gap = 100.0 * getMIPRelativeGap();
      IloNum getTime = cplex.getCplexTime();
      IloNum lapUsed = getTime - MIP::laptime;
      IloNum timeUsed = getTime - timeStart;
      if ( lapUsed > 10.0 )  {
	  cerr << "time used = " << timeUsed << endl;
	  MIP::laptime = getTime;
      }
      if ( timeUsed > timeLimit || gap < acceptableGap ) {
         getEnv().out() << endl
                        << "Good enough solution at "
                        << timeUsed << " sec., gap = "
                        << gap << "%, quitting." << endl;
         aborted = IloTrue;
         abort();
      }
   }
}

// Log new incumbents if they are at better than the old by a
// relative tolerance of 1e-5; also log progress info every
// 100 nodes.

ILOMIPINFOCALLBACK3(loggingCallback,
                    IloNumVarArray, vars,
                    IloNum,         lastLog,
                    IloNum,         lastIncumbent)
{
   int newIncumbent = 0;
   int nodes = getNnodes();

   if ( hasIncumbent()                                  &&
        fabs(lastIncumbent - getIncumbentObjValue())
              > 1e-5*(1.0 + fabs(getIncumbentObjValue())) ) {
      lastIncumbent = getIncumbentObjValue();
      newIncumbent = 1;
   }

   if ( nodes >= lastLog + 100  ||  newIncumbent ) {

      if ( !newIncumbent )  lastLog = nodes;
      getEnv().out() << "Nodes = " << nodes
                     << '(' << getNremainingNodes() << ')'
                     << "  Best objective = " << getBestObjValue();

      if ( hasIncumbent() ) {
         getEnv().out() << "  Incumbent objective = " << getIncumbentObjValue()
                        << endl;
      }
      else {
         getEnv().out() << endl;
      }

   }
   if ( newIncumbent ) {
      IloNumArray val(vars.getEnv());
      getIncumbentValues(val, vars);
      val[0] = getIncumbentValue(vars[0]);
      getEnv().out() << "New incumbent variable values: " << endl
                     << val << endl;
      val.end();
   }
}


//------------------------------------------------------------------------------
//	Private Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Protected Functions
//------------------------------------------------------------------------------

void MIP::_config( void )
{
    _limitTime		= DEFAULT_LIMIT_TIME;
    _limitGap		= DEFAULT_GAP;
}


//------------------------------------------------------------------------------
//	Public Functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
//
//  MIP::MIP --	default constructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
MIP::MIP()
{
    _isLazy = false;
    _config();
    _runTime = 0.0;
}


//
//  MIP::MIP --	copy constructor
//
//  Inputs
//	obj	: object of this class
//
//  Outputs
//	none
//
MIP::MIP( const MIP & obj )
{
    _model		= obj._model;
    _env		= obj._env;
    _x			= obj._x;
    _constr		= obj._constr;
    _objective	= obj._objective;

    //_isFound	= obj._isFound;

    _limitTime	= obj._limitTime;
    _limitGap	= obj._limitGap;

    _nVars		= obj._nVars;
    _nConstrs	= obj._nConstrs;
    _runTime    = obj._runTime;
}


//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------

//
//  MIP::~MIP --	destructor
//
//  Inputs
//	none
//
//  Outputs
//	none
//
MIP::~MIP()
{
    // free();
}



void MIP::allocate( void )
{
//------------------------------------------------------------------------------
//	Initialize the linear programming problem
//------------------------------------------------------------------------------
    _env	= IloEnv();
    _model	= IloModel	( _env );
    _objective	= IloMinimize	( _env );
    _cplex	= IloCplex	( _env );
    _cplex.extract( _model );
}


void MIP::build( void )
{
//------------------------------------------------------------------------------
//	Initialize the linear programming problem
//------------------------------------------------------------------------------
    _setVariables();
    _setConstraints();
    _solution.resize( _nVars );
    _useInitSol = false;
    _limitSols	= 0;
    cerr << "Finished building the MIP environment" << endl;
}

//
//  MIP::addModel --	add the objective and constraints to the model
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void MIP::addModel( void )
{
    cerr << "Adding the objective and constraints to the model..." << endl;

    try {

        _model.add( _objective );
        _model.add( _constr );
        //if( _isLazy == true ) _cplex.use( lazyCallback( _env, _x, _lazyCuts, _lazyBase ) );
    }
    catch ( IloCplex::Exception& e ) {
        cerr << "Cplex exception caught: " << e << endl;
        int errcode = e.getStatus();
        cerr << "	getStatus = " << errcode << endl;
    }
    catch ( IloException& e ) {
        cerr << "Concert exception caught: " << e << endl;
        e.end();
    }
    catch (...) {
        cerr << "Unknown exception caught" << endl;
    }
}

//
//  MIP::solve --	solve the mixed integer programming problem
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void MIP::solve( void )
{
//------------------------------------------------------------------------------
//      Initialize coordStation
//------------------------------------------------------------------------------

    cerr << "Now solving the constrained optimization problem" << endl;

#ifdef CPLEX_BEFORE
    for ( unsigned int k = 0; k < _nVars; ++k ) {
	    cout << "V[ " << setw( 4 ) << k << " ] = " << _x[ k ] << endl;
    }
    for ( unsigned int k = 0; k < _nConstrs; ++k ) {
        //for ( unsigned int k = 0; k < MIN2( 10000, _nConstrs ); ++k ) {
        cout << "C[ " << setw( 4 ) << k << " ] = " << _constr[ k ] << endl;
    }
    for ( unsigned int k = 0; k < _nLazy; ++k ) {
        for ( unsigned int k = 0; k < MIN2( 10000, _nConstrs ); ++k ) {
        cout << "L[ " << setw( 4 ) << k << " ] = " << _lazyCuts[ k ] << endl;
    }
#endif	// CPLEX_BEFORE

    try {

	//_model.add( _objective );
	//_model.add( _constr );
    //if( _isLazy == true ) _cplex.use( lazyCallback( _env, _x, _lazyCuts, _lazyBase ) );

    //if( _userCuts.getSize() != 0 ) _model.add( _userCuts );
    //if( _lazyCuts.getSize() != 0 ) _cplex.addLazyConstraints( _lazyCuts );
    //_cplex.addLazyConstraint( _lazyConstr );
    //_cplex.addLazyConstraints( _lazyCuts );
	// IloCplex cplex( _model );

    cerr << "Now solving the MIP problems .." << endl;

//#ifdef STARTONLY_VERSION
	if ( _useInitSol ) {
	    cerr << "***** No of initial solutions = " <<  _solution.size() << endl;
	    cerr << "***** No of variables = " <<  _nVars << endl;
	    IloNumArray initSol( _env, _nVars );
	    for ( unsigned int k = 0; k < _nVars; ++k )
		initSol[ k ] = 0.0;
	    for ( unsigned int k = 0; k < _solution.size(); ++k )
		initSol[ k ] = round( _solution[ k ] );
	    _env.out() << "Init Values        = " << initSol << endl;

	    _cplex.use( UserSolve( _env, _x, initSol, IloFalse ) );
	    _cplex.setParam( IloCplex::RootAlg, IloCplex::AutoAlg );
	    // cplex.setParam( IloCplex::MIPSearch, IloCplex::Traditional);
	    //_cplex.setParam( IloCplex::RootAlg, IloCplex::Network );
	}
	//#endif	// STARTONLY_VERSION
#ifdef STEPWISE_VERSION
	if ( _useInitSol ) {
	    cerr << "***** No of initiail solutions = " <<  _solution.size() << endl;
	    cerr << "***** No of variables = " <<  _nVars << endl;
	    IloNumArray initSol( _env, _nVars );
	    for ( unsigned int k = 0; k < _nVars; ++k )
		initSol[ k ] = 0.0;
	    for ( unsigned int k = 0; k < _solution.size(); ++k )
		initSol[ k ] = round( _solution[ k ] );
	    _env.out() << "Init Values        = " << initSol << endl;

	    _cplex.use( Rounddown( _env, _x, initSol ) );
	}
#endif	// #ifdef STEPWISE_VERSION
	else {

	    // set parameters
	    // default
	    _cplex.setParam( IloCplex::RootAlg, IloCplex::AutoAlg );
	    //_cplex.setParam( IloCplex::MIPSearch, IloCplex::Traditional );
	    // _cplex.setParam( IloCplex::RootAlg, IloCplex::AutoAlg );
	    // primal simplex
	    // _cplex.setParam( IloCplex::RootAlg, IloCplex::Primal );
	    // dual simplex
	    // _cplex.setParam( IloCplex::RootAlg, IloCplex::Dual );
	    // barrier with crossover
	    // _cplex.setParam( IloCplex::RootAlg, IloCplex::Barrier );
	    // barrier without crossover
	    // _cplex.setParam( IloCplex::RootAlg, IloCplex::Barrier );
	    // _cplex.setParam( IloCplex::BarCrossAlg, IloCplex::NoAlg );
	    // network with dual simplex cleanup
	    // _cplex.setParam( IloCplex::RootAlg, IloCplex::Network );
	    // sifting
	    // _cplex.setParam( IloCplex::RootAlg, IloCplex::Sifting );
	    // concurrent
	    // _cplex.setParam( IloCplex::RootAlg, IloCplex::Concurrent );

#ifdef DEBUG
        _cplex.setParam( IloCplex::MIPSearch, IloCplex::Traditional );
        _cplex.setParam( IloCplex::AggInd, CPX_OFF );
        _cplex.setParam( IloCplex::PerInd, CPX_OFF );
        _cplex.setParam( IloCplex::Reduce, CPX_OFF );
#endif // DEBUG
    }

//------------------------------------------------------------------------------
//	Callbacks
//------------------------------------------------------------------------------
	IloBool useLoggingCallback = IloFalse;
	//IloBool useLoggingCallback = IloTrue;
	//IloBool useTimeLimitCallback = IloFalse;
	IloBool useTimeLimitCallback = IloTrue;
	IloBool useAborter = IloFalse;

    IloCplex::Aborter myAborter;
	if ( useLoggingCallback ) {
	    IloNum lastObjVal = ( _objective.getSense() == IloObjective::Minimize ) ?
		IloInfinity : -IloInfinity;
	    _cplex.use( loggingCallback( _env, _x, -100000, lastObjVal ) );
	    // Turn off CPLEX logging
	    _cplex.setParam( IloCplex::MIPDisplay, 0 );
	}
	else if ( useTimeLimitCallback ) {
	    //_cplex.use( timeLimitCallback( _env, _cplex, IloFalse, cplex.getCplexTime(), 120.0, 95.0 ) );
	    _cplex.use( timeLimitCallback( _env, _cplex, IloFalse, _cplex.getCplexTime(),
					  _limitTime, _limitGap ) );
	    //_cplex.use( timeLimitCallback( _env, _cplex, IloFalse, cplex.getCplexTime(), 120.0, 20.0 ) );
	    //_cplex.use( timeLimitCallback( _env, _cplex, IloFalse, cplex.getCplexTime(), 120.0, 0.0 ) );
	}
	else if ( useAborter ) {
	    myAborter = IloCplex::Aborter( _env );
	    _cplex.use( myAborter );
	    // Typically, you would pass the Aborter object to
	    // another thread or pass it to an interrupt handler,
	    // and  monitor for some event to occur.  When it does,
	    // call the Aborter's abort method.
	    //
	    // To illustrate its use without creating a thread or
	    // an interrupt handler, abort immediately by calling
	    // abort before the solve.
	    //
	    myAborter.abort();
	}

#ifdef ENABLE_TIMER
    IloTimer timer(_env);
    timer.restart();
	laptime = _cplex.getCplexTime();
#endif	// ENABLE_TIMER

    // _cplex.exportModel( "../mps/floorplan.lp" );
    // _cplex.exportModel( "../mps/floorplan.mps" );
    //_cplex.exportModel( "../mps/floorplan-lazy.mps" );

	if ( !_cplex.solve() ) {
	    _env.error() << "Failed to optimize MIP." << endl;
	    throw( -1 );
	}

#ifdef ENABLE_TIMER
        timer.stop();
        _runTime += timer.getTime();
        cout << "************"  << endl;
        cout << "MIP runTime = " << _runTime/10000.0 << endl;
        cout << "************"  << endl;
#endif	// ENABLE_TIMER

	_env.out() << "Solution status = " << _cplex.getStatus() << endl;
    _env.out() << "Solution value = " << _cplex.getObjValue() << endl;
	//cerr << "########## Here 1" << endl;
    IloNumArray vals( _env );
	_cplex.getValues( vals, _x );

	//cerr << "########## Here 2" << endl;
#ifdef CPLEX_AFTER
	for ( unsigned int k = 0; k < _nVars; ++k ) {
	    cerr << "V[ " << setw( 4 ) << k << " ] = " << _x[ k ] << " *** " << vals[ k ] << endl;
	}
#endif	// CPLEX_AFTER


	//cerr << "########## Here 3" << endl;
	IloNumArray slacks( _env );
	_cplex.getSlacks( slacks, _constr );
#ifdef CPLEX_AFTER
	for ( unsigned int k = 0; k < _nConstrs; ++k ) {
	    cerr << "C[ " << setw( 4 ) << k << " ] = " << _constr[ k ] << " *** " << slacks[ k ] << endl;
	}
#endif	// CPLEX_AFTER

	//cerr << "########## Here 4" << endl;

//------------------------------------------------------------------------------
//	Storing solutions
//------------------------------------------------------------------------------
	//cerr << "########## Here 5" << endl;
	_solution.resize( _nVars );
	for ( unsigned int k = 0; k < _nVars; ++k ) {
	    _solution[ k ] = vals[ k ];
	}

	//cerr << "########## Here 6" << endl;
#ifdef DEBUG
	for ( unsigned int k = 0; k < _nVars; ++k ) {
	    cerr << " val[ " << k << " ] = " << vals[ k ]
		 << " _solution[ " << k << " ] = " << _solution[ k ] << endl;
	}
#endif	// DEBUG


    }
    catch ( IloCplex::Exception& e ) {
        cerr << "Cplex exception caught: " << e << endl;
        int errcode = e.getStatus();
        cerr << "	getStatus = " << errcode << endl;
    }
    catch ( IloException& e ) {
    	cerr << "Concert exception caught: " << e << endl;
        e.end();
    }
    catch (...) {
	    cerr << "Unknown exception caught" << endl;
    }

    cerr << "\a\a\a\a\a";

}


//
//  MIP::retrieve --	retrieve solutions
//
//  Inputs
//	sol : solutions
//
//  Outputs
//	none
//
void MIP::retrieve( vector< double > & sol )
{
    sol = _solution;
}


//
//  MIP::free --	release memory
//
//  Inputs
//	none
//
//  Outputs
//	none
//
void MIP::free( void )
{
    //_solution.clear();
    _constr.endElements();
    _x.endElements();
    //_lazyCuts.endElements();
    //_userCuts.endElements();
    _constr.end();
    _x.end();
    //_lazyCuts.end();
    //_userCuts.end();
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
//  MIP::operator = --	assignement
//
//  Inputs
//	obj	: objects of this class
//
//  Outputs
//	this object
//
MIP & MIP::operator = ( const MIP & obj )
{
    if ( this != &obj ) {

    _model		= obj._model;
	_env		= obj._env;
	_x		= obj._x;
	_constr		= obj._constr;
	_objective	= obj._objective;

	//_isFound	= obj._isFound;

	_limitTime	= obj._limitTime;
	_limitGap	= obj._limitGap;

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
ostream & operator << ( ostream & stream, const MIP & obj )
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
istream & operator >> ( istream & stream, MIP & obj )
{
    // do nothing
    return stream;
}


// end of header file
// Do not add any stuff under this line.
