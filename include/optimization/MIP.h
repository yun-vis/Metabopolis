//==============================================================================
// MIP.h
//	: header file for mixed integer programming
//
//	This class is for providing the basement for MIP optimization.
//
//------------------------------------------------------------------------------
//
//				Date: Wed Nov 28 00:39:59 2012
//
//==============================================================================

#ifndef _MIP_H		// beginning of header file
#define _MIP_H		// notifying that this file is included

//------------------------------------------------------------------------------
//	Including Header Files
//------------------------------------------------------------------------------
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <cmath>
#include <fstream>
#include <algorithm>

using namespace std;

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

//#include "Common.h"

//------------------------------------------------------------------------------
//	Switching Macros
//------------------------------------------------------------------------------
#define ENABLE_TIMER


//------------------------------------------------------------------------------
//	Defining Macros
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Ordinary mip optimization setting
#define	INFINITY_LIMIT_TIME	(2147483647)
//#define DEFAULT_LIMIT_TIME	  (50) //   50 seconds
//#define DEFAULT_LIMIT_TIME	 (100) //  100 seconds
//#define DEFAULT_LIMIT_TIME	 (120) //  120 seconds
//#define DEFAULT_LIMIT_TIME	 (500) //  500 seconds
//#define DEFAULT_LIMIT_TIME	(1000) // 1000 seconds
//#define DEFAULT_LIMIT_TIME	(2000) // 2000 seconds
#define	DEFAULT_LIMIT_TIME	(2147483647)
//#define DEFAULT_GAP		(80.0)
//#define DEFAULT_GAP		(50.0)
//#define DEFAULT_GAP		(30.0)
#define DEFAULT_GAP		( 0.0)


//------------------------------------------------------------------------------
//	Defining Classes
//------------------------------------------------------------------------------

class MIP {

  private:

    double		    _limitTime;
    double		    _limitGap;
    double          _runTime;

  protected:

    bool            _isLazy;
    unsigned int    _lazyBase;
    unsigned int	_nVars;
    unsigned int	_nConstrs;
    unsigned int	_nLazy;     // lazy constraints
    unsigned int	_nCut;      // user cuts

    IloModel		_model;
    IloEnv		    _env;
    IloCplex		_cplex;
    IloNumVarArray	_x;
    IloRangeArray	_constr;
    IloObjective	_objective;

    // lazy constraints
    IloRangeArray	_lazyCuts;
    //IloNumVarArray	_lazyx;

    // user cuts
    IloRangeArray	_userCuts;

//------------------------------------------------------------------------------
    vector< double >	_solution;
    bool		    _useInitSol;
    unsigned int	_limitSols;

//------------------------------------------------------------------------------
    void		_config( void );

    virtual void	_setVariables( void ) = 0;
    virtual void	_setConstraints( void ) = 0;
    virtual void    _setLazyCuts( unsigned int & nRows ) = 0;
    virtual void    _setUserCuts( unsigned int & nRows ) = 0;

  public:

//------------------------------------------------------------------------------
//	Constructors
//------------------------------------------------------------------------------
    MIP();			// default constructor
    MIP( const MIP & obj );
				// copy constructor

//------------------------------------------------------------------------------
//	Destructor
//------------------------------------------------------------------------------
    ~MIP();			// destructor


    static double	laptime;


//------------------------------------------------------------------------------
//	Referrring to members
//------------------------------------------------------------------------------
    const unsigned int & nVars( void ) const {
	return _nVars;
    }

    const unsigned int & nConsts( void ) const {
	return _nConstrs;
    }

    const unsigned int & nLazy( void ) const {
	return _nLazy;
    }

    void setLimitTime( int __limitTime ) {
	cerr << " Limit Time = " << __limitTime/1000 << " seconds" << endl;
	_limitTime = __limitTime;
    }

    void setGap( double __limitGap ) {
	cerr << " MIP Gap = " << __limitGap << endl;
	_limitGap = __limitGap;
    }

#ifdef ENABLE_OVERLAP_CHECK
    vector< pair< EdgeDescriptor , EdgeDescriptor > > & conflict( void ) {
	return _conflict;
    }
#endif	// ENABLE_OVERLAP_CHECK


//------------------------------------------------------------------------------
//	Create the system of equations and find the solution
//------------------------------------------------------------------------------
#ifdef ENABLE_OVERLAP_CHECK
    void resolve	( vector< Coord2 > & coordStation );
    void resolveAll	( void );
#endif	// ENABLE_OVERLAP_CHECK
    virtual void reuse	( void ) { _useInitSol = true; }

    void allocate	( void );
    void build		( void );
    void addModel	( void );
    void solve		( void );
    void retrieve	( vector< double > & sol );
    void free		( void );

//------------------------------------------------------------------------------
//	Assignment opereators
//------------------------------------------------------------------------------
    MIP & operator = ( const MIP & obj );
				// assignment

//------------------------------------------------------------------------------
//	I/O functions
//------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & stream, const MIP & obj );
				// output
    friend istream & operator >> ( istream & stream, MIP & obj );
				// input

//------------------------------------------------------------------------------
//	Class name
//------------------------------------------------------------------------------
    virtual const char * className( void ) const { return "MIP"; }
				// class name
};


#endif // _MIP_H

// end of header file
// Do not add any stuff under this line.
