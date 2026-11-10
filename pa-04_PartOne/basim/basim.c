/*----------------------------------------------------------------------------
PA-04:  Part One Intro to Enhanced Needham-Schroeder Key-Exchange with TWO-way Authentication

FILE:   basim.c

Written By:
     1- Samuel George
	 2- Danny Ramos
Submitted on:
     11/10/2026
----------------------------------------------------------------------------*/

#include <linux/random.h>
#include <time.h>
#include <stdlib.h>

#include "../myCrypto.h"

// Generate random nonces for Basim
void  getNonce4Basim( int which , Nonce_t  value )
{
	// Normally we generate random nonces using
	// RAND_bytes( (unsigned char *) value , NONCELEN  );
	// However, for grading purpose, we will use fixed values

	switch ( which )
	{
		case 1:		// the first and Only nonce
			value[0] = 0x66778899 ;
			break ;

		default:	// Invalid agrument. Must be either 1 or 2
			fprintf( stderr , "\n\nBasim trying to create an Invalid nonce\n exiting\n\n");
			exit(-1);
	}
}

//*************************************
// The Main Loop
//*************************************
int main ( int argc , char * argv[] )
{
    int       fd_A2B , fd_B2A   ;
    FILE     *log ;

    char *developerName = "Code by George & Ramos" ;

    fprintf( stdout , "Starting Basim's     %s\n" , developerName ) ;

    if( argc < 3 )
    {
        printf("\nMissing command-line file descriptors: %s <getFr. Amal> "
               "<sendTo Amal>\n\n", argv[0]) ;
        exit(-1) ;
    }

    fd_A2B    = atoi( argv[1] ) ;  // Read from Amal   File Descriptor
    fd_B2A    = atoi( argv[2] ) ;  // Send to   Amal   File Descriptor

    log = fopen("basim/logBasim.txt" , "w" );
    if( ! log )
    {
        fprintf( stderr , "Basim's %s. Could not create log file\n" , developerName ) ;
        exit(-1) ;
    }

    BANNER( log ) ;
    fprintf( log , "Starting Basim\n"  ) ;
    BANNER( log ) ;

    fprintf( log , "\n<readFrom Amal> FD=%d , <sendTo Amal> FD=%d\n\n" , fd_A2B , fd_B2A );

    // Get Basim's master keys with the KDC
    myKey_t   Kb ;    // Basim's master key with the KDC

    // Use  getKeyFromFile( "basim/basimKey.bin" , .... ) )
    if (! getKeyFromFile( "basim/basimKey.bin", &Kb))
    {
        fprintf( stderr, "\nCould not get Basim's Master Key & IV.\n");
        fprintf( log, "\nCould not get Basim's Master Key & IV.\n");
        fclose( log ) ;
        exit(-1);
    }
	// On failure, print "\nCould not get Basim's Masker key & IV.\n" to both  stderr and the Log file
	// and exit(-1)
    fprintf(log, "Basim has this Master Kb { key , IV }\n");
	// On success, print "Basim has this Master Kb { key , IV }\n" to the Log file
    BIO_dump_indent_fp( log, Kb.key, SYMMETRIC_KEY_LEN, 4);
	// BIO_dump the Key IV indented 4 spaces to the righ
    fprintf( log , "\n" );
	// BIO_dump the IV indented 4 spaces to the righ
    BIO_dump_indent_fp( log, Kb.iv, INITVECTOR_LEN, 4);
    fprintf( log, "\n" );


    // Get Basim's pre-created Nonces: Nb
	Nonce_t   Nb;

	// Use getNonce4Basim () to get Basim's 1st and only nonce into Nb
    getNonce4Basim( 1, Nb);
    fprintf( log , "Basim will use this Nonce:  Nb\n"  ) ;
    BIO_dump_indent_fp( log, Nb, NONCELEN, 4);

	// BIO_dump Nb indented 4 spaces to the righ
    fprintf( log , "\n" );

    fflush( log ) ;


    // PA-04 Part Two
    // will go here


    //*************************************
    // Final Clean-Up
    //*************************************

    fprintf( log , "\nBasim has terminated normally. Goodbye\n" ) ;
    fclose( log ) ;

    return 0 ;
}
