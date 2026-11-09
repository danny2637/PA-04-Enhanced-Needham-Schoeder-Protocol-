/*----------------------------------------------------------------------------
PA-04:  Part One Intro to Enhanced Needham-Schroeder Key-Exchange with TWO-way Authentication

FILE:   amal.c     SKELETON

Written By: 
     1- YOU  MUST   WRITE 
	 2- FULL NAMES  HERE   (or risk losing points )
Submitted on: 
     Insert the date of Submission here
----------------------------------------------------------------------------*/

#include <linux/random.h>
#include <time.h>
#include <stdlib.h>

#include "../myCrypto.h"

// Generate random nonces for Amal
void  getNonce4Amal( int which , Nonce_t  value )
{
	// Normally we generate random nonces using
	// RAND_bytes( (unsigned char *) value , NONCELEN  );
	// However, for grading purpose, we will use fixed values

	switch ( which ) 
	{
		case 1:		// the first nonce
			value[0] = 0x11223344 ;
			break ;

		case 2:		// the second nonce
			value[0] = 0xaabbccdd ;		
			break ;

		default:	// Invalid agrument. Must be either 1 or 2
			fprintf( stderr , "\n\nAmal trying to create an Invalid nonce\n exiting\n\n");
			exit(-1);
	}
}
	
//*************************************
// The Main Loop
//*************************************
int main ( int argc , char * argv[] )
{
    int      fd_A2K , fd_K2A , fd_A2B , fd_B2A  ;
    FILE    *log ;

    char *developerName = "Code by STUDENTS_LAST_NAMES" ;

    fprintf( stdout , "Starting Amal's      %s.\n" , developerName  ) ;
    
    if( argc < 5 )
    {
        printf("\nMissing command-line file descriptors: %s <getFr. KDC> <sendTo KDC> "
               "<getFr. Basim> <sendTo Basim>\n\n" , argv[0]) ;
        exit(-1) ;
    }
    fd_K2A    = ...... ;  // Read from KDC    File Descriptor
    fd_A2K    = ...... ;  // Send to   KDC    File Descriptor
    fd_B2A    = ...... ;  // Read from Basim  File Descriptor
    fd_A2B    = ...... ;  // Send to   Basim  File Descriptor

    log = fopen("amal/logAmal.txt" , "w" );
    if( ! log )
    {
        fprintf( stderr , "\nAmal's  %s. Could not create my log file\n" , developerName  ) ;
        exit(-1) ;
    }

    BANNER( log ) ;
    fprintf( log , "Starting Amal\n" ) ;
    BANNER( log ) ;

    fprintf( log , "\n<readFrom KDC> FD=%d , <sendTo KDC> FD=%d , "
                   "<readFrom Basim> FD=%d , <sendTo Basim> FD=%d\n\n" , 
                   fd_K2A , fd_A2K , fd_B2A , fd_A2B );

    // Get Amal's master key with the KDC
    myKey_t  Ka ;  // Amal's master key with the KDC


    // Use  getKeyFromFile( "amal/amalKey.bin" , .... ) )
	// On failure, print "\nCould not get Amal's Masker key & IV.\n" to both  stderr and the Log file
	// and exit(-1)
	// On success, print "Amal has this Master Ka { key , IV }\n" to the Log file
	// BIO_dump the Key IV indented 4 spaces to the righ
    fprintf( log , "\n" );
	// BIO_dump the IV indented 4 spaces to the righ


    // Get Amal's pre-created Nonces: Na and Na2
	Nonce_t   Na , Na2; 
    fprintf( log , "Amal will use these Nonces:  Na  and Na2\n"  ) ;
	// Use getNonce4Amal () to get Amal's 1st and second nonces into Na and Na2, respectively
	// BIO_dump Na indented 4 spaces to the righ
    fprintf( log , "\n" );
	// BIO_dump Na2 indented 4 spaces to the righ
    fprintf( log , "\n") ; 

    fflush( log ) ;

    //*************************************
    // Construct & Send    Message 1
    //*************************************
    BANNER( log ) ;
    fprintf( log , "         MSG1 New\n");
    BANNER( log ) ;

    char *IDa = "Amal is Hope", *IDb = "Basim is Smiley" ;
    size_t  LenMsg1 ;
    uint8_t  *msg1 ;
    LenMsg1 = MSG1_new( log , &msg1 , IDa , IDb , Na ) ;
    
    // Send MSG1 to KDC via the appropriate pipe

   fprintf( log , "Amal sent message 1 ( %lu bytes ) to the KDC with:\n    "
                   "IDa ='%s'\n    "
                   "IDb = '%s'\n" , LenMsg1 , IDa , IDb ) ;
    fprintf( log , "    Na ( %lu Bytes ) is:\n" , NONCELEN ) ;
    // BIO_dump the nonce Na
    fflush( log ) ;

    // Deallocate any memory allocated for msg1


    // PA-04 Part Two
    // will go here


    //*************************************   
    // Final Clean-Up
    //*************************************
   
    fprintf( log , "\nAmal has terminated normally. Goodbye\n" ) ;  
    fclose( log ) ;
    return 0 ;
}

