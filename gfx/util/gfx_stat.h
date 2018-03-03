/*
 * =====================================================================================
 *
 *       Filename:  gfx_stat.h
 *
 *    Description:  statistical basics
 *
 *        Version:  1.0
 *        Created:  02/02/2015 19:09:11
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  pretty awesome 
 *
 * =====================================================================================
 */

#ifndef gfx_Prob_h
#define gfx_Prob_h

#include <stdlib.h>
#include <time.h> 
#include <math.h>

namespace gfx{

/*! 
 Probability Density Functions
 */
    struct Rand {
        

        /// Seed 
        
        // inline static void Seed(int num){
        //     srand(num);
        //     gen.seed( num );
        // }  

        inline static void Seed() { 
            
            //typical srand
            srand( time(NULL) ); 
            //gen.seed( time(NULL) );
            
        }
        
        /// Number Between 0 and 1; 
        inline static double Num() { return 1.0 * rand() / (RAND_MAX); }
        
        /// Number Between 0 and max
        inline static double Num(double max) { return max * rand() / (RAND_MAX); }

        /// Number Between low and high
        inline static double Num(double low, double high){
            return low + ( (high-low) * rand() / RAND_MAX );
        }

        /// Random Boolean
        inline static bool Boolean() { return rand() & 1; }            

        /// Integer between high and low
        inline static int Int( int high, int low = 0 ) {
            int diff = high - low;

            int result = floor( Num(diff+1) );

            return low + result;
        }
        
        // inline static double Poisson(double mean) {            
        //     poisson_distribution<double> poisson(mean);
        //     return poisson( gen );
        // } 
        
        /// Uniform Distribution [0,1)
        // inline static int Uniform(int low, int high){
        //     uniform_int<int> uniform(low, high);
        //     return uniform( gen );
        // } 

        /// Uniform Distribution [0,1)
        inline static double Uniform(){
            //uniform_real<double> uniform(0,1);
            //return uniform( gen );
            return Num();
        }

        /// Normal (Gaussian) Distribution        
        // inline static double Normal(double mean, double dev) {
        //     variate_generator< mt19937, normal_distribution<double> > gaussian( mt19937(gen), normal_distribution<double>(mean, dev) )  ;
        //     //normal_distribution<double> gaussian(mean, dev) ;
        //     return gaussian();
        // }    
        
     
        /// Exponential Distribution
        // inline static double Exponential(double rate){
        //     exponential_distribution<double> exponential(rate);
        //     return exponential( gen );        
        // }   

        /// Power Law Distribution
        inline static double Pareto(double scale, double shape=1.0){
            //return exp( Exponential(rate) );
            return scale / ( pow( 1 - Num(), 1.0 / shape ) );
        }
        
    };
    
    struct Eval {
        inline static double Gaussian(double x, double b= 1, double c = 1, double a = 1){
            double num = (x-b);
            double den = 2 * c * c;
            return a * exp( -(num*num) / den ) ;
        }

    };
        
    class Stat {
      
        public:
        
        inline static bool Gaussian(double x, double b= 1, double c = 1, double a = 1){
            return Prob( Eval::Gaussian(x,b,c,a) );
        }
        
        inline static bool Power(double p, double k, double a = 1){
            return Prob( a * pow(p,k) );
        }
        
        /// Bernoulli: Returns true with a probability of p
        inline static bool Prob(double p){ 
            return ( Rand::Num() <= p ) ? 1 : 0;
        }
    };
}


#endif

